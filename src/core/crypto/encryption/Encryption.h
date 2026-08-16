#include <QByteArray>
#include <openssl/evp.h>

namespace Core::Crypto {
    class Encryption {
        public:
        bool init(const QByteArray& key, const QByteArray& iv);
        QByteArray encryptChunk(const QByteArray& chunk);
        QByteArray finish();
        QByteArray tag();

        private:
        EVP_CIPHER_CTX* m_ctx = nullptr;
    };
};