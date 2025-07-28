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


    // Connecting buttons and signals of lineEdits
    connect(ui->button_cancel, SIGNAL(clicked(bool)), this, SLOT(clicked_button_cancel()));
    connect(ui->button_save, SIGNAL(clicked(bool)), this, SLOT(clicked_button_save()));
    connect(ui->lineEdit_password, SIGNAL(textChanged(QString)), this, SLOT(lineEdit_password_textChanged(QString)));
    connect(ui->lineEdit_repeat, SIGNAL(textChanged(QString)), this, SLOT(lineEdit_repeat_textChanged(QString)));


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

    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    repeatEnabled = true;

    action_hidePassword = ui->lineEdit_password->addAction(IconLoader::getIcon(Icon::EyeClosed, theme), QLineEdit::TrailingPosition);
    connect(action_hidePassword, SIGNAL(triggered(bool)), this, SLOT(hidePassword()));

    action_openPasswordGenerator = ui->lineEdit_password->addAction(IconLoader::getIcon(Icon::Dice, theme), QLineEdit::TrailingPosition);
    connect(action_openPasswordGenerator, SIGNAL(triggered(bool)), this, SLOT(openPasswordGenerator()));


    // Setting echo mode to password lines
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_repeat->setEchoMode(QLineEdit::Password);

}

DbCreator::~DbCreator() {
    delete ui;
}


void DbCreator::hidePassword() {
    if (ui->lineEdit_password->echoMode() == QLineEdit::EchoMode::Password) {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::Eye, theme));
        ui->lineEdit_password->setEchoMode(QLineEdit::Normal);
        ui->lineEdit_repeat->clear();
        ui->lineEdit_repeat->hide();
        ui->label_repeat->hide();
        repeatEnabled = false;
    } else {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::EyeClosed, theme));
        ui->lineEdit_repeat->show();
        ui->label_repeat->show();
        repeatEnabled = true;
        ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    }
}


void DbCreator::openPasswordGenerator() {
    QString generatedPassword;
    PasswordGenerator *window_PasswordGenerator = new PasswordGenerator(this, &generatedPassword);
    window_PasswordGenerator->exec();
    delete window_PasswordGenerator;

    if (generatedPassword.size() > 0) {
        ui->lineEdit_repeat->setText(generatedPassword);
        ui->lineEdit_password->setText(generatedPassword);
    }
}

void DbCreator::clicked_button_save() {
    *key = ui->lineEdit_password->text().toUtf8(); // Returning key that user entered
    this->close();
}


void DbCreator::clicked_button_cancel() {
    this->reject();
}


void DbCreator::lineEdit_repeat_textChanged(const QString &arg1) {
    // If password is hidden
    if (repeatEnabled) {
        if (arg1!=ui->lineEdit_password->text() && ui->lineEdit_repeat->text().size() > 0) { // If password in repeat line is not the same as in password line
            // Setting red palette to repeat line
            QPalette lineRepeatPalette;
            lineRepeatPalette.setColor(QPalette::Base, QColor(235,135,135));
            ui->lineEdit_repeat->setPalette(lineRepeatPalette);
        } else {
            // Else set default palette
            QPalette lineRepeatPalette;
            ui->lineEdit_repeat->setPalette(lineRepeatPalette);
        }
    }
}


void DbCreator::lineEdit_password_textChanged(const QString &arg1) {
    bool hasDigit = false;
    bool hasUppercase = false;
    bool hasLowercase = false;
    bool hasSpecial = false;


    switch(ui->lineEdit_password->text().size()) {
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

