#include "OTP.h"

#include "../../components/Button.h"
#include "../../components/Label.h"
#include "../../utils/Utils.h"
#include "../../../../resources/IllustrationManager.h"
#include "../../vault/vault_window/VaultWindow.h"
#include "../auth_window/AuthWindow.h"

#include "../../../core/config/Constants.h"
#include "../../../core/theme/ThemeManager.h"
#include "../../../core/session/SessionManager.h"
#include "../../../core/services/auth/TokenManager.h"

using Ui::OTP;
using Ui::Utils::TextWithBtn;

/* =========================================================================================
                              ACCOUNT OTP IMPLEMENTATION
   ========================================================================================= */
OTP::OTP(QWidget *parent, Ui::AuthWindow *authWindow) : QWidget(parent), _authWindow(authWindow) {
    setAttribute(Qt::WA_TranslucentBackground);

    // Illustration of OTP at the top
    illustration = new Label(true);
    illustration->setFixedSize(QSize(106, 106));
    illustration->setPixmap(QPixmap(IllustrationManager::illustration(Illustrations::OTP)).scaled(106, 106, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    illustration->setScaledContents(true);

    // Main Heading
    heading = new Label("Inter", 22, QFont::Bold, false, "Verify with OTP");
    heading->setStyleSheet("color: black;");

    // Text Under Main Heading
    text = new Label("Segoe UI", 10, QFont::Medium, false);
    text->setWordWrap(true);
    text->setStyleSheet("color: #8D8D8D;");
    text->setFixedWidth(324);
    connect(this, &OTP::emailEntered, this, &OTP::onEmailEntered);

    // OTP Widget (Contains 6 input OTP_BOXS)
    _otpInputWidget = new OTPInputWidget;
    connect(_otpInputWidget, &OTPInputWidget::OTPincomplete, this, [this]{  _verifyBtn->setEnabled(false); });
    connect(_otpInputWidget, &OTPInputWidget::OTPcompleted, this, [this](const QString &otp){
        currentOtp = otp;
        _verifyBtn->setEnabled(true);
    });

    // Message Label (For displaying error messages)
    _message = new AnimatedLabel("Segoe UI", 10, QFont::Medium, false);
    _message->setWordWrap(true);
    _message->setStyleSheet("color: #FF0000;");
    _message->setFixedWidth(324);

    // Resend OTP Widget (Resend Text + Button + Timer)
    _resendOtpWidget = new TextWithBtn("Didn`t receive the OTP?", QSize(136, 22), "Resend", QSize(50, 12), true);
    connect(_resendOtpWidget, &TextWithBtn::buttonClicked, this, &OTP::onResendClicked);

    // Verify Button
    _verifyBtn = new Button("Verify");
    _verifyBtn->setCursor(Qt::PointingHandCursor);
    _verifyBtn->setDisplayMode(Button::TextOnly);
    _verifyBtn->setFixedSize(QSize(360, 36));
    _verifyBtn->setGradientColors("#008EDE", "#15F2FF", "#008EDE");
    _verifyBtn->setFontProperties("Segoe UI", 10, QFont::DemiBold);
    _verifyBtn->setLoaderButton(true);
    _verifyBtn->setEnabled(false);
    connect(_verifyBtn, &Button::clicked, this, &OTP::onVerifyClicked);
   

    // Cancel Button
    _cancelBtn = new Button("Cancel");
    _cancelBtn->setCursor(Qt::PointingHandCursor);
    _cancelBtn->setDisplayMode(Button::TextOnly);
    _cancelBtn->setSecondary(true);
    _cancelBtn->setFixedSize(QSize(360, 36));
    _cancelBtn->setFontProperties("Segoe UI", 10, QFont::DemiBold);
    connect(_cancelBtn, &Button::clicked, this, &OTP::onCancelClicked);

    // Main Layout
    auto *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->addWidget(illustration, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addWidget(heading, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addWidget(text, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addWidget(_otpInputWidget, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(_message, 0, Qt::AlignHCenter);
    layout->addSpacing(16);
    layout->addWidget(_resendOtpWidget, 0, Qt::AlignHCenter);
    layout->addSpacing(26);
    layout->addWidget(_verifyBtn, 0, Qt::AlignHCenter);
    layout->addSpacing(16);
    layout->addWidget(_cancelBtn, 0, Qt::AlignHCenter);
    layout->addStretch();
    setLayout(layout);

    // Timer
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &OTP::onOTPTimeout);

    // OTP Core
    otp_core = new OTPCore(this);

    connect(otp_core, &OTPCore::somethingWentWrong, this, &OTP::onSomethingWentWrong);
    connect(otp_core, &OTPCore::maxLimitReached,    this, &OTP::onMaxLimitReached);
    connect(otp_core, &OTPCore::OTPSent,            this, &OTP::onOTPSent);
    connect(otp_core, &OTPCore::OTPVerified,        this, &OTP::onOTPVerified);
    connect(otp_core, &OTPCore::noInternet,         this, &OTP::onNoInternet);

    // Theme Mode
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &OTP::setDarkMode);
    setDarkMode((tm.isDarkMode()));  
}

/* --------------------  Setters  -----------------  */
void OTP::onEmailEntered(const QString &email) {
    QString emailStr = email;
    int idx = emailStr.indexOf('@');
    if (idx > 3)
        for (int i = 3; i < idx; i++)
            emailStr[i] = '*';

    if (email.isEmpty())
        text->setText("");
    else
        text->setText(QString("To confirm your identity, please enter the One-Time Password (OTP) sent to your registered email-address (%1) below.").arg(emailStr));
}

void OTP::setEmail(const QString &email) {
    _email = email;

    _resendOtpWidget->button()->setEnabled(false);
    isSendingOTP = true;

    otp_core->sendOTP(_email);

    emit emailEntered(_email);
}

void OTP::setAuthType(const QString &authType) {
    _authType = authType;
}

void OTP::setDarkMode(bool isDarkMode) {
    // Header (Verify with OTP)
    heading->setStyleSheet(QString("color: %1;").arg(isDarkMode ? "white" : "black"));

    // Cancel Button
    _cancelBtn->setDarkMode(isDarkMode);

    // Resend OTP widget (text + button + timer)
    _resendOtpWidget->text()->setStyleSheet(QString("color: %1").arg(isDarkMode ? "white" : "black"));
    _resendOtpWidget->timer()->setStyleSheet(QString("color: %1").arg(isDarkMode ? "white" : "black"));

    // OTP input box widgets
    _otpInputWidget->setDarkMode(isDarkMode);
}

/* -------- Slots ----------------- */
void OTP::onOTPSent() {
    isSendingOTP = false;
    disableResendAndSetTimer();
    _message->setAnimatedText("");
}

void OTP::onOTPVerified(bool isVerified, const QJsonObject &obj) {
    if (isVerified) {
        disableControls("Verified");

        if (obj.isEmpty()) {
            emit OTPVerified(isVerified);
        } else {
            // User Account Details
            SessionManager::instance().loadFromJson(obj);

            if (_authWindow) {
                _authWindow->close();
                _authWindow->deleteLater();
            }

            Ui::Vault::VaultWindow::instance()->show();
        }
    } else {
        _verifyBtn->setText(tr("Verify"));
        _verifyBtn->setEnabled(true);
        _message->setAnimatedText(tr("Your entered OTP is incorrect or expired."));
    }
}

void OTP::onResendClicked() {
    if (isSendingOTP)
        return;

    isSendingOTP = true;
    _resendOtpWidget->button()->setEnabled(false);

    if (otp_core) 
        otp_core->sendOTP(_email);
}

void OTP::onVerifyClicked() {
    _verifyBtn->setEnabled(false);
    _verifyBtn->setText("");

    if (otp_core)
        otp_core->verifyOTP(currentOtp, _email, _authType);
}

void OTP::onCancelClicked() {
    QApplication::quit();
}

void OTP::onOTPTimeout() {
    totalSecs--;

    if (totalSecs < 0) {
        timer->stop();
        _resendOtpWidget->button()->setEnabled(true);
        _resendOtpWidget->timer()->hide();
        return;
    }

    int mins = totalSecs / 60;
    int secs = totalSecs % 60;

    timeString = QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));

    _resendOtpWidget->timer()->setText(timeString);
}

void OTP::disableResendAndSetTimer() {
    _resendOtpWidget->button()->setEnabled(false);

    _resendOtpWidget->timer()->setText(OTP_INITIAL_TIME_STR); // Initial time
    _resendOtpWidget->timer()->show();
    
    totalSecs = OTP_TIME;

    if (timer) 
        timer->start(1000); // 1 sec
}

void OTP::disableControls(const QString &buttonText, const QString &errorMessage) {
    _verifyBtn->setText(buttonText);
    _verifyBtn->setEnabled(false);                 // Disables the verify button after successful OTP matching

    _resendOtpWidget->button()->setEnabled(false); // Disables the resend button when otp is verified
    _resendOtpWidget->timer()->hide();             // Hides the timer
    
    _message->setAnimatedText(errorMessage);

    if (timer) 
        timer->stop();

    _otpInputWidget->setEnabled(false);
}

using OTPAction =  Core::OTPService::OTPAction;
void OTP::onSomethingWentWrong(const OTPAction &action) {
    if (action == OTPAction::Send) {
        isSendingOTP = false;

        _message->setAnimatedText(tr("Failed to send OTP to your email address. Try again later"));

        _verifyBtn->setText(tr("Verify"));
        _verifyBtn->setEnabled(false);                 

        _resendOtpWidget->button()->setEnabled(true); 
    } else {
        _message->setAnimatedText(tr("Failed to verify your entered OTP"));   

        _verifyBtn->setText(tr("Verify"));
        _verifyBtn->setEnabled(true);
    }
}

void OTP::onMaxLimitReached() {
    disableControls("Verify");

    if (_message)
        _message->setAnimatedText(tr("Maximum limit reached. Try again after 24hrs."));
}

void OTP::onNoInternet() {
    _verifyBtn->setEnabled(true);
    _verifyBtn->setText(tr("Verify"));
}



/* =========================================================================================
                           CUSTOMIZED WIDGETS FOR ABOVE CLASS
   ========================================================================================= */

/* ---------------- OTP Widget ------------------------  */
OTPInputWidget::OTPInputWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(QSize(360, 60));
    setFocusPolicy(Qt::StrongFocus);

    // Layout
    auto *layout = new QHBoxLayout(this);
    layout->addStretch();
    layout->setSpacing(0);

    // Font
    QFont font;
    font.setFamily("Segoe UI");
    font.setBold(true);
    font.setPointSize(22);

    // Boxes
    for (int i = 0; i < OTP_BOXS; i++) {
        auto *box = new QLabel;
        box->setFont(font);
        box->setAlignment(Qt::AlignCenter);
        box->setStyleSheet(normalStyle);
        box->setFixedSize(QSize(46, 46));
        box->setContentsMargins(0, 0, 0, 3);
        OTPBoxes.append(box);
        layout->addWidget(box);
        if (i < 4)
            layout->addSpacing(28);
    }

    layout->addStretch();

    // Theme Connection
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &OTPInputWidget::setDarkMode);

    updateStyles();
    updateHighlight();
}

void OTPInputWidget::checkOtpCompletion() {
    QString otp_text;
    bool allFilled = true;

    for (auto *box : OTPBoxes) {
        if (box->text().isEmpty()) {
            allFilled = false;
            break;
        }

        otp_text.append(box->text());
    }

    allFilled ? emit OTPcompleted(otp_text) : emit OTPincomplete();
}

/* --------------------  Overrided Event Methods  -----------------  */
void OTPInputWidget::keyPressEvent(QKeyEvent *event) {
    const int n = OTPBoxes.size();

    if ((event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) ||
        (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z))
    {
        QString text = event->text().toUpper();
        if (!text.isEmpty() && currentIndex >= 0 && currentIndex < n)
        {
            OTPBoxes[currentIndex]->setText(text);
            currentIndex++;

            if (currentIndex >= n)
                currentIndex = n - 1;

            updateHighlight();
            checkOtpCompletion();
        }
    }
    else if (event->key() == Qt::Key_Backspace) {

        if (currentIndex >= 0 && currentIndex < n) {

            if (!OTPBoxes[currentIndex]->text().isEmpty()) {
                OTPBoxes[currentIndex]->clear();
            }
            else {
                if (currentIndex > 0)
                    currentIndex--;

                OTPBoxes[currentIndex]->clear();
            }

            updateHighlight();
            checkOtpCompletion();
        }
    }
}

void OTPInputWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    this->setFocus(Qt::OtherFocusReason);
}

/* --------------------  Setters  -----------------  */
void OTPInputWidget::updateStyles() {
    bg_color_normal = _isDarkMode ? "#2D2D2D" : "#FBFBFB";
    bg_color_focused = _isDarkMode ? "#242424" : "#FFFFFF";
    text_color = _isDarkMode ? "white" : "black";
    border_color = _isDarkMode ? "#616161" : "#CCCCCC";

    normalStyle = QString(
                      "background-color: %1; border-radius: 7px; border: 1px solid %2; color: %3;")
                      .arg(bg_color_normal)
                      .arg(border_color)
                      .arg(text_color);

    focusedStyle = QString(
                       "background-color: %1; border: 2px solid #109AC7; border-radius: 7px; color: %2;")
                       .arg(bg_color_focused)
                       .arg(text_color);

    disabledStyle = QString(
                        "background-color: %1; border-radius: 7px; border: 1px solid %2; color: gray;")
                        .arg(bg_color_normal)
                        .arg(border_color);
}

void OTPInputWidget::updateHighlight() {
    for (int i = 0; i < OTPBoxes.size(); i++) {
        if (!isEnabled())
            OTPBoxes[i]->setStyleSheet(disabledStyle);
        else if (i == currentIndex)
            OTPBoxes[i]->setStyleSheet(focusedStyle);
        else
            OTPBoxes[i]->setStyleSheet(normalStyle);
    }
}

void OTPInputWidget::setDarkMode(bool isDarkMode) {
    _isDarkMode = isDarkMode;
    updateStyles();
    updateHighlight();
}

void OTPInputWidget::setEnabled(bool enabled) {
    QWidget::setEnabled(enabled);
    updateStyles();
    updateHighlight();
}