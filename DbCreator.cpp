#include "DbCreator.h"
#include "ui_DbCreator.h"
#include <qtimer.h>

DbCreator::DbCreator(QWidget *parent, QSqlDatabase *resultDb, QString *resultPath, QByteArray *resultKey, Theme theme)
    : QDialog(parent)
    , ui(new Ui::DbCreator)
    , m_resultPath(resultPath)
    , m_theme(theme)
    , m_resultDb(resultDb)
    , m_resultKey(resultKey)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Create Database"));
    ui->label_entropy->setToolTip("Measure of password strength");




    // Connecting buttons and signals of lineEdits
    connect(ui->button_cancel, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->button_save, SIGNAL(clicked(bool)), this, SLOT(button_save_clicked()));
    connect(ui->lineEdit_password, SIGNAL(textChanged(QString)), this, SLOT(lineEdit_password_textChanged(QString)));
    connect(ui->lineEdit_repeat, SIGNAL(textChanged(QString)), this, SLOT(lineEdit_repeat_textChanged(QString)));



    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    m_repeatEnabled = true;

    m_action_hidePassword = ui->lineEdit_password->addAction(IconLoader::getIcon(Icon::EyeClosed, theme), QLineEdit::TrailingPosition);
    connect(m_action_hidePassword, SIGNAL(triggered(bool)), this, SLOT(hidePassword()));

    m_action_openPasswordGenerator = ui->lineEdit_password->addAction(IconLoader::getIcon(Icon::Dice, theme), QLineEdit::TrailingPosition);
    connect(m_action_openPasswordGenerator, SIGNAL(triggered(bool)), this, SLOT(openPasswordGenerator()));


    // Setting echo mode to password lines
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_repeat->setEchoMode(QLineEdit::Password);

}

DbCreator::~DbCreator() {
    delete ui;
}

void DbCreator::showMsgBox(const QString &title, const QString &text, const QMessageBox::Icon &icon) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIcon(icon);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}



void DbCreator::hidePassword() {
    if (ui->lineEdit_password->echoMode() == QLineEdit::EchoMode::Password) {
        m_action_hidePassword->setIcon(IconLoader::getIcon(Icon::Eye, m_theme));
        ui->lineEdit_password->setEchoMode(QLineEdit::Normal);
        ui->lineEdit_repeat->clear();
        ui->lineEdit_repeat->hide();
        ui->label_repeat->hide();
        m_repeatEnabled = false;
    } else {
        m_action_hidePassword->setIcon(IconLoader::getIcon(Icon::EyeClosed, m_theme));
        ui->lineEdit_repeat->show();
        ui->label_repeat->show();
        m_repeatEnabled = true;
        ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    }
}


void DbCreator::openPasswordGenerator() {
    QString generatedPassword;
    m_window_PasswordGenerator = new PasswordGenerator(this, &generatedPassword);
    m_window_PasswordGenerator->exec();
    delete m_window_PasswordGenerator;

    if (generatedPassword.size() > 0) {
        ui->lineEdit_repeat->setText(generatedPassword);
        ui->lineEdit_password->setText(generatedPassword);
    }
}

void DbCreator::button_save_clicked() {
    if (ui->lineEdit_password->text() != "") {
        if (m_repeatEnabled && ui->lineEdit_password->text() != ui->lineEdit_repeat->text()) {
            QMessageBox msg;
            msg.setText(tr("Password must matching with repeat line."));
            msg.setIcon(QMessageBox::Warning);
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
        } else {
            QByteArray key = ui->lineEdit_password->text().toUtf8();
            if (!DbManager::createAndFillDatabase(*m_resultPath, key, m_resultDb)) {
                showMsgBox(tr("Error"), tr("Unable to create new database.\nTry again please."), QMessageBox::Critical);
                return;
            }
            QVector<QString> tables;
            if (!DbManager::loadDb(*m_resultPath, &key, m_resultDb, &tables)) {
                showMsgBox(tr("Error"), tr("Password is incorrect or file is damaged.\nTry again please."), QMessageBox::Critical);
            }
        }
    } else {
        QMessageBox msg;
        msg.setText(tr("Password line must be not empty"));
        msg.setIcon(QMessageBox::Warning);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
}


void DbCreator::lineEdit_repeat_textChanged(const QString &arg1) {
    // If password is hidden
    if (m_repeatEnabled) {
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
    int entropy = PasswordGenerator::getEntropy(arg1);
    ui->label_entropy->setText("Entropy: " + QString::number(entropy));

    switch(PasswordGenerator::getStrength(entropy)) {
    case Password::Weak:
        ui->label_quality->setText(tr("Quality: Weak"));
        break;
    case Password::Normal:
        ui->label_quality->setText(tr("Quality: Normal"));
        break;
    case Password::Strong:
        ui->label_quality->setText(tr("Quality: Strong"));
        break;
    case Password::VeryStrong:
        ui->label_quality->setText(tr("Quality: Very Strong"));
        break;
    }
}

