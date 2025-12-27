#include "Label.h"

/* Custom Text Label (With and Without Icon)*/
Label::Label(bool isIconic, 
            const QString &family, 
            int pointSize, 
            QFont::Weight weight, 
            bool italic, 
            const QString &text, 
            Qt::Alignment alignment, 
            QWidget *parent) : QLabel(parent) {

   setAttribute(Qt::WA_TranslucentBackground);
   setAlignment(alignment);

   // If not iconic, set the font and text
   if (!isIconic) {
      setText(text);

      QFont fnt;
      fnt.setPointSize(pointSize);
      fnt.setFamily(family);
      fnt.setWeight(weight);
      fnt.setItalic(italic);
      setFont(fnt);
   }
}
