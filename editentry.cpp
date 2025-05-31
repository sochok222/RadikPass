#include "editentry.h"
#include "ui_editentry.h"
#include <qdatetime.h>
#include <qsqlerror.h>

EditEntry::EditEntry(QWidget *parent, QTableView *tableView, QSqlDatabase *db, QSqlTableModel *model)
    : QDialog(parent)
    , ui(new Ui::EditEntry)
    , tableView(tableView)
    , db(db)
    , model(model)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Edit Entry"));
    setup();
}

EditEntry::~EditEntry()
{
    delete ui;
}

void EditEntry::setup()
{
    if(tableView == nullptr || db == nullptr)
    {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Something went wrong"));
        msg.addButton(QMessageBox::Ok);
        msg.exec();
    }
    QSqlQuery query(*db);

    QString table = model->tableName();
    QString getId = QString("SELECT id FROM [%1] LIMIT 1 OFFSET %2").arg(table).arg(tableView->currentIndex().row());

    if(!query.exec(getId))
    {
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

    QString loadData = QString("SELECT * FROM [%1] WHERE id = %2").arg(table).arg(id);
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

bool EditEntry::atLeastOneNotEmpty()
{
    if(!ui->title->text().isEmpty() || !ui->username->text().isEmpty() || !ui->password->text().isEmpty() || !ui->url->text().isEmpty() || !ui->notes->toPlainText().isEmpty())
        return true;
    return false;
}


void EditEntry::on_writeButton_clicked()
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
