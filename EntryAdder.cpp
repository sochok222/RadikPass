#include "EntryAdder.h"
#include "ui_EntryAdder.h"
#include <qsqlerror.h>
#include <qtimer.h>

void showMsgBox(const QString &text) {
    QMessageBox msg;
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}

EntryAdder::EntryAdder(QWidget *parent, QSqlDatabase *db, QString tableName, Theme theme)
    : QDialog(parent)
    , ui(new Ui::EntryAdder)
    , tableName(tableName)
    , db(db)
    , theme(theme)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Add Entry"));

    connect(ui->button_cancel, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->button_ok, SIGNAL(clicked(bool)), this, SLOT(button_ok_clicked()));

    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);

    action_hidePassword = ui->lineEdit_password->addAction(IconLoader::getIcon(Icon::EyeClosed, theme), QLineEdit::TrailingPosition);
    connect(action_hidePassword, SIGNAL(triggered(bool)), this, SLOT(hidePassword()));

    action_generatePassword = ui->lineEdit_password->addAction(IconLoader::getIcon(Icon::Dice, theme), QLineEdit::TrailingPosition);
    connect(action_generatePassword, SIGNAL(triggered(bool)), this, SLOT(openPasswordGenerator()));


    // Check if database is open
    if (db == nullptr || !db->isOpen()) {
        showMsgBox(tr("Can't open database"));
        QTimer::singleShot(0, this, SLOT(close()));
    }
}

EntryAdder::~EntryAdder() {
    delete action_generatePassword;
    delete action_hidePassword;
    delete ui;
}


void EntryAdder::hidePassword() {
    if (ui->lineEdit_password->echoMode() == QLineEdit::EchoMode::Password) {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::Eye, theme));
        ui->lineEdit_password->setEchoMode(QLineEdit::Normal);
    } else {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::EyeClosed, theme));
        ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    }
}


void EntryAdder::openPasswordGenerator() {
    QString generatedPassword;
    PasswordGenerator *window_PasswordGenerator = new PasswordGenerator(this, &generatedPassword);
    window_PasswordGenerator->exec();
    delete window_PasswordGenerator;

    if (generatedPassword.size() > 0)
        ui->lineEdit_password->setText(generatedPassword);
}


bool EntryAdder::atLeastOneNotEmpty() {
    // Check if at least one field is not empty and return true if it is.
   if (!ui->lineEdit_title->text().isEmpty() || !ui->lineEdit_username->text().isEmpty() || !ui->lineEdit_password->text().isEmpty() || !ui->lineEdit_url->text().isEmpty() || !ui->lineEdit_notes->toPlainText().isEmpty())
        return true;
    return false;
}


void EntryAdder::button_ok_clicked() {
    qDebug() << Q_FUNC_INFO;

    QSqlQuery query(*db); // QSqlQuery to access QSqlDatabase db.

    // Check if at least one field not empty
    if (atLeastOneNotEmpty()) {
        // Database query that will insert row in table.
        QString insertData = QString("INSERT INTO [%1] (Title, [User Name], Password, URL, Notes, [Creation Time], [Last Changed]) VALUES ("
                                     ":title, :username, :password, :url, :notes, :creationTime, :lastChanged"
                                     ")").arg(tableName);

        // prepare() allows to bindValues.
        query.prepare(insertData);

        // Binding values.
        query.bindValue(":title", ui->lineEdit_title->text());
        query.bindValue(":username", ui->lineEdit_username->text());
        query.bindValue(":password", ui->lineEdit_password->text());
        query.bindValue(":url", ui->lineEdit_url->text());
        query.bindValue(":notes", ui->lineEdit_notes->toPlainText());
        query.bindValue(":creationTime", QDateTime::currentDateTime().toString("H:mm dd/MM/yyyy")); // Using QDateTime to get current date and time.
        query.bindValue(":lastChanged", QDateTime::currentDateTime().toString("H:mm dd/MM/yyyy"));

        if (!query.exec()) {
            qDebug() << "Error: " << query.lastError().text() << "\nLast query: " << query.lastQuery();
            showMsgBox("Unable to save changes.\nTry again please.");
        }

        // Closing window.
        this->reject();
    } else { // If all fields are empty needs to show user error.
        showMsgBox(tr("At least one field must not be empty"));
    }
}
