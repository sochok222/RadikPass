#include "DbOpener.h"

#include "MainWindow.h"
#include "ui_DbOpener.h"

DbOpener::DbOpener(QWidget *parent, AppState *appState)
    : QDialog(parent)
    , ui(new Ui::DbOpener)
    , theme(theme)
    , appState(appState)
{
    ui->setupUi(this);

    if (appState == nullptr) {
        qCritical() << "appState is nullptr";
        QTimer::singleShot(0, this, SLOT(close()));
    }

    ui->button_ok->setDefault(true);

    // Connecting buttons to slots
    connect(ui->button_ok, SIGNAL(clicked(bool)), this, SLOT(button_ok_clicked()));
    connect(ui->button_cancel, SIGNAL(clicked(bool)), this, SLOT(close()));

    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);

    action_hidePassword = ui->lineEdit_password->addAction(IconLoader::getIcon(Icon::EyeClosed, theme), QLineEdit::TrailingPosition);
    connect(action_hidePassword, SIGNAL(triggered(bool)), this, SLOT(hidePassword()));

    QSettings setting("AlexRadik", "RadikPass");
    ui->button_ok->setAutoDefault(true);
    this->setWindowTitle(tr("Open Database"));
}

DbOpener::~DbOpener() {
    delete ui;
}

void DbOpener::hidePassword() {
    if (ui->lineEdit_password->echoMode() == QLineEdit::EchoMode::Password) {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::Eye, theme));
        ui->lineEdit_password->setEchoMode(QLineEdit::Normal);
    } else {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::EyeClosed, theme));
        ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    }
}

void DbOpener::showMsgBox(const QString &title, const QString &text, const QMessageBox::Icon &icon) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIcon(icon);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}


void DbOpener::button_ok_clicked() {
    if (ui->lineEdit_password->text().size() > 0) {
        if (!DbManager::loadDb(appState->pathToDatabase, appState->masterKey, &appState->db, &appState->tables)) { // Trying to load database
            showMsgBox(tr("Error"), tr("Password is incorrect or database file is damaged\nTry again, please"), QMessageBox::Critical);
        }
        this->close();
    } else {
        QMessageBox msg;
        msg.setText(tr("Password line must be not empty"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
}



