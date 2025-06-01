#ifndef _H
#define _H

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDialog>
#include "iconloader.h"
#include <QDirIterator>
#include <QStandardPaths>
#include <QString>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QIcon>
#include <QLayout>
#include <QPushButton>
#include <QPointer>
#include <qabstractitemdelegate.h>
#include <QTranslator>
#include <QDataWidgetMapper>
#include <QStandardItemModel>


namespace Ui {
class AddTable;
}

class AddTable : public QDialog
{
    Q_OBJECT

public:
    // QSqlDatabase* to write changes in opened database, *tables to append new table to vector of tables that displayed in listWidget in MainWindow
    // QString theme to know current theme
    explicit AddTable(QWidget *parent = nullptr, QSqlDatabase *db = nullptr, std::vector<QString> *tables = nullptr, QString theme = "");
    ~AddTable();

private slots:
    void on_addTableButton_clicked(); // When user wants to create new table

    void on_buttonCancel_clicked(); // When user cancels action

private:
    Ui::AddTable *ui;

    // Storing parameters that passed to class constructor
    QSqlDatabase *db;
    std::vector<QString> *tables;
    QMap<QString, QIcon*> *icons;
    QString theme;

    void loadIcons(); // This functions loads icons to QComboBox, where user can select icon that he wants

    // *mapper and *model to know which icon the user selected
    QDataWidgetMapper *mapper = new QDataWidgetMapper();
    QStandardItemModel *model = new QStandardItemModel();
};


#endif // _H
