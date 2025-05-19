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
    explicit AddTable(QWidget *parent = nullptr, QSqlDatabase *db = nullptr, std::vector<QString> *tables = nullptr, QString theme = "");
    ~AddTable();

private slots:
    void on_addTableButton_clicked();


    void on_buttonCancel_clicked();

private:
    Ui::AddTable *ui;
    QSqlDatabase *db;
    std::vector<QString> *tables;
    QMap<QString, QIcon*> *icons;
    QString theme;
    void loadIcons();
    QDataWidgetMapper *mapper = new QDataWidgetMapper();
    QStandardItemModel *model = new QStandardItemModel();
};


#endif // _H
