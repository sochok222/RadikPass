#include "databaseloader.h"
#include <qsqldriver.h>
#include <QList>
#include <QModelIndex>
#include <QTableWidgetItem>
#include <QPainter>
#include <QStyledItemDelegate>
#include <qsqlrecord.h>

DatabaseLoader::DatabaseLoader() {}

QString DatabaseLoader::backupPath = "";

void DatabaseLoader::showMsgBox(const QString text)
{
    qDebug() << Q_FUNC_INFO;
    QMessageBox msg;
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}

bool DatabaseLoader::createBackup(QSqlDatabase *db)
{
    qDebug() << Q_FUNC_INFO;
    backupPath.clear();

    QTemporaryFile tmp;
    tmp.setAutoRemove(false);

    if(!tmp.open())
    {
        showMsgBox("Can't open temporary file\nFunc: " Q_FUNC_INFO);
        return false;
    }

    backupPath = tmp.fileName();

    QSqlQuery query(*db);

    query.prepare("ATTACH DATABASE :path AS TempDb");
    query.bindValue(":path", backupPath);
    if(!query.exec())
    {
        showMsgBox("Can't attach temporary file\nError: " + query.lastError().text() + "\nFunc: " + Q_FUNC_INFO);
        return false;
    }

    query.prepare("SELECT name, sql FROM main.sqlite_master WHERE type='table' AND name != 'sqlite_sequence'");
    if (!query.exec()) {
        showMsgBox("Can't load data to temporary file\nError: " + query.lastError().text() + "\nFunc: "+Q_FUNC_INFO);
        return false;
    }

    while (query.next()) {
        QString tableName = query.value(0).toString();
        QString createSQL = query.value(1).toString();
        createSQL.replace("CREATE TABLE ", "CREATE TABLE TempDb.");

        QSqlQuery createQry(*db);
        if (!createQry.exec(createSQL)) {
            showMsgBox("Create query error\nDeleting backup file\nError: " + query.lastError().text() + "\nFunc: " + Q_FUNC_INFO);
            return false;
        }

        QString copySQL = QString("INSERT INTO TempDb.[%1] SELECT * FROM [%1]").arg(tableName);
        if (!createQry.exec(copySQL)) {
            showMsgBox("Failed to copy data\nError: " + query.lastError().text() + "\nFunc: " + Q_FUNC_INFO);
        } else {
            qDebug() << "Table" << tableName << "copied.";
        }
    }


    // Detaching database
    query.prepare("DETACH DATABASE TempDb");
    if(!query.exec())
    {
        showMsgBox("Can't detach tempDb database\nError: " + query.lastError().text() + "\nFunc: " + Q_FUNC_INFO);
    }
    return true;
}

bool DatabaseLoader::loadBackup(QSqlDatabase *db)
{
    QSqlQuery query(*db);

    query.prepare("ATTACH DATABASE :path AS TempDb");
    query.bindValue(":path", backupPath);
    if(!query.exec())
    {
        showMsgBox("Can't attach temporary file\nError: " + query.lastError().text() + "\nFunc: " + Q_FUNC_INFO);
        return false;
    }

    query.prepare("SELECT name, sql FROM TempDb.sqlite_master WHERE type='table' AND name != 'sqlite_sequence'");
    if (!query.exec()) {
        showMsgBox("Can't load data to temporary file\nError: " + query.lastError().text() + "\nFunc: "+Q_FUNC_INFO);
        return false;
    }

    while (query.next()) {
        QString tableName = query.value(0).toString();
        QString createSQL = query.value(1).toString();
        createSQL.replace("CREATE TABLE ", "CREATE TABLE main.");

        QSqlQuery createQry(*db);
        if (!createQry.exec(createSQL)) {
            showMsgBox("Create query error\nDeleting backup file\nError: " + query.lastError().text() + "\nFunc: " + Q_FUNC_INFO);
            return false;
        }

        QString copySQL = QString("INSERT INTO main.[%1] SELECT * FROM TempDb.[%1]").arg(tableName);
        if (!createQry.exec(copySQL)) {
            showMsgBox("Failed to copy data\nError: " + query.lastError().text() + "\nFunc: " + Q_FUNC_INFO);
        }
    }


    // Detaching database
    query.prepare("DETACH DATABASE TempDb");
    if(!query.exec())
    {
        showMsgBox("Can't detach tempDb database\nError: " + query.lastError().text() + "\nFunc: " + Q_FUNC_INFO);
    }

    return true;
}

bool DatabaseLoader::deleteBackup(QSqlDatabase *db)
{
    QFile file(backupPath);
    QByteArray zeroes = QByteArray(file.size(), 0);

    if(!file.open(QIODevice::WriteOnly))
    {
        showMsgBox("Can't open temporary file\nFunc: " Q_FUNC_INFO);
        return false;
    }

    if(!file.write(zeroes))
    {
        showMsgBox("Can't write zeroes to temporary file\nFunc: " Q_FUNC_INFO);
        return false;
    }

    file.close();
    if(!file.remove())
    {
        showMsgBox("Can't delete temporary file\nFunc: " Q_FUNC_INFO);
        return false;
    }

    return true;
}

void DatabaseLoader::handleError(const QString &errorMessage)
{
    qDebug() << Q_FUNC_INFO;
    QMessageBox msg;
    msg.setText(errorMessage);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
    exit(EXIT_FAILURE);
}

bool DatabaseLoader::getRowId(QSqlTableModel *model, QTableView *tableView, QSqlDatabase *db, int &rowId)
{
    QSqlQuery query(*db);

    QString getId = QString("SELECT id FROM %1 LIMIT 1 OFFSET %2").arg(model->tableName()).arg(tableView->currentIndex().row());
    query.prepare(getId);
    if(!query.exec())
    {
        return false;
    }
    query.next();
    rowId = query.value(0).toInt();
    return true;
}

bool DatabaseLoader::deleteTable(QSqlDatabase *db, const QString tablename)
{
    qDebug() << Q_FUNC_INFO;
    QSqlQuery query(*db);

    query.prepare("DELETE FROM TablesSettings WHERE [Table] = :name");
    query.bindValue(":name", tablename);
    if(!query.exec())
    {
        qDebug() << "Can't execute query";
        return false;
    }

    QString command = QString("DROP TABLE [%1]").arg(tablename);

    if(!query.exec(command))
    {
        qDebug() << "Can't execute query";
        query.exec("INSERT INTO TablesSettings ([Table]) VALUES ('"+tablename+"')");
        return false;
    }
    return true;
}


QByteArray* DatabaseLoader::encryptData(QByteArray *data, const QByteArray &key)
{
    qDebug() << Q_FUNC_INFO;
    if (key.size() < 32) return {}; // AES-256 вимагає 32-байтний ключ

    // Генеруємо IV (16 байт для AES-CBC)
    QByteArray *iv = new QByteArray(16, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(iv->data()), iv->size());

    // Контекст OpenSSL
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return {};

    // Ініціалізація AES-256-CBC
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                           reinterpret_cast<const unsigned char*>(key.data()),
                           reinterpret_cast<const unsigned char*>(iv->data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // Буфер для шифрованих даних
    QByteArray *encrypted = new QByteArray;
    encrypted->resize(data->size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));

    int len = 0, totalLen = 0;
    if (EVP_EncryptUpdate(ctx,
                          reinterpret_cast<unsigned char*>(encrypted->data()), &len,
                          reinterpret_cast<const unsigned char*>(data->data()), data->size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }
    totalLen += len;

    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(encrypted->data()) + totalLen, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }
    totalLen += len;
    encrypted->resize(totalLen);
    EVP_CIPHER_CTX_free(ctx);

    qDebug() << encrypted->size();
    qDebug() << encrypted->data();
    QByteArray *ret = new QByteArray;
    ret->append(*iv + *encrypted);
    delete iv;
    delete encrypted;
    return ret; // Повертаємо IV + зашифровані дані
}

QByteArray* DatabaseLoader::decryptData(QByteArray *encryptedData, const QByteArray &key)
{
    qDebug() << Q_FUNC_INFO;
    if (encryptedData->size() < 16 || key.size() < 32) return {};

    QByteArray iv = encryptedData->left(16);
    QByteArray cipherText = encryptedData->mid(16);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return {};

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                           reinterpret_cast<const unsigned char*>(key.data()),
                           reinterpret_cast<const unsigned char*>(iv.data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    QByteArray *decrypted = new QByteArray;
    decrypted->resize(cipherText.size());

    int len = 0, totalLen = 0;
    if (EVP_DecryptUpdate(ctx,
                          reinterpret_cast<unsigned char*>(decrypted->data()), &len,
                          reinterpret_cast<const unsigned char*>(cipherText.data()), cipherText.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }
    totalLen += len;

    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(decrypted->data()) + totalLen, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }
    totalLen += len;
    decrypted->resize(totalLen);
    EVP_CIPHER_CTX_free(ctx);

    return decrypted;
}

bool DatabaseLoader::loadTemporaryDatabase(QSqlDatabase &db, QString &path, std::vector<QString> &tables)
{
    qDebug() << Q_FUNC_INFO;

    // Connecting database to memory
    db.setDatabaseName(":memory:");
    // Returning false if can`t open datatbase
    if(!db.open())
        return false;
    QSqlQuery qry(db);
    // Executing query that attaching temporary database as tempDb
    qry.prepare("ATTACH DATABASE :path as tempDb");
    qry.bindValue(":path", path);
    if(!qry.exec())
    {
        qDebug() << "Can`t attach temporary database, query error: " << qry.lastError().text();
        return false;
    }

    qry.prepare("SELECT name, sql FROM tempDb.sqlite_master WHERE type='table' AND name != 'sqlite_sequence'");
    if (!qry.exec()) {
        qDebug() << "Can't get create SQLs:" << qry.lastError().text();
        return false;
    }

    tables.clear(); // Clearing list of tables
    while (qry.next()) {
        QString tableName = qry.value(0).toString();
        QString createSQL = qry.value(1).toString();

        qDebug() << "Creating table:" << tableName;
        QSqlQuery createQry(db);
        if (!createQry.exec(createSQL)) {
            qDebug() << "Create table failed:" << createQry.lastError().text();
            continue;
        }

        QString copySQL = QString("INSERT INTO [%1] SELECT * FROM tempDb.[%1]").arg(tableName);
        if (!createQry.exec(copySQL)) {
            qDebug() << "Insert failed:" << createQry.lastError().text();
        } else {

            if(tableName != "TablesSettings")tables.push_back(tableName);
        }
    }

    qry.prepare("DETACH DATABASE tempDb");
    if(!qry.exec())
    {
        qDebug() << "Can`t detach temporary database, query error: " <<  qry.lastError().text();
        return false;
    }
    // Func. returns true if it executes to the end
    return true;
}

bool DatabaseLoader::checkMasterKey(QByteArray &masterKey)
{
    qDebug() << Q_FUNC_INFO;
    // If masterKey has less bytes than it must have, this will append zeroes to fill it
    // Else it will remove excrescent bytes
    if(masterKey.size() < 32)
    {
        QByteArray toAppend(32-masterKey.size(), '0');
        masterKey.append(toAppend);
        return false;
    }else if(masterKey.size() > 32)
    {
        masterKey.remove(32, masterKey.size());
        return false;
    }
    return true;
}

template<typename T>
bool DatabaseLoader::deleteTemporaryFile(T &file)
{
    qDebug() << Q_FUNC_INFO;
    // Opening file and reading content from it
    qDebug() << "File size: " << file.size();
    // Creating an QByteArray with zeroes in size of temporary file
    QByteArray *array = new QByteArray(file.size(), '0');
    // Trying to open file
    if(!file.open())
    {
        qDebug() << "Can`t open file";
        delete array;
        return false;
    }
    if(!file.write(*array))
    {
        qDebug() << "Can`t write data to file";
        file.close();
        file.remove();
        delete array;
        return false;
    }
    file.close();
    if(!file.remove())
    {
        qDebug() << "Can`t delete file";
        delete array;
        return false;
    }
    delete array;
    return true;
}

bool DatabaseLoader::loadDb(const QString encryptedDatabase, QByteArray &key, QSqlDatabase *db, std::vector<QString> &tables)
{
    qDebug() << Q_FUNC_INFO;
    // Setting master-key
    if(!checkMasterKey(key)) qDebug() << "Your master-key lenght was incorrect, now it is correct";

    // Setting database path
    QFile encrypted(encryptedDatabase);

    // Encrypting and writing that content to encrypted database
    encrypted.open(QIODevice::ReadWrite);

    // Than I need to create QTemporaryFile and write decrypted data to it
    QTemporaryFile temp;
    temp.setAutoRemove(false);
    if(!temp.open())
    {
        qDebug("Can`t open temporary file");
        return false;
    }

    // Trying to write decrypted data
    encrypted.seek(0);
    QByteArray *encData = new QByteArray(encrypted.readAll());
    encData = decryptData(encData, key);
    if(encData == nullptr || encData->size() == 0)
    {
        return false;
        qDebug() << "decryptData returned nullptr";
    }
    if(!temp.write(*encData))
    {
        qDebug("Can`t write decrypted data to temporary file");
        return false;
    }
    delete encData;
    temp.close();

    // Copying database path to QString
    QString databasePath(temp.fileName());
    qDebug() << "Temporary database path: " << databasePath;

    // Trying to load temporary database to :memory database, and then safety delete temporary file
    if(!loadTemporaryDatabase(*db, databasePath, tables))
    {
        qDebug("Can`t load temporary database to :memory:");
        return false;
    }
    if(!deleteTemporaryFile(temp))
    {
        qDebug("Can`t delete temporary database");
        return false;
    }

    return true;
}

bool DatabaseLoader::uploadDb(const QString encryptedDatabase, QByteArray &key, QSqlDatabase *db)
{
    qDebug() << Q_FUNC_INFO;
    // Creating temporary file to save changes
    QTemporaryFile tmp;
    tmp.setAutoRemove(false);
    if(!tmp.open())
    {
        qDebug() << "Error in: " << Q_FUNC_INFO;
        qDebug("Can`t open temporary file to write new database");
        return false;
    }

    QSqlQuery qry(*db);

    // This code will attach database that stored in :path as tempDb
    qry.prepare("ATTACH DATABASE :path as tempDb");
    qry.bindValue(":path", tmp.fileName());
    if(!qry.exec())
    {
        qDebug() << "Error in: " << Q_FUNC_INFO;
        qDebug() << "Can`t attach temporary database, query error: " << qry.lastError().text();
        return false;
    }

    qry.prepare("SELECT name, sql FROM main.sqlite_master WHERE type='table' AND name != 'sqlite_sequence'");
    if (!qry.exec()) {
        qDebug() << "Can't get create SQLs:" << qry.lastError().text();
        return false;
    }

    while (qry.next()) {
        QString tableName = qry.value(0).toString();
        QString createSQL = qry.value(1).toString();
        createSQL.replace("CREATE TABLE ", "CREATE TABLE TempDb.");

        qDebug() << "Creating table:" << tableName;
        QSqlQuery createQry(*db);
        if (!createQry.exec(createSQL)) {
            qDebug() << "Create table failed:" << createQry.lastError().text();
            continue;
        }

        QString copySQL = QString("INSERT INTO tempDb.[%1] SELECT * FROM [%1]").arg(tableName);
        if (!createQry.exec(copySQL)) {
            qDebug() << "Insert failed:" << createQry.lastError().text();
        } else {
            qDebug() << "Table" << tableName << "copied.";
        }
    }


    // Detaching database
    qry.prepare("DETACH DATABASE tempDb");
    if(!qry.exec())
    {
        qDebug() << "Error in: " << Q_FUNC_INFO;
        qDebug() << "Can`t detach temporary database, query error: " <<  qry.lastError().text();
        return false;
    }

    QFile file(encryptedDatabase);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Error in: " << Q_FUNC_INFO;
        qDebug("Can't open encrypted database");
        return false;
    }

    // This function checks lengh of master-key, it lengh must be 32 bytes, if it less we need to append zeroes
    // Else removes extra bytes
    DatabaseLoader::checkMasterKey(key);

    QByteArray *data = new QByteArray(tmp.readAll());
    // Writing encrypted changes to encrypted file
    if(!file.write(*encryptData(data, key)))
    {
        qDebug() << "Error in: " << Q_FUNC_INFO;
        qDebug() << "Can't write encrypted data back to encrypted database";
        return false;
    }
    delete data;
    // Safe delete of temporary file
    if(!deleteTemporaryFile(tmp))
    {
        qDebug() << "Error in: " << Q_FUNC_INFO;
        qDebug() << "Can't delete temporary database";
        return false;
    }
    return true;
}

bool DatabaseLoader::createAndFillDatabase(const QString databasePath, QByteArray &key, QSqlDatabase *db)
{
    qDebug() << Q_FUNC_INFO;
    checkMasterKey(key);
    QTemporaryFile tmp;
    if(!tmp.open())
    {
        qDebug() << "Can't open temporary file";
        return false;
    }

    db->setDatabaseName(":memory:");

    if(!db->open())
    {
        qDebug() << "Can't open database";
    }

    QSqlQuery query(*db);

    query.prepare("ATTACH DATABASE :path as TempDb");
    query.bindValue(":path", tmp.fileName());
    if(!query.exec())
    {
        qDebug() << "Can't attach temporary database";
        return false;
    }

    query.prepare(R"(
    CREATE TABLE TempDb.General (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        Title TEXT,
        [User Name] TEXT,
        Password TEXT,
        URL TEXT,
        Notes TEXT,
        [Creation Time] TEXT,
        [Last Changed] TEXT
    )
    )");


    if(!query.exec())
    {
        qDebug() << "Can't add columns to table:" << query.lastQuery();
        qDebug() << "Query execution error:" << query.lastError().text();
        return false;
    }

    query.prepare(R"(
    CREATE TABLE TempDb.TablesSettings (
        [Table] TEXT,
        [Icon] TEXT NOT NULL DEFAULT 'key'
    );
    )");

    if(!query.exec())
    {
        qDebug() << "Can't add table with settings" << query.lastQuery();
        qDebug() << "Query execution error:" << query.lastError().text();
        return false;
    }

    query.exec(R"(
    INSERT INTO TablesSettings ([Table]) VALUES ('General')
    )");

    query.prepare("INSERT INTO TempDb.general (Title) VALUES ('test')");
    query.exec();
    query.exec("SELECT id FROM TempDb.general ORDER BY id DESC LIMIT 1");
    if (query.next())
        qDebug() << "Last inserted id:" << query.value(0).toInt();



    QByteArray *raw = new QByteArray(tmp.readAll());
    QFile file(databasePath);
    file.open(QIODevice::WriteOnly);
    *raw = *encryptData(raw, key);
    qDebug() << "Encrypted data size: " << raw->size();
    if(!file.write(*raw))
    {
        qDebug() << "Can't write encrypted raw data to database";
        delete raw;
        return false;
    }
    file.close();


    return true;
}
