#include "KeyManager.h"

#include <openssl/rand.h>
#include <openssl/evp.h>
#include "../../utils/Utils.h"

#include <QApplication>

using Core::Crypto::KeyManager;
KeyManager::KeyManager(QObject *parent) : QObject(parent),  _salt(QByteArray::fromHex("A1B2C3D4E5F60718293A4B5C6D7E8F90")) {

}

KeyManager* Core::Crypto::KeyManager::instance() {
    static KeyManager *km = new KeyManager(qApp);
    return km;
}

bool KeyManager::deriveKey(const QByteArray &password, const QByteArray &salt, QByteArray &outputBuffer)
{
    outputBuffer.resize(32); // AES-256 Key Size

    return PKCS5_PBKDF2_HMAC(
        password.constData(),
        password.size(),
        reinterpret_cast<const unsigned char *>(salt.constData()),
        salt.size(),
        100000,
        EVP_sha256(),
        32,
        reinterpret_cast<unsigned char *>(outputBuffer.data())
    ) == 1;
}

QByteArray KeyManager::generateFileKey() {
    QByteArray key(32, Qt::Uninitialized);
    RAND_bytes(reinterpret_cast<unsigned char *>(key.data()), key.size());
    return key;
}

Core::Crypto::WrappedKey KeyManager::wrapFileKey(const QByteArray &unwrappedKey) {
    if (!_isUnlocked || unwrappedKey.size() != 32)
        return {};
    
    Core::Crypto::WrappedKey wKey;

    wKey.iv.resize(12);
    if (RAND_bytes(
            reinterpret_cast<unsigned char *>(wKey.iv.data()), 
            wKey.iv.size()
        ) != 1
    )
        return {};

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        return {};

    if (EVP_EncryptInit_ex(
        ctx,
        EVP_aes_256_gcm(),
        nullptr,
        nullptr,
        nullptr) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    if (EVP_CIPHER_CTX_ctrl(
        ctx,
        EVP_CTRL_GCM_SET_IVLEN,
        wKey.iv.size(),
        nullptr) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    if (EVP_EncryptInit_ex(
        ctx,
        nullptr,
        nullptr,
        reinterpret_cast<const unsigned char*>(_masterKey.constData()),
        reinterpret_cast<const unsigned char*>(wKey.iv.constData())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }
    
    wKey.encryptedKey.resize(unwrappedKey.size());
    int outLen = 0;
    
    if (EVP_EncryptUpdate(
            ctx,
            reinterpret_cast<unsigned char *>(wKey.encryptedKey.data()),
            &outLen,
            reinterpret_cast<const unsigned char *>(unwrappedKey.constData()),
            unwrappedKey.size()
        ) != 1
    ) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    int finalLen = 0;
    if (EVP_EncryptFinal_ex(
            ctx,
            reinterpret_cast<unsigned char *>(wKey.encryptedKey.data()) + outLen,
            &finalLen
        ) != 1
    ) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    wKey.tag.resize(16);
    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_GET_TAG,
            16,
            wKey.tag.data()
        ) != 1
    ) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_free(ctx);

    return wKey;
}

std::optional<QByteArray> KeyManager::unwrapFileKey(const Core::Crypto::WrappedKey &wrappedKey) {
    if (_masterKey.size() != 32 || !_isUnlocked)
        return std::nullopt;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        return std::nullopt;

    if (EVP_DecryptInit_ex(
            ctx,
            EVP_aes_256_gcm(),
            nullptr,
            nullptr,
            nullptr
        ) != 1
    ) {
        EVP_CIPHER_CTX_free(ctx);
        return std::nullopt;
    }

    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_SET_IVLEN,
            12,
            nullptr
        ) != 1
    ) {
        EVP_CIPHER_CTX_free(ctx);
        return std::nullopt;
    }

    if (EVP_DecryptInit_ex(
            ctx,
            nullptr,
            nullptr,
            reinterpret_cast<const unsigned char *>(_masterKey.data()),
            reinterpret_cast<const unsigned char *>(wrappedKey.iv.data())
        ) != 1
    ){
        EVP_CIPHER_CTX_free(ctx);
        return std::nullopt;
    }

    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_SET_TAG,
            16,
            const_cast<char *>(wrappedKey.tag.data())
        ) != 1
    ) {
        EVP_CIPHER_CTX_free(ctx);
        return std::nullopt;
    }

    QByteArray result(32, Qt::Uninitialized);

    int outLen = 0;
    if (EVP_DecryptUpdate(
            ctx,
            reinterpret_cast<unsigned char *>(result.data()),
            &outLen,
            reinterpret_cast<const unsigned char *>(wrappedKey.encryptedKey.constData()),
            wrappedKey.encryptedKey.size()
        ) != 1
    ) {
        EVP_CIPHER_CTX_free(ctx);
        return std::nullopt;
    }

    int finalLen = 0;
    int ret = EVP_DecryptFinal_ex(
        ctx,
        reinterpret_cast<unsigned char *>(result.data()) + outLen,
        &finalLen
    );

    if (ret != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return std::nullopt;
    }

    EVP_CIPHER_CTX_free(ctx);

    return result;
}

bool KeyManager::unlock(QByteArray &password) {
    if (password.isEmpty())
        return false;
    
    // Salt (It will store on server later)
    // if (RAND_bytes(reinterpret_cast<unsigned char *>(_salt.data()), _salt.size()) != 1)
    //     return false;

    // Derive AES Key
    if (!deriveKey(password, _salt, _masterKey))
        return false;

    _isUnlocked = true;
    Utils::cleanupMemory(password);

    return true;
}

void KeyManager::lock() {
    _isUnlocked = false;
    Utils::cleanupMemory(_masterKey);
    Utils::cleanupMemory(_salt);
}

const QByteArray& KeyManager::masterKey() const {
    return _masterKey;
}
