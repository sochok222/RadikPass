#ifndef ENTRYADDER_H
#define ENTRYADDER_H

#include <QDialog>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QDateTime>
#include <QTranslator>
#include <QScopedPointer>
#include "PasswordGenerator.h"
#include "IconLoader.h"


namespace Ui {
class EntryAdder;
}

class EntryAdder : public QDialog
{
    Q_OBJECT

public:
    // QSqlDatabase *db to write changes in opened database, tableName to know in which table.
    explicit EntryAdder(QWidget *parent = nullptr, QSqlDatabase *db = nullptr, const QString tableName = "", Theme theme = Theme::Dark);
    ~EntryAdder();

private slots:
    void on_okButton_clicked(); // When user clicks ok button this fuction executes
    void openPasswordGenerator();
    void hidePassword();

private:
    Ui::EntryAdder *ui;
    Theme theme;
    QAction *action_generatePassword;
    QAction *action_hidePassword;
    bool atLeastOneNotEmpty(); // Function that checks that al leas one field is not emtpy
    QSqlDatabase *db; // Storing database that passed to the class constructor
    QString tableName; // Storing Name of current table that passed to the class constructor
};

#endif // ENTRYADDER_H
