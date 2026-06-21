#include "Decryption.h"

using Core::Crypto::Decryption;

bool Decryption::init(const QByteArray &key, const QByteArray &iv, const QByteArray &tag) {
    m_ctx = EVP_CIPHER_CTX_new();
    if (!m_ctx)
        return false;

    if (EVP_DecryptInit_ex(
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

    if (EVP_DecryptInit_ex(
        m_ctx, 
        nullptr, 
        nullptr,
        reinterpret_cast<const unsigned char*>(key.data()),
        reinterpret_cast<const unsigned char*>(iv.data())) != 1
    )
        return false;


    if (EVP_CIPHER_CTX_ctrl(
        m_ctx,
        EVP_CTRL_GCM_SET_TAG,
        160,
        const_cast<char *>(tag.data())) != 1
    )
        return false;

    return true;
}

QByteArray Decryption::decryptChunk(const QByteArray &chunk) {
    QByteArray out;
    out.resize(chunk.size() + 16);

    int outLen = 0;
    if (EVP_DecryptUpdate(
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

QByteArray Decryption::finish() {
    QByteArray finalBlock(16, Qt::Uninitialized);
    int len = 0;

    EVP_DecryptFinal_ex(
        m_ctx,
        reinterpret_cast<unsigned char*>(finalBlock.data()),
        &len
    );

    finalBlock.resize(len);

    EVP_CIPHER_CTX_free(m_ctx);
    m_ctx = nullptr;

    return finalBlock;
}

Decryption::~Decryption() {
    EVP_CIPHER_CTX_free(m_ctx);
}