#pragma once

#include "../../components/LineProgress.h"
#include "../../components/SpinnerProgress.h"
#include "../../components/Button.h"
#include "../../components/Seperator.h"
#include "../../components/Label.h"

#include "../../windows/subWindow/SubWindow.h"

#include <QVBoxLayout>
#include <QDateTime>
#include <QHBoxLayout>

struct Update {
  QString _currentVersion = "0.0.0";
  QString _newVersion = "0.0.0";
  QString _size = "0 MB";
  QDateTime _releasedDate;
  QString _updateNotes;

  Update(const QString &currentVersion, 
         const QString &newVersion, 
         const QString &size, 
         const QDateTime releasedDate,
         const QString &updateNotes);
};

class UpdateInfo : public QWidget {
  Q_OBJECT

  public:
  UpdateInfo(QWidget *parent = nullptr);
  void setDarkMode(bool enable);
  void setUpdate(const Update &update);
  void setUpdateAvailable(bool isAvailable);

  signals:
  void isUpdateAvailable(bool isAvailable);

  private:
  // Theme Mode
  bool isDarkMode = false;

  // Update Version, Size , and Released Date
  Label *update_ver_date_label = nullptr;

  // What's new label
  Label *whats_new_label = nullptr;

  // Update Notes
  Label *update_notes_label = nullptr;

  // Seperator
  Seperator *sep = nullptr;
};

class AppUpdates : public SubWindow {
  Q_OBJECT

  public:
  AppUpdates(QWidget *parent = nullptr);
  void setDarkMode(bool enable);

  UpdateInfo* updateInfoWidget() const;
  
  protected:
  void showEvent(QShowEvent *event);

  private:
  // Theme Mode Flag
  bool isDarkMode = false;

  // Window Title
  Label *winTitle = nullptr;

  // Layout
  QVBoxLayout *win_content_area_layout = nullptr;

  // Checking for update
  // Spinner Wrapper
  QWidget *spinner_wrapper = nullptr;
  // Spinner
  SpinnerProgress *spinner = nullptr;
  // Spinner Label
  Label *spinner_label = nullptr;

  // Update details
  UpdateInfo *update_details_widget = nullptr;

  // Seperator
  Seperator *sep = nullptr;
};