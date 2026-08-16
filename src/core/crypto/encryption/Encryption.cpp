#include "Encryption.h"

using Core::Crypto::Encryption;

bool Encryption::init(const QByteArray &key, const QByteArray &iv) {
    m_ctx = EVP_CIPHER_CTX_new();
    if (!m_ctx)
        return false;

    if (EVP_EncryptInit_ex(
        m_ctx, 
        EVP_aes_256_gcm(), 
        nullptr, 
        nullptr, 
        nullptr) != 1
    )
        return false;

    if (EVP_CIPHER_CTX_ctrl(
            m_ctx, 
            EVP_CTRL_GCM_SET_IVLEN, 
            iv.size(), 
            nullptr
        ) != 1
    )
        return false;

    if (EVP_EncryptInit_ex(
        m_ctx, 
        nullptr, 
        nullptr,
        reinterpret_cast<const unsigned char*>(key.data()),
        reinterpret_cast<const unsigned char*>(iv.data())) != 1
    )
        return false;

    return true;
}

QByteArray Encryption::encryptChunk(const QByteArray &chunk) {
    QByteArray out;
    out.resize(chunk.size() + 16);

    int outLen = 0;
    if (EVP_EncryptUpdate(
        m_ctx,
        reinterpret_cast<unsigned char*>(out.data()),
        &outLen,
        reinterpret_cast<const unsigned char*>(chunk.data()),
        chunk.size()) != 1)
    {
        return {};
    }

    out.resize(outLen);
    return out;
}

QByteArray Encryption::finish() {
    QByteArray finalBlock(16, Qt::Uninitialized);
    int len = 0;

    EVP_EncryptFinal_ex(
        m_ctx,
        reinterpret_cast<unsigned char*>(finalBlock.data()),
        &len
    );

    finalBlock.resize(len);

    EVP_CIPHER_CTX_free(m_ctx);
    m_ctx = nullptr;

    return finalBlock;
}

QByteArray Encryption::tag() {
    QByteArray tag(16, Qt::Uninitialized);

    EVP_CIPHER_CTX_ctrl(
        m_ctx,
        EVP_CTRL_GCM_GET_TAG,
        16,
        tag.data()
    );

    return tag;
}