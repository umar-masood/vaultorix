#pragma once

#include "../../components/RoundedBox.h"
#include "../../components/Button.h"
#include "../../components/SmoothOpacity.h"

#include "../accountSettings/AccountSettings.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QVector>

class UserMenu : public RoundedBox {
    Q_OBJECT

    public:
    explicit UserMenu(QWidget *parent = nullptr);

    void setDarkMode(bool enable);
    void setAvator(const QPixmap &avator);
    void setName(const QString &name);
    void setEmail(const QString &email);
    void showAt(QWidget *anchorWidget);

    Button* accountSettingsButton() const;
    Button* manageSubscriptionButton() const;
    Button* reportBugButton() const;

    void fadeIn();
    void fadeOut();

    protected:
    void paintEvent(QPaintEvent *event) override;

    private:
    // Theme Mode Flag
    bool isDarkMode = false;

    // Data
    QPixmap _avator;
    QString _name;
    QString _email;

    // Option Buttons
    Button *account_settings_btn      = nullptr;
        // Account Settings Window
        AccountSettingsWindow *acc_settings_win = nullptr;
        
    Button *manage_subscription_btn   = nullptr;
    Button *report_bug_btn            = nullptr;
    
    QVector<Button *> option_buttons;
    
    // Layout
    QVBoxLayout *layout = nullptr;

    // Animations Effects
    SmoothOpacity *smooth_opacity = nullptr;
    QPropertyAnimation *animation = nullptr;

    // Helpers
    Button* createButton(const QString &text, const QString &iconPath);
};