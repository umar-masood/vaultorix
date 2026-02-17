#pragma once
#include <QWidget>
#include <QPixmap>
#include <QString>
#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QFont>

class RuleItem : public QWidget {
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
    const int iconSize = 20;

    // Different Icons for different states
    QPixmap validIcon, invalidIcon;

    // Text Color Codes
    QString uncoloredText = "#8D8D8D";
    QString coloredText= "#009138";

    QString text;
    QFont font;
};

class PwdRulesWidget : public QWidget {
    Q_OBJECT

    public:
    explicit PwdRulesWidget(QWidget *parent = nullptr);

    // Getters of Password Validation Rules Widgets
    RuleItem* atLeastEight() const;
    RuleItem* atLeastOneLowerCaseChar() const;
    RuleItem* atLeastOneUpperCaseChar() const;
    RuleItem* atLeastOneDigit() const;
    RuleItem* atLeastOneSpecialChar() const;
    RuleItem* strongPassword() const;

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


