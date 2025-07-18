#ifndef _H
#define _H

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDialog>
#include "IconLoader.h"
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
class TableAdder;
}

class TableAdder : public QDialog
{
    Q_OBJECT

public:
    // QSqlDatabase* to write changes in opened database, *tables to append new table to vector of tables that displayed in listWidget in MainWindow
    // QString theme to know current theme
    explicit TableAdder(QWidget *parent = nullptr, QSqlDatabase *db = nullptr, std::vector<QString> *tables = nullptr, Theme theme = Theme::Dark);
    ~TableAdder();

private slots:
    void on_addTableButton_clicked(); // When user wants to create new table

    void on_buttonCancel_clicked(); // When user cancels action

private:
    Ui::TableAdder *ui;

    void showMsgBox(const QString &message);

    enum class rtrnCodes{
        exists,
        notExists,
        cantBeUsed
    };

    // Function checks if table that user wants to create is already existing, if table exists returns true, otherwise return false.
    rtrnCodes checkIfTableExists(const QString newTable);

    // Storing parameters that passed to class constructor
    QSqlDatabase *db;
    std::vector<QString> *tables;
    QMap<QString, QIcon*> *icons;
    Theme theme;

    void loadIcons(); // This functions loads icons to QComboBox, where user can select icon that he wants

    // *mapper and *model to know which icon the user selected
    QDataWidgetMapper *mapper = new QDataWidgetMapper();
    QStandardItemModel *model = new QStandardItemModel();

    // validator to limit user input.
    QValidator *validator;
};


#endif // _H
