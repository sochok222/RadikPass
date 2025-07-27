#include "DbCreator.h"
#include "ui_DbCreator.h"
#include <qtimer.h>

DbCreator::DbCreator(QWidget *parent, QByteArray *key, const QString path, Theme theme)
    : QDialog(parent)
    , ui(new Ui::DbCreator)
    , key(key)
    , theme(theme)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("Create Database")); // Setting window title

    if (path != "") // If path is not empty
    {
        ui->label_path->setText("Path: " + path); // Writing path to window
    } else { // Else showing error to user
        QMessageBox msg;
        msg.setText(tr("Unable to load path.\nTry again please."));
        msg.setIcon(QMessageBox::Critical);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        QTimer::singleShot(0, this, SLOT(close()));
    }

    ui->line_password->setEchoMode(QLineEdit::Password);
    repeatEnabled = true;

    action_hidePassword = ui->line_password->addAction(IconLoader::getIcon(Icon::EyeClosed, theme), QLineEdit::TrailingPosition);
    connect(action_hidePassword, SIGNAL(triggered(bool)), this, SLOT(hidePassword()));

    action_openPasswordGenerator = ui->line_password->addAction(IconLoader::getIcon(Icon::Dice, theme), QLineEdit::TrailingPosition);
    connect(action_openPasswordGenerator, SIGNAL(triggered(bool)), this, SLOT(openPasswordGenerator()));


    // Setting echo mode to password lines
    ui->line_password->setEchoMode(QLineEdit::Password);
    ui->line_repeat->setEchoMode(QLineEdit::Password);

}

DbCreator::~DbCreator() {
    delete ui;
}


void DbCreator::hidePassword() {
    if (ui->line_password->echoMode() == QLineEdit::EchoMode::Password) {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::Eye, theme));
        ui->line_password->setEchoMode(QLineEdit::Normal);
        ui->line_repeat->clear();
        ui->line_repeat->hide();
        ui->label_repeat->hide();
        repeatEnabled = false;
    } else {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::EyeClosed, theme));
        ui->line_repeat->show();
        ui->label_repeat->show();
        repeatEnabled = true;
        ui->line_password->setEchoMode(QLineEdit::Password);
    }
}


void DbCreator::openPasswordGenerator() {
    QString generatedPassword;
    PasswordGenerator *window_PasswordGenerator = new PasswordGenerator(this, &generatedPassword);
    window_PasswordGenerator->exec();
    delete window_PasswordGenerator;

    if (generatedPassword.size() > 0) {
        ui->line_repeat->setText(generatedPassword);
        ui->line_password->setText(generatedPassword);
    }
}


void DbCreator::on_saveButton_clicked() {
    *key = ui->line_password->text().toUtf8(); // Returning key that user entered
    this->close(); // Closing window
}

void DbCreator::on_linePassword_textChanged(const QString &arg1) {
    bool hasDigit = false;
    bool hasUppercase = false;
    bool hasLowercase = false;
    bool hasSpecial = false;


    switch(ui->line_password->text().size()) {
    case 1:
        ui->label_passwordQuality->setText(tr("Weak"));
        break;
    case 8:
        ui->label_passwordQuality->setText(tr("Normal"));
        break;
    case 16:
        ui->label_passwordQuality->setText(tr("Strong"));
        break;
    case 25:
        ui->label_passwordQuality->setText(tr("Very Strong"));
        break;
    }
}

void DbCreator::on_lineRepeat_textChanged(const QString &arg1) {
    // If password is hidden
    if (repeatEnabled) {
        if (arg1!=ui->line_password->text() && ui->line_repeat->text().size() > 0) { // If password in repeat line is not the same as in password line
            // Setting red palette to repeat line
            QPalette lineRepeatPalette;
            lineRepeatPalette.setColor(QPalette::Base, QColor(235,135,135));
            ui->line_repeat->setPalette(lineRepeatPalette);
        } else {
            // Else set default palette
            QPalette lineRepeatPalette;
            ui->line_repeat->setPalette(lineRepeatPalette);
        }
    }
}


void DbCreator::on_cancelButton_clicked() {
    this->reject(); // Closing window
}

