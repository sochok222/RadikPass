#ifndef ADDPASSWORDDIALOG_H
#define ADDPASSWORDDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QDateTime>
#include <QTranslator>


namespace Ui {
class AddPasswordDialog;
}

class AddPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPasswordDialog(QWidget *parent = nullptr, const QSqlDatabase &db = QSqlDatabase::addDatabase("QSQLITE"), QString tableName = "");
    ~AddPasswordDialog();

private slots:
    void on_writeButton_clicked();

private:
    Ui::AddPasswordDialog *ui;
    bool atLeastOneNotEmpty();
    QSqlDatabase db;
    QString tableName;
};

#endif // ADDPASSWORDDIALOG_H
