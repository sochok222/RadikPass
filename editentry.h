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

// This class creates window when user wants to edit row.
class EditEntry : public QDialog
{
    Q_OBJECT

public:
    // *tableView to know which row user wants to edit, *db to write changes, *model to load data from row.
    explicit EditEntry(QWidget *parent = nullptr, QTableView *tableView = nullptr, QSqlDatabase *db = nullptr, QSqlTableModel *model = nullptr);
    ~EditEntry();

private slots:
    void on_okButton_clicked(); // When user clicks Ok button

    void on_cancelButton_clicked(); // When user clicks Cancel button

private:
    Ui::EditEntry *ui;

    // Storing values that got from class constructor.
    QSqlTableModel *model;
    QTableView *tableView;
    QSqlDatabase *db;

    // Fills fields in EditEntry window with values that in row.
    void fillData();

    // Check if at least one field in window is not empty.
    bool atLeastOneNotEmpty();
};

#endif // EDITENTRY_H
