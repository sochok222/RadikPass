#include "EntryAdder.h"
#include "ui_EntryAdder.h"
#include <qsqlerror.h>

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
{
    ui->setupUi(this);
    // Set window name.
    this->setWindowTitle(tr("Add Entry"));

    QAction *action_generatePassword = ui->password->addAction(IconLoader::getIcon(Icon::dice, theme), QLineEdit::TrailingPosition);
    connect(action_generatePassword, SIGNAL(triggered(bool)), this, SLOT(openPasswordGenerator()));

    // Check if database is open
    if(db == nullptr || !db->isOpen()) {
        showMsgBox(tr("Can't open database"));
        this->close();
    }
}

EntryAdder::~EntryAdder() {
    delete ui;
}


void EntryAdder::openPasswordGenerator() {
    QString generatedPassword;
    PasswordGenerator *window_PasswordGenerator = new PasswordGenerator(this, &generatedPassword);
    window_PasswordGenerator->exec();
    delete window_PasswordGenerator;

    if (generatedPassword.size() > 0)
        ui->password->setText(generatedPassword);
}


bool EntryAdder::atLeastOneNotEmpty() {
    // Check if at least one field is not empty and return true if it is.
   if (!ui->title->text().isEmpty() || !ui->username->text().isEmpty() || !ui->password->text().isEmpty() || !ui->url->text().isEmpty() || !ui->notes->toPlainText().isEmpty())
        return true;
    return false;
}



void EntryAdder::on_okButton_clicked() {
    qDebug() << Q_FUNC_INFO;

    QSqlQuery query(*db); // QSqlQuery to access QSqlDatabase db.

    // Check if at least one field not empty
    if (atLeastOneNotEmpty())
    {
        // Database query that will insert row in table.
        QString insertData = QString("INSERT INTO [%1] (Title, [User Name], Password, URL, Notes, [Creation Time], [Last Changed]) VALUES ("
                                     ":title, :username, :password, :url, :notes, :creationTime, :lastChanged"
                                     ")").arg(tableName);

        // prepare() allows to bindValues.
        query.prepare(insertData);

        // Binding values.
        query.bindValue(":title", ui->title->text());
        query.bindValue(":username", ui->username->text());
        query.bindValue(":password", ui->password->text());
        query.bindValue(":url", ui->url->text());
        query.bindValue(":notes", ui->notes->toPlainText());
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
