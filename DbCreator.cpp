#include "DbCreator.h"
#include "ui_DbCreator.h"

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
        ui->labelPath->setText("Path: " + path); // Writing path to window
    } else { // Else showing error to user
        QMessageBox msg;
        msg.setText(tr("Unable to load path.\nTry again please."));
        msg.setIcon(QMessageBox::Critical);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        this->close();
    }

    ui->linePassword->setEchoMode(QLineEdit::Password);
    repeatEnabled = true;

    action_hidePassword = ui->linePassword->addAction(IconLoader::getIcon(Icon::EyeClosed, theme), QLineEdit::TrailingPosition);
    connect(action_hidePassword, SIGNAL(triggered(bool)), this, SLOT(hidePassword()));

    action_openPasswordGenerator = ui->linePassword->addAction(IconLoader::getIcon(Icon::Dice, theme), QLineEdit::TrailingPosition);
    connect(action_openPasswordGenerator, SIGNAL(triggered(bool)), this, SLOT(openPasswordGenerator()));


    // Setting maximum length to password lines
    ui->linePassword->setMaxLength(50);
    ui->lineRepeat->setMaxLength(50);

    // Setting echo mode to password lines
    ui->linePassword->setEchoMode(QLineEdit::Password);
    ui->lineRepeat->setEchoMode(QLineEdit::Password);

    // Creating validator for password
    QRegularExpression rx(R"(^[a-zA-Zа-яА-Я0-9!@#$&().+,/-]*$)");
    validator = new QRegularExpressionValidator(rx, this);
    // Setting validator for password lines
    ui->linePassword->setValidator(validator);
    ui->lineRepeat->setValidator(validator);
}

DbCreator::~DbCreator() {
    delete ui;
    delete validator;
}


void DbCreator::hidePassword() {
    if (ui->linePassword->echoMode() == QLineEdit::EchoMode::Password) {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::Eye, theme));
        ui->linePassword->setEchoMode(QLineEdit::Normal);
        ui->lineRepeat->clear();
        ui->lineRepeat->hide();
        ui->labelRepeat->hide();
        repeatEnabled = false;
    } else {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::EyeClosed, theme));
        ui->lineRepeat->show();
        ui->labelRepeat->show();
        repeatEnabled = true;
        ui->linePassword->setEchoMode(QLineEdit::Password);
    }
}


void DbCreator::openPasswordGenerator() {
    QString generatedPassword;
    PasswordGenerator *window_PasswordGenerator = new PasswordGenerator(this, &generatedPassword);
    window_PasswordGenerator->exec();
    delete window_PasswordGenerator;

    if (generatedPassword.size() > 0) {
        ui->lineRepeat->setText(generatedPassword);
        ui->linePassword->setText(generatedPassword);
    }
}


void DbCreator::on_saveButton_clicked() {
    *key = ui->linePassword->text().toUtf8(); // Returning key that user entered
    this->close(); // Closing window
}

void DbCreator::on_linePassword_textChanged(const QString &arg1) {
    // Checking password quality depending on length
    switch(ui->linePassword->text().size()) {
    case 1:
        ui->labelPasswordQuality->setText(tr("Weak"));
        break;
    case 8:
        ui->labelPasswordQuality->setText(tr("Normal"));
        break;
    case 16:
        ui->labelPasswordQuality->setText(tr("Strong"));
        break;
    case 25:
        ui->labelPasswordQuality->setText(tr("Very Strong"));
        break;
    }
}

void DbCreator::on_lineRepeat_textChanged(const QString &arg1) {
    // If password is hidden
    if (repeatEnabled) {
        if (arg1!=ui->linePassword->text() && ui->lineRepeat->text().size() > 0) { // If password in repeat line is not the same as in password line
            // Setting red palette to repeat line
            QPalette lineRepeatPalette;
            lineRepeatPalette.setColor(QPalette::Base, QColor(235,135,135));
            ui->lineRepeat->setPalette(lineRepeatPalette);
        } else {
            // Else set default palette
            QPalette lineRepeatPalette;
            ui->lineRepeat->setPalette(lineRepeatPalette);
        }
    }
}


void DbCreator::on_cancelButton_clicked() {
    this->reject(); // Closing window
}

