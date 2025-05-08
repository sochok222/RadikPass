#ifndef _H
#define _H

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDialog>
#include <QString>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QIcon>
#include <QLayout>
#include <QPushButton>
#include <QPointer>
#include <qabstractitemdelegate.h>
#include <QTranslator>


namespace Ui {
class AddTable;
}

class AddTable : public QDialog
{
    Q_OBJECT

public:
    explicit AddTable(QWidget *parent = nullptr, QSqlDatabase *db = nullptr, std::vector<QString> *tables = nullptr, QMap<QString, QIcon*> *icons = nullptr);
    ~AddTable();

private slots:
    void on_addTableButton_clicked();


    void on_buttonCancel_clicked();

private:
    Ui::AddTable *ui;
    QSqlDatabase *db;
    std::vector<QString> *tables;
    QString icon = "key";
    QMap<QString, QIcon*> *icons;
    QVector<QString> iconNames = {"entry", "trash", "key", "url", "user", "settings", "bitcoin", "card", "game", "house", "money", "net", "office", "pc", "programming"};
};


#endif // _H
