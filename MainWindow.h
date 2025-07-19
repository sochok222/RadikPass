#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSql/qsqldatabase.h>
#include "EntryAdder.h"
#include "ColumnsConfigurator.h"
#include "PasswordGenerator.h"
#include <QtSql/QSqlDatabase>
#include <QContextMenuEvent>
#include "DbManager.h"
#include "DbCreator.h"
#include <QDesktopServices>
#include "ui_MainWindow.h"
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include "CellDelegate.h"
#include "DbOpener.h"
#include <QScopedPointer>
#include <openssl/rand.h>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QSqlTableModel>
#include <openssl/evp.h>
#include "IconLoader.h"
#include <QActionGroup>
#include <QKeySequence>
#include "EntryEditor.h"
#include "TableEditor.h"
#include <QCloseEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QTranslator>
#include <QScrollBar>
#include "TableAdder.h"
#include <QShortcut>
#include <QSettings>
#include <windows.h>
#include <QAction>
#include <QOBject>
#include <QDebug>
#include <QFile>
#include <QMenu>
#include <QDir>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, QByteArray MasterKey = "", QTranslator *translator = nullptr, Theme theme = Theme::Dark);
    ~MainWindow();

public slots:
    void on_listWidget_currentTextChanged(const QString &currentText); // When user changes current element in listWidget that holds names of tables.

private:
    Ui::MainWindow *ui;

    // Remembering last used table
    int lastUsedTable = 0;

    // Setting up shortcuts
    void setShortcuts();

    // Catch close event to ask if user wants save changes or not (if isChanged = true).
    void closeEvent(QCloseEvent *event) override;

    // Checking if user has selected row in tableView.
    bool hasSelectedRow();

    // This function copies text to windows clipboard.
    void copyText(const QString &text);

    // Storing opened database in QSqlDatabase.
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    // *model that will be shown in tableView.
    QSqlTableModel *model;

    // Current selected table.
    QString currentTable;

    // Vector with avaible to user tables, this vector loaded to listWidget.
    std::vector<QString> tables;

    // Loading settings.
    QSettings settings = QSettings("AlexRadik", "RadikPass");

    // Key with which database will be decrypted/encrypted
    QByteArray key = 0;

    // If user makes any changes in database needs to set this state to true.
    bool isChanged = false;

    // Translator that translates program.
    QTranslator *translator;

    // Current theme.
    Theme theme;

    // Cell delegate that will mask column by *.
    CellDelegate *maskColumn = new CellDelegate;

    // For some reason program do not translates actions that allows user to switch color theme.
    // To fix that, every time when user wants to translate program needs to manually reset actions.
    void setColorThemeActions();

    // Setting tooltips to buttons
    void setTooltips();

    // This function loads icons in interface according to current theme.
    void loadIcons();

    // Connecting buttons to slots
    void connectButtons();

    // Connecting actions in toolbar to slots
    void connectActions();

    // Showing message boxes with error message
    void showMsgBox(const QString &title, const QString &text, const QMessageBox::Icon &icon);

public slots:
    // When user clicks on right button of the mouse.
    // This fuction olso check checks where user clicked.
    void customMenuRequested(QPoint pos);

    // When user doubleclicks at row in tableView.
    // This fuction checks in which cell user clicked, depending on that this fucntion will copy content or open link in browser.
    void itemDoubleclicked(const QModelIndex &index);

    // Copy password from row in tableView.
    void copyPassword();

    // Copy username from row in tableView.
    void copyUsername();

    // Delete table from opened database.
    void deleteTable();

    // Edit table in opened database.
    void editTable();

    // Deleting selected row.
    void deleteEntry();

    // Copies link from selected row.
    void copyUrl();

    // Opens link from selected row, link will be opened in standard browser.
    void openUrl();

    // Editing selected row.
    void editRow();

    // This function will hide/show and mask/unmask columns according to settings that loaded from QSettings.
    void cfgColumns();

    // Function will create dialog of AddTable class.
    void createTable();

    // Function will create dialog of AddEntry class.
    void addEntry();

    // Function will create dialog of CreateDatabase class.
    void createDatabase();

    // Function will create dialog of OpenDatabase class.
    void openDatabase();

    // This function will disable or enable actions in View menu in toolbar
    void configureEntryMenu();

    // This function will set icons in listWidget according to current theme.
    void loadIconsToListWidget();

    // This function will duplicate selected row in tableView.
    void duplicateEntry();

    // This function saves all changes to encrypted database.
    void saveAll();

    // Setting horizontal headers of columns in the tableView
    void setHeaders();

    // This function will hide/show and mask/unmask columns according to settings that loaded from QSettings.
    void configureColumns();

    // Opening PasswordGenerator window
    void openPasswordGenerator();

    // This functions are performing change of language in program.
    void setUkrainianLanguage();
    void setEnglishLanguage();
    void setGermanLanguage();

    // This functions are performing change of color theme in program.
    void setSystemColorTheme();
    void setDarkColorTheme();
    void setLightColorTheme();

private slots:
    void on_searchBar_textChanged(const QString &arg1);


private:
    // Context menu menus and actions.
    QScopedPointer<QMenu> menu_contextMenu;
    QScopedPointer<QMenu> menu_url;
    QScopedPointer<QAction> action_copyUrl;
    QScopedPointer<QAction> action_OpenUrl;
    QScopedPointer<QAction> action_copyUsername;
    QScopedPointer<QAction> action_copyPassword;
    QScopedPointer<QAction> action_delete; // This action will delete either entry or table, depending on place where the context menu was called
    QScopedPointer<QAction> action_edit;
    QScopedPointer<QAction> action_add;
    QScopedPointer<QAction> action_configureColumns;

    // menubar
    // Color theme actions, and group to group that actions.
    QScopedPointer<QActionGroup> group_colorThemes;
    QScopedPointer<QAction> action_systemTheme;
    QScopedPointer<QAction> action_darkTheme;
    QScopedPointer<QAction> action_lightTheme;

    // Shortcuts
    QScopedPointer<QShortcut> shortcutOpen;
    QScopedPointer<QShortcut> shortcutClose;
    QScopedPointer<QShortcut> shortcutDuplicate;
    QScopedPointer<QShortcut> shortcutDelete;
    QScopedPointer<QShortcut> shortcutAddEntry;
    QScopedPointer<QShortcut> shortcutNewDatabase;
};




#endif // MAINWINDOW_H
