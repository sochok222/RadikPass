#include "DbOpener.h"
#include "ui_DbOpener.h"

DbOpener::DbOpener(QWidget *parent, QSqlDatabase *db, QString pathToDatabase, QByteArray *masterKey, QVector<QString> *tables, Theme colorTheme)
    : QDialog(parent)
    , ui(new Ui::DbOpener)
    , db(db)
    , pathToDatabase(pathToDatabase)
    , masterKey(masterKey)
    , colorTheme(colorTheme)
    , tables(tables)
{
    ui->setupUi(this);
    ui->button_ok->setDefault(true);

    if (db == nullptr) {
        qCritical() << "db is nullptr";
        QTimer::singleShot(0, this, SLOT(close()));
    }
    if (masterKey == nullptr) {
        qCritical() << "resultKey is nullptr";
        QTimer::singleShot(0, this, SLOT(close()));
    }
    if (tables == nullptr) {
        qCritical() << "tables is nullptr";
        QTimer::singleShot(0, this, SLOT(close()));
    }
    if (pathToDatabase == "") {
        qCritical() << "Path is empty";
        QTimer::singleShot(0, this, SLOT(close()));
    }

    // Connecting buttons to slots
    connect(ui->button_ok, SIGNAL(clicked(bool)), this, SLOT(button_ok_clicked()));
    connect(ui->button_cancel, SIGNAL(clicked(bool)), this, SLOT(close()));

    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);

    action_hidePassword = ui->lineEdit_password->addAction(IconLoader::getIcon(Icon::EyeClosed, colorTheme), QLineEdit::TrailingPosition);
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
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::Eye, colorTheme));
        ui->lineEdit_password->setEchoMode(QLineEdit::Normal);
    } else {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::EyeClosed, colorTheme));
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
        if (!DbManager::loadDb(pathToDatabase, masterKey, db, tables)) { // Trying to load database
            showMsgBox(tr("Error"), tr("Password is incorrect or database file is damaged\nTry again, please"), QMessageBox::Critical);
        }
        masterKey->clear();

        this->close();
    } else {
        showMsgBox(tr("Warning"), tr("Password line must be not empty"), QMessageBox::Warning);
    }
}



