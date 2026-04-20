#pragma once

#include "../../windows/subWindow/SubWindow.h"

class Label;
class Toggle;
class Seperator;
class ComboBox;

namespace Ui::Vault {
    class Preferences : public SubWindow {
        Q_OBJECT

        public:
        Preferences(QWidget *parent = nullptr);

        ComboBox* lockTimeoutComboBox() const;
        ComboBox* appearanceComboBox() const;
        ComboBox* languageComboBox() const;
        Toggle* encryptImportToggle() const;
        Toggle* autoUpdatesToggle() const;
        Button* saveButton() const;

        private:
        void setDarkMode(bool isDarkMode);

        // Save Button
        Button *save_btn = nullptr;

        // Window Title Label
        Label *winTitle = nullptr;

        // Seperator
        Seperator *titlebar_sep = nullptr;

        // Options
        // 1:: Lock Timeout Option
        QHBoxLayout *lock_timeout_layout = nullptr;
        // Icon
        Label *lock_timeout_icon = nullptr;
        // Sublayout
        QVBoxLayout *lock_timeout_sublayout = nullptr;
            // Header
            Label *lock_timeout_header = nullptr;
            // Subtext
            Label *lock_timeout_subText = nullptr;
        // Combo Box
        ComboBox *lock_timeout_combobox = nullptr;

        // ------- Seperator ---------------
        Seperator *sep_1 = nullptr;

        // 2:: Encrypt on Import Option
        QHBoxLayout *encrypt_import_layout = nullptr;
        // Icon
        Label *encrypt_import_icon = nullptr;
        // Sublayout
        QVBoxLayout *encrypt_import_sublayout = nullptr;
            // Header
            Label *encrypt_import_header = nullptr;
            // Subtext
            Label *encrypt_import_subText = nullptr;
        // Toggle
        Toggle *encrypt_import_toggle = nullptr;

        // ------- Seperator ---------------
        Seperator *sep_2 = nullptr;

        // 3:: Appearance
        QHBoxLayout *appearance_layout = nullptr;
        // Icon
        Label *appearance_icon = nullptr;
        // Sublayout
        QVBoxLayout *appearance_sublayout = nullptr;
            // Header
            Label *appearance_header = nullptr;
            // Subtext
            Label *appearance_subText = nullptr;
        // ComboBox
        ComboBox *appearance_combobox = nullptr;

        // ------- Seperator ---------------
        Seperator *sep_3 = nullptr;

        // 4:: Automatic Updates
        QHBoxLayout *auto_updates_layout = nullptr;
        // Icon
        Label *auto_updates_icon = nullptr;
        // Sublayout
        QVBoxLayout *auto_updates_sublayout = nullptr;
            // Header
            Label *auto_updates_header = nullptr;
            // Subtext
            Label *auto_updates_subText = nullptr;
        // Toggle
        Toggle *auto_updates_toggle = nullptr;

        // ------- Seperator ---------------
        Seperator *sep_4 = nullptr;

        // 5:: App Language
        QHBoxLayout *app_lang_layout = nullptr;
        // Icon
        Label *app_lang_icon = nullptr;
        // Sublayout
        QVBoxLayout *app_lang_sublayout = nullptr;
            // Header
            Label *app_lang_header = nullptr;
            // Subtext
            Label *app_lang_subText = nullptr;
        // ComboBox
        ComboBox *app_lang_combobox = nullptr;
    };
};