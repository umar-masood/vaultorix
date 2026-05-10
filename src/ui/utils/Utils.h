#pragma once

#include <QPixmap>
#include <QWidget>
#include <QString>
#include <QFont>
#include <QLabel>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QSize>

#include "../components/TextField.h"
#include "../components/Button.h"
#include "../components/ToolTip.h"
#include "../components/CheckBox.h"
#include "../components/Label.h"

namespace Ui::Utils {
    QPixmap cropToCircle(const QPixmap &pixmap, int size);

    /* ---------------- Password Validation Rules Items --------------------- */
    class RuleItem : public QWidget
    {
        Q_OBJECT

        public:
        RuleItem(const QString &ruleTxt, QWidget *parent = nullptr);
        void setValid();
        void setInvalid();

        protected:
        void paintEvent(QPaintEvent *event) override;

        private:
        // Flag to check validity of each rule
        bool validityState = false;

        // Icon Size
        QSize iconSize = QSize(20, 20);

        // Different Icons for different states
        QPixmap validIcon, invalidIcon;

        // Text Color Codes
        QString uncoloredText = "#8D8D8D";
        QString coloredText = "#009138";

        QString text;
        QFont font;
    };

    class PasswordRules : public QWidget
    {
        Q_OBJECT

        public:
        explicit PasswordRules(QWidget *parent = nullptr);

        // Getters of Password Validation Rules Widgets
        RuleItem *atLeastEight() const;
        RuleItem *atLeastOneLowerCaseChar() const;
        RuleItem *atLeastOneUpperCaseChar() const;
        RuleItem *atLeastOneDigit() const;
        RuleItem *atLeastOneSpecialChar() const;
        RuleItem *strongPassword() const;

        private:
        // Main Layout
        QVBoxLayout *mainLayout = nullptr;

        // Password Validation Rules Widgets
        RuleItem *_atLeastEightChars = nullptr;
        RuleItem *_atLeastOneLowerChar = nullptr;
        RuleItem *_atLeastOneUpperChar = nullptr;
        RuleItem *_atLeastOneDigit = nullptr;
        RuleItem *_atLeastOneSpecialChar = nullptr;
        RuleItem *_strongPassword = nullptr;
    };

    /* -------------- Customized TextField --------------- */
    class CustomTextField : public TextField
    {
        Q_OBJECT

        public:
        explicit CustomTextField(bool hasValidity = false, QWidget *parent = nullptr);
        void setValid();
        void setInvalid();
        void setDarkMode(bool value) override;
        void setTooltip(const QString &tooltipText);
        QLabel *validityIcon() const;

        private:
        // Validity Icon
        QLabel *_validityIcon = nullptr;
        // ToolTip
        ToolTip *tooltip = nullptr;

        // Flag
        bool hasTip = false;
    };

    /* ------------------ CheckBox with Button -------------- */
    class CheckWithBtn : public QWidget
    {
        Q_OBJECT

        public:
        explicit CheckWithBtn(QWidget *parent = nullptr);
        void setDarkMode(bool value);
        Button *button() const;
        CheckBox *checkBox() const;

        private:
        // Theme Mode Flag
        bool isDarkMode = false;

        // CheckBox
        CheckBox *_checkbox = nullptr;

        // HyperLink Button
        Button *_button = nullptr;

        signals:
        void onButtonClicked();
        void boxChecked(bool checked);
    };

    /* ----------------- Text with Button ------------------ */
    class TextWithBtn : public QWidget {
       Q_OBJECT

       public:
       explicit TextWithBtn(const QString &promptText = QString(),
                            const QSize &promptTextSize = QSize(), 
                            const QString &hyperlinkText =  QString(), 
                            const QSize &hyperlinkSize = QSize(), 
                            bool hasTimer = false,
                            QWidget *parent = nullptr);

       Label* text() const;
       Button* button() const;
       Label* timer() const;

       private:
       // Text Label
       Label *_text = nullptr;

       // Timer Label
       Label *_timer = nullptr;
    
       // Resend Button
       Button *_button = nullptr;

       signals:
       void buttonClicked();
    };
};
