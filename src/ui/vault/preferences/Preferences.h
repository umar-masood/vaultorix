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

        ComboBox* languageComboBox() const;
        Toggle* encryptImportToggle() const;
        Toggle* autoUpdatesToggle() const;
        Button* saveButton() const;

        private:
        void setDarkMode(bool isDarkMode);

        // Window Title Label
        Label *winTitle = nullptr;

        // ------- Seperator ---------------
        Seperator *sep_1 = nullptr;

        // 2:: Encrypt on Import Option
        // Icon
        Label *encrypt_import_icon = nullptr;
            // Header
            Label *encrypt_import_header = nullptr;
            // Subtext
            Label *encrypt_import_subText = nullptr;
        // Toggle
        Toggle *encrypt_import_toggle = nullptr;

        // ------- Seperator ---------------
        Seperator *sep_2 = nullptr;

        // 3:: Automatic Updates
        // Icon
        Label *auto_updates_icon = nullptr;
            // Header
            Label *auto_updates_header = nullptr;
            // Subtext
            Label *auto_updates_subText = nullptr;
        // Toggle
        Toggle *auto_updates_toggle = nullptr;

        // ------- Seperator ---------------
        Seperator *sep_3 = nullptr;

        // 4:: App Language
        // Icon
        Label *app_lang_icon = nullptr;
            // Header
            Label *app_lang_header = nullptr;
            // Subtext
            Label *app_lang_subText = nullptr;
        // ComboBox
        ComboBox *app_lang_combobox = nullptr;
    };
};