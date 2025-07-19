#ifndef ENTRYEDITOR_H
#define ENTRYEDITOR_H

#include <QDialog>
#include <QTableView>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include "IconLoader.h"
#include "PasswordGenerator.h"
#include <QTranslator>

namespace Ui {
class EntryEditor;
}

// This class creates window when user wants to edit row.
class EntryEditor : public QDialog
{
    Q_OBJECT

public:
    // *tableView to know which row user wants to edit, *db to write changes, *model to load data from row.
    explicit EntryEditor(QWidget *parent = nullptr, QTableView *tableView = nullptr, QSqlDatabase *db = nullptr, QSqlTableModel *model = nullptr, Theme theme = Theme::Dark);
    ~EntryEditor();

private slots:
    void on_okButton_clicked(); // When user clicks Ok button

    void on_cancelButton_clicked(); // When user clicks Cancel button

    void openPasswordGenerator();

    void hidePassword();

private:
    Ui::EntryEditor *ui;

    Theme theme;
    QAction *action_generatePassword;
    QAction *action_hidePassword;

    // Storing values that got from class constructor.
    QSqlTableModel *model;
    QTableView *tableView;
    QSqlDatabase *db;

    // Fills fields in EntryEditor window with values that in row.
    void fillData();

    // Check if at least one field in window is not empty.
    bool atLeastOneNotEmpty();

    // Showing message box to user
    void showMsgBox(const QString &text);
};

#endif // ENTRYEDITOR_H
