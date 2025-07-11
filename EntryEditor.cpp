#include "EntryEditor.h"
#include "ui_EntryEditor.h"
#include <qdatetime.h>
#include <qsqlerror.h>

EntryEditor::EntryEditor(QWidget *parent, QTableView *tableView, QSqlDatabase *db, QSqlTableModel *model)
    : QDialog(parent)
    , ui(new Ui::EntryEditor)
    , tableView(tableView)
    , db(db)
    , model(model)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Edit Entry"));

    // Checking if database is opened
    if(tableView == nullptr || db == nullptr || !db->isOpen()) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Something went wrong"));
        msg.addButton(QMessageBox::Ok);
        msg.exec();
        this->close();
    }

    fillData(); // Filling fiels with data from entry
}

EntryEditor::~EntryEditor() {
    delete ui;
}

void EntryEditor::fillData() {
    QSqlQuery query(*db);

    QString tableName = model->tableName(); // Receiving name of tableName
    QString getIdQuery = QString("SELECT id FROM [%1] LIMIT 1 OFFSET %2").arg(tableName).arg(tableView->currentIndex().row()); // Query that gets id from entry

    if(!query.exec(getIdQuery)) {
        QMessageBox msg;
        msg.setInformativeText(tr("Can't load data from database"));
        msg.setText(tr("Try again, please"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        this->close();
    }

    QString id;
    if(query.next())
        id = query.value(0).toString();

    QString loadData = QString("SELECT * FROM [%1] WHERE id = %2").arg(tableName).arg(id);
    if(!query.exec(loadData))
    {
        QMessageBox msg;
        msg.setInformativeText(tr("Can't load data from database"));
        msg.setText(tr("Try again, please"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        this->close();
    }
    if(query.next())
    {
        ui->title->setText(query.value(1).toString());
        ui->username->setText(query.value(2).toString());
        ui->password->setText(query.value(3).toString());
        ui->url->setText(query.value(4).toString());
        ui->notes->setText(query.value(5).toString());
    }
}

bool EntryEditor::atLeastOneNotEmpty()
{
    if(!ui->title->text().isEmpty() || !ui->username->text().isEmpty() || !ui->password->text().isEmpty() || !ui->url->text().isEmpty() || !ui->notes->toPlainText().isEmpty())
        return true;
    return false;
}


void EntryEditor::on_okButton_clicked()
{
    if(atLeastOneNotEmpty())
    {
        QSqlQuery query(*db);
        QString table = model->tableName();

        QString getId = QString("SELECT id FROM [%1] LIMIT 1 OFFSET %2").arg(table).arg(tableView->currentIndex().row());
        query.prepare(getId);
        if(!query.exec())
        {
            qDebug() << query.lastError();
        }
        query.next();
        QString id = query.value(0).toString();


        QString statement = QString("UPDATE [%1] SET Title = :title, "
                                                    "[User Name] = :username, "
                                                    "Password = :password, "
                                                    "URL = :url, "
                                                    "Notes = :notes,"
                                                    "[Last Changed] = :lastChanged "
                                                    "WHERE id == %2").arg(table).arg(id);

        query.prepare(statement);
        query.bindValue(":title", ui->title->text());
        query.bindValue(":username", ui->username->text());
        query.bindValue(":password", ui->password->text());
        query.bindValue(":url", ui->url->text());
        query.bindValue(":notes", ui->notes->toPlainText());
        query.bindValue(":lastChanged", QDateTime::currentDateTime().toString("H:mm dd/MM/yyyy"));

        if(!query.exec())
        {
            QMessageBox msg;
            msg.setText(tr("Can't edit this row\nError: ") + query.lastError().text());
            qDebug() << tr("Failed to execute statement: ") << query.lastQuery();
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
        }


        this->close();
    }else{
        QMessageBox msg;
        msg.setIcon(QMessageBox::Information);
        msg.setText(tr("At least one field must not be empty"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
}

void EntryEditor::on_cancelButton_clicked() {
    this->close();
}

