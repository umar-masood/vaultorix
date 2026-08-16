#pragma once

#include "../../windows/subwindow/Subwindow.h"
#include "../../../core/services/update/UpdateService.h"

#include <QDateTime>
#include <optional>

class SpinnerProgress;
class LineProgress;
class Button;
class Seperator;
class Label;
class QStackedWidget;
class QVBoxLayout;
class QHBoxLayout;

namespace Ui::Vault {
    class Update : public SubWindow {
        Q_OBJECT

        public:
        Update(QWidget *parent = nullptr);
        LineProgress *downloadProgressBar() const;
        Button *updateButton() const;

        protected:
        void showEvent(QShowEvent *event);

        private slots:
        void onUpdateCurrentState(const Core::UpdateService::State &state);
        void onUpdateError(const Core::UpdateService::Error &error);
        void onUpdateAvailable(std::optional<Core::UpdateService::UpdateData> data);
        void onUpdateDownloaded();
        void onUpdateDownloadProgress(int percent);
        void onUpdateDownloadStarted();
        void onUpdateNowButtonClicked();

        private:
        void setDarkMode(bool isDarkMode);
        QWidget* updateInfoWidget();
        void setUpdateDetails(const Core::UpdateService::UpdateData &data);

        // Update Core
        Core::UpdateService *update_core = nullptr;

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