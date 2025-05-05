#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSql/qsqldatabase.h>
#include "addpassworddialog.h"
#include "configurecolumns.h"
#include <QtSql/QSqlDatabase>
#include <QContextMenuEvent>
#include "databaseloader.h"
#include "createdatabase.h"
#include <QDesktopServices>
#include <QtSql/QSqlDriver>
#include "ui_mainwindow.h"
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include "celldelegate.h"
#include "opendatabase.h"
#include <QScopedPointer>
#include <QTemporaryFile>
#include <openssl/rand.h>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QSqlTableModel>
#include <openssl/evp.h>
#include <QActionGroup>
#include <QProgressBar>
#include "editentry.h"
#include "edittable.h"
#include <QCloseEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include "addtable.h"
#include <windows.h>
#include <QSettings>
#include <QThread>
#include <QAction>
#include <QBuffer>
#include <thread>
#include <chrono>
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
    MainWindow(QWidget *parent = nullptr, QByteArray MasterKey = "");
    ~MainWindow();

private slots:
    void on_listWidget_currentTextChanged(const QString &currentText);

private:
    Ui::MainWindow *ui;
    bool decryptDatabaseToMemory(const QString &key, QSqlDatabase &memoryDb, const QString &encryptedFilePath, QByteArray &rtrn);
    QByteArray encryptDatabase(const QString &filePath, const QByteArray &data, const QString &key);
    bool createEncryptedDatabase(const QString &dbPath, const QString &key);
    bool createAndPopulateDatabase(const QString &dbPath);
    void closeEvent(QCloseEvent *event) override;
    void configureColumns();
    bool hasSelectedRow();
    void copyText(const QString text);
    QIcon getIcon(const QString name);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QSqlTableModel *model;
    QString currentTable;
    std::vector<QString> tables;
    QSettings settings = QSettings("AlexRadik", "RadiPass");
    QByteArray key = 0;
    CellDelegate *maskColumn = new CellDelegate;
    bool isChanged = false;

public slots:
    void customMenuRequested(QPoint pos);
    void itemDoubleclicked(const QModelIndex &index);
    void copyPassword();
    void copyUsername();
    void deleteTable();
    void editTable();
    void deleteRow();
    void copyUrl();
    void openUrl();
    void editRow();
    void cfgColumns();
    void createTable();
    void addEntry();
    void createDatabase();
    void openDatabase();
    void configureEntryMenu();
    void setIconsInListWidget();
    void duplicateEntry();
    void saveAll();

private: // Context menu
    QScopedPointer<QMenu> mainContextMenu;
    QScopedPointer<QMenu> subMenuUrl;
    QScopedPointer<QAction> actionCopyUrl;
    QScopedPointer<QAction> actionOpenUrl;
    QScopedPointer<QAction> actionCopyUsername;
    QScopedPointer<QAction> actionCopyPassword;
    QScopedPointer<QAction> actionDelete;
    QScopedPointer<QAction> actionEdit;
    QScopedPointer<QAction> actionAdd;
    QScopedPointer<QAction> actionCfgColumns;

private: // Icons
    QIcon *iconTrash = new QIcon(":/icons/trash.png");
    QIcon *iconAdd = new QIcon(":/icons/add.png");
    QIcon *iconEdit = new QIcon(":/icons/edit.png");
    QIcon *iconKey = new QIcon(":/icons/key.png");
    QIcon *iconCopy = new QIcon(":/icons/copy.png");
    QIcon *iconUrl = new QIcon(":/icons/url.png");
    QIcon *iconUser = new QIcon(":/icons/user.png");
    QIcon *iconOpenUrl = new QIcon(":/icons/openUrl.png");
    QIcon *iconSettings = new QIcon(":/icons/setting.png");
    QIcon *iconOpen = new QIcon(":/icons/open.png");
    QIcon *iconCreateNew = new QIcon(":/icons/createNew.png");
    QIcon *iconClose = new QIcon(":/icons/close.png");
    QIcon *iconEntry = new QIcon(":/icons/entry.png");
    QIcon *iconLanguage = new QIcon(":/icons/language.png");
    QIcon *iconColorScheme = new QIcon(":/icons/colorScheme.png");
    QIcon *iconDuplicate = new QIcon(":/icons/duplicate.png");

    QIcon *iconBitcoin = new QIcon(":/icons/TablesOrRows/bitcoin.png");
    QIcon *iconCard = new QIcon(":/icons/TablesOrRows/card.png");
    QIcon *iconGame = new QIcon(":/icons/TablesOrRows/game.png");
    QIcon *iconHouse = new QIcon(":/icons/TablesOrRows/house.png");
    QIcon *iconMoney = new QIcon(":/icons/TablesOrRows/money.png");
    QIcon *iconNet = new QIcon(":/icons/TablesOrRows/net.png");
    QIcon *iconOffice = new QIcon(":/icons/TablesOrRows/office.png");
    QIcon *iconPc = new QIcon(":/icons/TablesOrRows/pc.png");
    QIcon *iconProgramming = new QIcon(":/icons/TablesOrRows/programming.png");

    QMap<QString, QIcon*> icons = {{"trash", iconTrash}, {"add", iconAdd}, {"edit", iconEdit}, {"key", iconKey}, {"copy", iconCopy},
                                    {"url", iconUrl}, {"user", iconUser}, {"openUrl", iconOpenUrl}, {"settings", iconSettings},
                                    {"open", iconOpen}, {"createNew", iconCreateNew}, {"close", iconClose}, {"entry", iconEntry},
                                    {"language", iconLanguage}, {"colorScheme", iconColorScheme}, {"duplicate", iconDuplicate},
                                    {"bitcoin", iconBitcoin}, {"card", iconCard}, {"game", iconGame}, {"house", iconHouse},
                                    {"money", iconMoney}, {"net", iconNet}, {"office", iconOffice}, {"pc", iconPc},
                                    {"programming", iconProgramming}};
};




#endif // MAINWINDOW_H
