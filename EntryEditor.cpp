#include "EntryEditor.h"
#include "ui_EntryEditor.h"
#include <qdatetime.h>
#include <qsqlerror.h>

void EntryEditor::showMsgBox(const QString &text) {
    qInfo() << Q_FUNC_INFO;
    QMessageBox msg;
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

EntryEditor::EntryEditor(QWidget *parent, QTableView *tableView, QSqlDatabase *db, QSqlTableModel *model, Theme theme)
    : QDialog(parent)
    , ui(new Ui::EntryEditor)
    , tableView(tableView)
    , db(db)
    , model(model) {
    ui->setupUi(this);
    this->setWindowTitle(tr("Edit Entry"));

    QAction *action_generatePassword = ui->line_password->addAction(IconLoader::getIcon(Icon::Dice, theme), QLineEdit::TrailingPosition);
    connect(action_generatePassword, SIGNAL(triggered(bool)), this, SLOT(openPasswordGenerator()));

    // Checking if database is opened
    if (tableView == nullptr || db == nullptr || !db->isOpen()) {
        showMsgBox(tr("Something went wrong.\nTry again please"));
    }

    fillData(); // Filling fiels with data from entry
}

EntryEditor::~EntryEditor() {
    delete ui;
}

void EntryEditor::openPasswordGenerator() {
    QString generatedPassword;
    PasswordGenerator *window_PasswordGenerator = new PasswordGenerator(this, &generatedPassword);
    window_PasswordGenerator->exec();
    delete window_PasswordGenerator;

    if (generatedPassword.size() > 0)
        ui->line_password->setText(generatedPassword);
}

void EntryEditor::fillData() {
    qInfo() << Q_FUNC_INFO;

    QSqlQuery query(*db);
    QString tableName = model->tableName(); // Name of current table
    QString getIdQuery = QString("SELECT id FROM [%1] LIMIT 1 OFFSET %2").arg(tableName).arg(tableView->currentIndex().row()); // Getting entry id
    if (!query.exec(getIdQuery)) { // Executing query
        showMsgBox(tr("Unable to load data from database.\nTry again, please."));
        qCritical() << "Unable to execute query: " << query.lastQuery() << "\nError: " << query.lastError();
        this->close();
    }

    QString id; // Variable for id of entry
    if (query.next())
        id = query.value(0).toString();


    QString loadData = QString("SELECT * FROM [%1] WHERE id = %2").arg(tableName).arg(id); // Query loads data from entry
    if (!query.exec(loadData)) { // Executing query
        showMsgBox(tr("Unable to load data from database.\nTry again, please."));
        qCritical() << "Unable to execute query: " << query.lastQuery() << "\nError: " << query.lastError();
        this->close();
    }
    if (query.next()) { // Copying data from entry to fields
        ui->line_title->setText(query.value(1).toString());
        ui->line_username->setText(query.value(2).toString());
        ui->line_password->setText(query.value(3).toString());
        ui->line_url->setText(query.value(4).toString());
        ui->line_notes->setText(query.value(5).toString());
    } else {
        showMsgBox(tr("Unable to load data from database.\nTry again, please."));
        qCritical() << "Query didn't find an entry";
        this->close();
    }
}

bool EntryEditor::atLeastOneNotEmpty() {
    // Checking if at least one field is not emtpy
    if (!ui->line_title->text().isEmpty() || !ui->line_username->text().isEmpty() || !ui->line_password->text().isEmpty() || !ui->line_url->text().isEmpty() || !ui->line_notes->toPlainText().isEmpty())
        return true;
    return false;
}


void EntryEditor::on_okButton_clicked() {
    if (atLeastOneNotEmpty()) { // Checking if at least one field is not empty
        QSqlQuery query(*db);
        QString table = model->tableName(); // Name of current table
        QString getId = QString("SELECT id FROM [%1] LIMIT 1 OFFSET %2").arg(table).arg(tableView->currentIndex().row()); // Getting entry id
        if (!query.exec(getId)) {
            showMsgBox(tr("Unable to load data from database.\nTry again, please."));
            qCritical() << "Unable to execute query: " << query.lastQuery() << "\nError: " << query.lastError();
            this->close();
        }
        query.next();
        QString id = query.value(0).toString();

        // This query updates columns of entry
        QString command = QString("UPDATE [%1] SET Title = :title, "
                                                    "[User Name] = :username, "
                                                    "Password = :password, "
                                                    "URL = :url, "
                                                    "Notes = :notes,"
                                                    "[Last Changed] = :lastChanged "
                                                    "WHERE id == %2").arg(table).arg(id);
        // Preparing query and binding values
        query.prepare(command);
        query.bindValue(":title", ui->line_title->text());
        query.bindValue(":username", ui->line_username->text());
        query.bindValue(":password", ui->line_password->text());
        query.bindValue(":url", ui->line_url->text());
        query.bindValue(":notes", ui->line_notes->toPlainText());
        query.bindValue(":lastChanged", QDateTime::currentDateTime().toString("H:mm dd/MM/yyyy"));

        if (!query.exec()) {
            showMsgBox(tr("Unable to edit this row.\nTry again, please."));
            qCritical() << "Unable to execute query: " << query.lastQuery() << "\nError: " << query.lastError();
        }
        this->close();
    } else {
        showMsgBox(tr("At least one field must not be empty"));
    }
}

void EntryEditor::on_cancelButton_clicked() {
    this->close();
}

