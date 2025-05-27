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
    explicit AddEntry(QWidget *parent = nullptr, const QSqlDatabase &db = QSqlDatabase::addDatabase("QSQLITE"), QString tableName = "");
    ~AddEntry();

private slots:
    void on_writeButton_clicked();

private:
    Ui::AddEntry *ui;
    bool atLeastOneNotEmpty();
    QSqlDatabase db;
    QString tableName;
};

#endif // ADDENTRY_H
