#pragma once

#include "../../components/RoundedBox.h"

#include <QApplication>
#include <QVector>

class Button;
class SmoothOpacity;
class QPropertyAnimation;
class QVBoxLayout;

namespace Ui::Vault { class AccountSettings; };
namespace Ui::Vault {
    class UserMenu : public RoundedBox {
        Q_OBJECT

        public:
        explicit UserMenu(QWidget *parent = nullptr);
        void setAvator(const QPixmap &avator);
        void setName(const QString &name);
        void setEmail(const QString &email);
        void showAt(QWidget *anchorWidget);

        Button* accountSettingsButton() const;
        Button* manageSubscriptionButton() const;

        void fadeIn();
        void fadeOut();

        protected:
        void paintEvent(QPaintEvent *event) override;

        private:
        void setDarkMode(bool isDarkMode);
        void onAccountSettingsBtnClicked();

        // Data
        QPixmap _avator;
        QString _name, _email;

        // Option Buttons
        Button *account_settings_btn      = nullptr;
        // Account Settings Window
        Ui::Vault::AccountSettings *acc_settings_win = nullptr;

        Button *manage_subscription_btn   = nullptr;

        QVector<Button *> option_buttons;

        // Layout
        QVBoxLayout *layout = nullptr;

        // Animations Effects
        SmoothOpacity *smooth_opacity = nullptr;
        QPropertyAnimation *animation = nullptr;

        // Helpers
        Button* createButton(const QString &text, const QString &iconPath);
    };
};