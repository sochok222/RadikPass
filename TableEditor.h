#ifndef TABLEEDITOR_H
#define TABLEEDITOR_H

#include <QDialog>
#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QIcon>
#include <QTranslator>
#include <QSqlDatabase>
#include <QListWidget>
#include <QSqlQuery>
#include <QValidator>
#include <QRegularExpressionValidator>
#include <QStandardItemModel>
#include <QDataWidgetMapper>
#include <QStandardPaths>
#include <QDirIterator>
#include <QStandardItem>
#include "IconLoader.h"

namespace Ui {
class TableEditor;
}

// This class will create window when user wants to edit table (name or icon of table).
class TableEditor : public QDialog
{
    Q_OBJECT

public:
    // *db to write changes, tableName to know which table user wants to edit,
    // *listWidget stores rows with tables(names and icons) in MainWindow, this class will change values in listWidget by pointer.
    // theme to load icons according to current theme.
    explicit TableEditor(QWidget *parent = nullptr, QSqlDatabase *db = nullptr, const QString tableName = "", QListWidget *listWidget = nullptr, Theme theme = Theme::Dark);
    ~TableEditor();

private:
    Ui::TableEditor *ui;

    // Values from class constructor.
    QSqlDatabase *db;
    QString tableName;
    QListWidget *listWidget;
    Theme theme;

    // This will catch close event, and if user maked some changes needs to ask if user wants to save changes.
    void closeEvent(QCloseEvent *event) override;

    // To check if icon or name was changed.
    bool isNameChanged = false;
    bool isIconChanged = false;

    // Loads icon to QComboBox according to current theme.
    void loadIcons();

    // This will write changes to database.
    bool saveChanges();

    // This fuction shows user MessageBox if error occurs.
    void showMsgBox(const QString text);

    // For mapping QComboBox to QStandardItemModel, to know which icon user chosed.
    QStandardItemModel *model;
    QDataWidgetMapper *mapper;

public slots:
    // Catch changes.
    void iconChanged() {isIconChanged = true;};
    void textChanged() {isNameChanged = true;};

    // Connecting iconChanged() and textChanged() to signals that emits on changing.
    void setConnections();

private slots:
    void on_buttonSave_clicked(); // When user clicks Save button.
    void on_buttonCancel_clicked(); // When user clicks Cancel button.
};



#endif // TABLEEDITOR_H
