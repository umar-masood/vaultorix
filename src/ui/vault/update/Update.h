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
#include <QStackedWidget>

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

  // Main Layout
  QVBoxLayout *win_content_area_layout = nullptr;

  // Stacked Widget (State Manager)
  QStackedWidget *stacked_widget = nullptr;

  /* ===============================================================
                              Pages
     =============================================================== */

  QWidget *checking_page = nullptr;
  QWidget *no_update_page = nullptr;
  QWidget *update_page = nullptr;

  QStackedWidget *action_stack = nullptr;
  QWidget *action_button_page = nullptr;
  QWidget *action_progress_page = nullptr;
  /* ===============================================================
                              Checking Page
     =============================================================== */

  QWidget *spinner_wrapper = nullptr;
  SpinnerProgress *spinner = nullptr;
  Label *spinner_label = nullptr;

  /* ===============================================================
                              No Update Page
     =============================================================== */

  Label *no_update_label = nullptr;

  /* ===============================================================
                              Update Available Page
     =============================================================== */

  UpdateInfo *update_details_widget = nullptr;
  Seperator *sep = nullptr;
  Button *update_btn = nullptr;

  /* ===============================================================
                              Downloading Page
     =============================================================== */

  Label *downloading_label = nullptr;
  LineProgress *update_progressbar = nullptr;
  Label *closing_app_label = nullptr;
  /* ===============================================================
                              Timer
     =============================================================== */

  QTimer *timer = nullptr;
  int v = 0;
};