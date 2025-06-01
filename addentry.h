#ifndef ADDENTRY_H
#define ADDENTRY_H

#include <QDialog>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QDateTime>
#include <QTranslator>


namespace Ui {
class AddEntry;
}

class AddEntry : public QDialog
{
    Q_OBJECT

public:
    // QSqlDatabase &db to write changes in opened database and tableName to know in which table.
    explicit AddEntry(QWidget *parent = nullptr, const QSqlDatabase &db = QSqlDatabase::addDatabase("QSQLITE"), QString tableName = "");
    ~AddEntry();

private slots:
    void on_okButton_clicked(); // When user clicks ok button this fuction executes

private:
    Ui::AddEntry *ui;

    bool atLeastOneNotEmpty(); // Function that checks that al leas one field is not emtpy

    QSqlDatabase db; // Storing database that passed to the class constructor
    QString tableName; // Storing Name of current table that passed to the class constructor
};

#endif // ADDENTRY_H
