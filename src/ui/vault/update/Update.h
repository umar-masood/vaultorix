#pragma once

#include "../../windows/subWindow/SubWindow.h"
#include "../../components/LineProgress.h"

#include <QVBoxLayout>
#include <QDateTime>
#include <QHBoxLayout>
#include <QStackedWidget>

class SpinnerProgress;
class Button;
class Seperator;
class Label;

namespace Ui::Vault {
    struct Update {
        QString _currentVersion = "0.0.0";
        QString _newVersion = "0.0.0";
        QString _size = "0 MB";
        QDateTime _releasedDate;
        QString _updateNotes;

        Update(const QString &currentVersion,
               const QString &newVersion,
               const QString &size,
               const QDateTime &releasedDate,
               const QString &updateNotes);
    };

    class AppUpdate : public SubWindow {
        Q_OBJECT

        public:
        enum class UpdateState { 
            Available, 
            NotAvailable, 
            NoInternet, 
            SomethingWentWrong
        };

        AppUpdate(QWidget *parent = nullptr);
        void setUpdateDetails(const Update &update);
        void setUpdateState(const UpdateState &state);
        LineProgress *downloadProgressBar() const;
        Button *updateButton() const;

        protected:
        void showEvent(QShowEvent *event);

        private:
        void setDarkMode(bool isDarkMode);
        QWidget* updateInfoWidget();

        // Window Title
        Label *winTitle = nullptr;

        // Main Layout
        QVBoxLayout *win_content_area_layout = nullptr;

        // Stacked Widget (State Manager)
        QStackedWidget *stacked_widget = nullptr;

        // Update Info Ui Widget
        Label *update_ver_date_label = nullptr;
        Label *whats_new_label = nullptr;
        Label *update_notes_label = nullptr;
        Seperator *update_sep = nullptr;

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
                          No Update/ No Internet Page
           =============================================================== */
        Label *no_update_label = nullptr;

        /* ===============================================================
                              Update Available Page
           =============================================================== */
        Seperator *main_sep = nullptr;
        Button *update_btn = nullptr;

        /* ===============================================================
                              Downloading Page
           ============================================================== */
        Label *downloading_label = nullptr;
        LineProgress *download_progressbar = nullptr;
        Label *closing_app_label = nullptr;
    };
};