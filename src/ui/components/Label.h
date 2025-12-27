#pragma once

#include <QLabel>
#include <QFont>
#include <QWidget>
#include <QString>
#include "./SmoothOpacity.h"

class Label : public QLabel {
   Q_OBJECT 

   public:
   explicit Label(bool isIconic = false,
                  const QString &family = "Segoe UI",
                  int pointSize = 8,
                  QFont::Weight weight = QFont::Normal,
                  bool italic = false,
                  const QString &text = " ",
                  Qt::Alignment alignment = Qt::AlignCenter,
                  QWidget *parent = nullptr);
};
