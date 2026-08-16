#pragma once

#include <QObject>
#include <QByteArray>
#include <optional>

namespace Core::Crypto {
    struct WrappedKey {
        QByteArray encryptedKey;
        QByteArray iv;
        QByteArray tag;
    };

    class KeyManager : public QObject {
        Q_OBJECT        
        KeyManager(QObject *parent = nullptr);

        public:
        static KeyManager* instance();

        bool unlock(QByteArray &password, QByteArray &salt);
        void lock();
        const QByteArray& masterKey() const;

        static bool deriveKey(const QByteArray &password, const QByteArray &salt, QByteArray &outputBuffer);
        static QByteArray generateFileKey();
        
        WrappedKey wrapFileKey(const QByteArray &unwrappedKey);
        std::optional<QByteArray> unwrapFileKey(const WrappedKey &wrappedKey);

        private:
        bool _isUnlocked = false;
        QByteArray _masterKey;
    };
};