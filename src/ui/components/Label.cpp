#include "Label.h"

/* --------------- Custom Text Label (With and Without Icon) ------------------ */
Label::Label(bool isIconic, 
            Qt::Alignment alignment, 
            QWidget *parent) : QLabel(parent) 
{
   setAttribute(Qt::WA_TranslucentBackground);
   setAlignment(alignment);
}

Label::Label(const QString &family, 
            int pointSize, 
            QFont::Weight weight, 
            bool italic, 
            const QString &text, 
            Qt::Alignment alignment, 
            QWidget *parent) : QLabel(parent) 
{
   setAttribute(Qt::WA_TranslucentBackground);
   setAlignment(alignment);
   setText(text);

   QFont fnt;
   fnt.setPointSize(pointSize);
   fnt.setFamily(family);
   fnt.setWeight(weight);
   fnt.setItalic(italic);
   setFont(fnt);
}

void Label::setTextColor(const QColor &color) {
   setStyleSheet(QString("color : %1;").arg(color.name()));
}

AnimatedLabel::AnimatedLabel(bool isIconic, 
                           const QString &family, 
                           int pointSize, 
                           QFont::Weight weight, 
                           bool italic, 
                           const QString &text, 
                           Qt::Alignment alignment, 
                           QWidget *parent) : QLabel(parent) 
{
   setAttribute(Qt::WA_TranslucentBackground);
   setAlignment(alignment);
   init();
}

AnimatedLabel::AnimatedLabel(const QString &family, 
                           int pointSize, 
                           QFont::Weight weight, 
                           bool italic, 
                           const QString &text, 
                           Qt::Alignment alignment, 
                           QWidget *parent) : QLabel(parent) 
{
   setAttribute(Qt::WA_TranslucentBackground);
   setAlignment(alignment);

   QFont fnt;
   fnt.setPointSize(pointSize);
   fnt.setFamily(family);
   fnt.setWeight(weight);
   fnt.setItalic(italic);
   setFont(fnt); 
   setText(text);

   init();
}

void AnimatedLabel::init() {
   // Opacity effect
   effect = new QGraphicsOpacityEffect;
   effect->setOpacity(0.0);
   setGraphicsEffect(effect);

   // Fade In Animation
   fadeIn = new QPropertyAnimation(effect, "opacity", this);
   fadeIn->setStartValue(0.0);
   fadeIn->setEndValue(1.0);
   fadeIn->setDuration(500);
   fadeIn->setEasingCurve(QEasingCurve::InOutQuad);

   // Fade Out Animation
   fadeOut = new QPropertyAnimation(effect, "opacity", this);
   fadeOut->setStartValue(1.0);
   fadeOut->setEndValue(0.0);
   fadeOut->setDuration(500);
   fadeOut->setEasingCurve(QEasingCurve::InOutQuad);

   connect(fadeOut, &QPropertyAnimation::finished, this, [this]() {
      fadeOut->stop();        
      effect->setOpacity(1.0); 
      QLabel::hide();    
     // setGraphicsEffect(nullptr);
   });
}

void AnimatedLabel::show() {
   if (fadeOut->state() == QAbstractAnimation::Running)
      fadeOut->stop();

   effect->setOpacity(0.0);

   QLabel::show();

   fadeIn->start();
}

void AnimatedLabel::hide() {
   if (fadeIn->state() == QAbstractAnimation::Running)
      fadeIn->stop();

   if (fadeOut)
      fadeOut->start();
}

void AnimatedLabel::setAnimatedText(const QString &text) {
   hide();
   QTimer::singleShot(500, this, [this, text]() {
      QLabel::setText(text);
      show(); 
   });
}
