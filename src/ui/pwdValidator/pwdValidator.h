#pragma once
#include <QtWidgets>

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

class PwdValidator : public QWidget {
    Q_OBJECT
public:
    explicit PwdValidator(QWidget *parent = nullptr);

public slots:
    void vald_pwd_reqs(const QString &pw);

signals:
    void pwd_is_strong();
    void pwd_is_weak();

private:
    void load_weak_pwds();
    bool chk_weak_pwd(QByteArray pwd);

    RuleItem *lst_8 = nullptr;
    RuleItem *o_lc = nullptr;
    RuleItem *o_uc = nullptr;
    RuleItem *o_n = nullptr;
    RuleItem *o_sc = nullptr;
    RuleItem *stg_pwd = nullptr;

    QSet<QString> weak_pwds;
};


