#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql/QSqlDatabase>
#include <qsqltablemodel.h>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QTemporaryFile>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <QTranslator>
#include <QMessageBox>
#include <QTableView>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QDir>

class DbManager
{
public:
    DbManager();

    static bool getRowId(QSqlTableModel *model, QTableView *tableView, QSqlDatabase *db, int &rowId); // Get id of in database

    static void showMsgBox(const QString text); // Showing QMessageBox to warn user if an error occurs

    // Loading encrypted datbase from encryptedDatabase path, decrypting it by key
    // Loaind it to *db, and pushing tables tables that are avaible to user in vector &tables
    static bool loadDb(const QString encryptedDatabase, QByteArray &key, QSqlDatabase *db, std::vector<QString> &tables);

    // Loading
    static bool loadTemporaryDatabase(QSqlDatabase &db, QString &path, std::vector<QString> &tables);

    static bool createAndFillDatabase(const QString databasePath, QByteArray &key, QSqlDatabase *db);
    static bool uploadDb(const QString encryptedDatabase, QByteArray &key, QSqlDatabase *db);
    static bool deleteTable(QSqlDatabase *db, const QString tablename);
    static QByteArray* decryptData(QByteArray *encryptedData, const QByteArray &key);
    static QByteArray* encryptData(QByteArray *data, const QByteArray &key);
    static bool checkMasterKey(QByteArray &masterKey);

    template<typename T>
    static bool deleteTemporaryFile(T &file);
    static bool createBackup(QSqlDatabase *db);
    static bool loadBackup(QSqlDatabase *db);
    static bool deleteBackup(QSqlDatabase *db);
    static QString backupPath;

private:
    void handleError(const QString &errorMessage);
};

#endif // DBMANAGER_H
