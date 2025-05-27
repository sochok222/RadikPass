#include "addentry.h"
#include "mainwindow.h"
#include "ui_addentry.h"
#include <qsqlerror.h>

AddEntry::AddEntry(QWidget *parent, const QSqlDatabase &db, QString tableName)
    : QDialog(parent)
    , ui(new Ui::AddEntry)
    , tableName(tableName)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Add Entry"));
}

AddEntry::~AddEntry()
{
    delete ui;
}




bool AddEntry::atLeastOneNotEmpty()
{
    if(!ui->title->text().isEmpty() || !ui->username->text().isEmpty() || !ui->password->text().isEmpty() || !ui->url->text().isEmpty() || !ui->notes->toPlainText().isEmpty())
        return true;
    return false;
}



void AddEntry::on_writeButton_clicked()
{
    qDebug() << Q_FUNC_INFO; // Writing function names to see where error appears, all this messages shown in Application Output

    QSqlQuery query(db);



    if(atLeastOneNotEmpty())
    {
        QString insertData = QString("INSERT INTO [%1] (Title, [User Name], Password, URL, Notes, [Creation Time]) VALUES ("
                                     ":title, :username, :password, :url, :notes, :time"
                                     ")").arg(tableName);
        qDebug() << "query: " << insertData;
        query.prepare(insertData);
        query.bindValue(":title", ui->title->text());
        query.bindValue(":username", ui->username->text());
        query.bindValue(":password", ui->password->text());
        query.bindValue(":url", ui->url->text());
        query.bindValue(":notes", ui->notes->toPlainText());
        query.bindValue(":time", QDateTime::currentDateTime());
        if(!query.exec())
        {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("Error: ") + query.lastError().text() + tr("\nLast query: ") + query.lastQuery());
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
