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

    void setChecked();
    void setUnchecked();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool checkedState = false;

    QPixmap iconChecked;
    QPixmap iconUnchecked;

    const int iconSize = 20;

    QString uncoloredText = "#8D8D8D";
    QString coloredText= "#009138";

    QString text;
    QFont font;
};

class PwdRulesWidget : public QWidget {
    Q_OBJECT
public:
    explicit PwdRulesWidget(QWidget *parent = nullptr);

    RuleItem* atLeastEight() const;
    RuleItem* oneLowerCase() const;
    RuleItem* oneUpperCase() const;
    RuleItem* oneDigit() const;
    RuleItem* oneSpecialChar() const;
    RuleItem* strongPwd() const;

private:
    RuleItem *lst_8 = nullptr;
    RuleItem *o_lc = nullptr;
    RuleItem *o_uc = nullptr;
    RuleItem *o_n = nullptr;
    RuleItem *o_sc = nullptr;
    RuleItem *stg_pwd = nullptr;

    QSet<QString> weak_pwds;
};


