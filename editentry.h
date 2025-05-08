#ifndef EDITENTRY_H
#define EDITENTRY_H

#include <QDialog>
#include <QTableView>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QTranslator>

namespace Ui {
class EditEntry;
}

class EditEntry : public QDialog
{
    Q_OBJECT

public:
    explicit EditEntry(QWidget *parent = nullptr, QTableView *tableView = nullptr, QSqlDatabase *db = nullptr, QSqlTableModel *model = nullptr);
    ~EditEntry();

private slots:
    void on_writeButton_clicked();

private:
    QSqlTableModel *model;
    QTableView *tableView;
    Ui::EditEntry *ui;
    QSqlDatabase *db;

    void setup();
    bool atLeastOneNotEmpty();
};

#endif // EDITENTRY_H
