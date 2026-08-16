#include <QByteArray>
#include <openssl/evp.h>

namespace Core::Crypto {
    class Decryption {
        public:
        bool init(const QByteArray& key, const QByteArray& iv, const QByteArray &tag);
        QByteArray decryptChunk(const QByteArray& chunk);
        QByteArray finish();

        ~Decryption();

        private:
        EVP_CIPHER_CTX *m_ctx = nullptr;
    };
};