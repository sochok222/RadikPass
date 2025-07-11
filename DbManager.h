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
#include <QListWidget>
#include <QDir>

// This class used to manage db.
class DbManager
{
public:
    DbManager();

    // Get id of in database.
    static bool getRowId(QSqlTableModel *model, QTableView *tableView, QSqlDatabase *db, int &rowId);

    // Showing QMessageBox to warn user if an error occurs.
    static void showMsgBox(const QString &text);

    static void search(const QString &text, QSqlDatabase *db, QListWidget *tables);

    // Loading encrypted datbase from encryptedDatabase path, decrypting it by key
    // Loaind it to *db, and pushing tables tables that are avaible to user in vector &tables
    static bool loadDb(const QString encryptedDatabase, QByteArray &key, QSqlDatabase *db, std::vector<QString> &tables);

    // This fuction will copy decrypted database to &db at &path, and load list of tabes that avaible to user to &tables.
    static bool loadTemporaryDatabase(QSqlDatabase &db, QString &path, std::vector<QString> &tables);

    // Creates and fills database in temoprary file by *db, then encrypts this database content to databasePath using the &key
    static bool createAndFillDatabase(const QString databasePath, QByteArray &key, QSqlDatabase *db);

    // This fuction saves content of database to enctypedDatabase,
    // using &key to set key with which database will be encrypted and *db to write changes.
    static bool uploadDb(const QString encryptedDatabase, QByteArray &key, QSqlDatabase *db);

    // Deletes table tableName using *db
    static bool deleteTable(QSqlDatabase *db, const QString tableName);

    // Returns decrypted data array, *encryptedData this is QByteArray that was red from encrypted database,
    // &key this is key with wich *encryptedData will be decrypted
    static QByteArray* decryptData(QByteArray *encryptedData, QByteArray &key);

    // Return encrypted data array, *data this is data that will be encrypted using &key as master-key.
    static QByteArray* encryptData(QByteArray *data, QByteArray &key);

    // This will check and correct &masterKey to correct lengh, for aes 256 this is key that has lengh of 32 bytes,
    // if key is shorter than 32 bytes it will append zeroes
    static bool checkMasterKey(QByteArray &masterKey);

    // This will safely delete temporary file &file.
    // Before removing it will rewrite content of file by zeroes, and then delete file.
    template<typename T>
    static bool deleteTemporaryFile(T &file);
};

#endif // DBMANAGER_H
