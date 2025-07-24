#include "DbManager.h"
#include <qsqldriver.h>
#include <QList>
#include <QModelIndex>
#include <QTableWidgetItem>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTranslator>
#include <qsqlrecord.h>

DbManager::DbManager() {}

void DbManager::showMsgBox(const QString &text) {
    QMessageBox msg; // Creating message box
    msg.setText(text); // Setting text
    msg.setStandardButtons(QMessageBox::Ok); // Adding "Ok" button

    msg.exec(); // Showing
}


void DbManager::search(const QString &text, QSqlDatabase *db, QListWidget *tables) {
    qInfo() << Q_FUNC_INFO;

    QSqlQuery query(*db);
    // Creating table where search results will be stored
    query.exec(R"(
    CREATE TABLE IF NOT EXISTS Search (
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

    query.exec("DELETE FROM Search"); // Clearing Search table

    QSqlQuery copyQuery(*db); // This QSqlQuery will find entries
    for (int i = 0; i < tables->count(); i++) {
        QString toExec = QString("INSERT INTO Search SELECT * FROM [%1] WHERE Title LIKE '%2%' OR [User Name] LIKE '%2%' OR URL LIKE '%2%' OR Notes LIKE '%2%'").arg(tables->item(i)->text()).arg(text);

        qDebug() << "Query: " << toExec;

        if (!copyQuery.exec(toExec)) {
            qCritical() << "Unable to execute: " << query.lastQuery() <<", error: " << query.lastError();
        }
    }
}

bool DbManager::getRowId(QSqlTableModel *model, QTableView *tableView, QSqlDatabase *db, int &rowId) {
    qInfo() << Q_FUNC_INFO;

    QSqlQuery query(*db);

    // Creating a query that will find id of row and executing it
    QString getId = QString("SELECT id FROM %1 LIMIT 1 OFFSET %2").arg(model->tableName()).arg(tableView->currentIndex().row());
    if (!query.exec(getId)) {
        qCritical() << "Unable to execute query, error: " << query.lastError();
        return false; // If unable to execute needs to return false
    }

    query.next(); // Retrieving found value
    rowId = query.value(0).toInt(); // Putting value to rowId

    return true;
}

bool DbManager::deleteTable(QSqlDatabase *db, const QString tableName) {
    qInfo() << Q_FUNC_INFO;

    QSqlQuery query(*db);

    qInfo() << "Preparing 'DELETE FROM TablesSettings' query ...";

    // Preparing and executing a query that will delete the row with table and icon from the TablesSettings
    query.prepare("DELETE FROM TablesSettings WHERE [Table] = :name");
    query.bindValue(":name", tableName);
    if (!query.exec()) {
        qCritical() << "Query execution failed, error: " << query.lastError();
        return false;
    } qDebug() << "Successfully executed";

    qInfo() << "Preparing 'DROP TABLE' query...";

    // Preparingand executing a query that will delete the table from the database
    QString command = QString("DROP TABLE [%1]").arg(tableName);
    if (!query.exec(command)) {
        qCritical() << "Query execution failed, error: " << query.lastError();
        query.exec("INSERT INTO TablesSettings ([Table]) VALUES ('"+tableName+"')");
        return false;
    }qDebug() << "Successfully executed";

    return true;
}


QByteArray* DbManager::encryptData(QByteArray *data, QByteArray &gotKey) {
    qInfo() << Q_FUNC_INFO;

    // Creating iv with size of 16 bytes and filling it with random bytes
    QByteArray *iv = new QByteArray(16, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(iv->data()), iv->size());

    QByteArray key(32, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(key.data()), key.size());
    if(!PKCS5_PBKDF2_HMAC_SHA1(reinterpret_cast<char*>(gotKey.data()), gotKey.size(), 0, 0, 1000000, 32, reinterpret_cast<unsigned char*>(key.data()))) {
        qDebug() << "Key derivation func error";
    }

    // Allocating cipher context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx){
        qCritical() << "Encryption failed, cipher context allocation failed.";
        return nullptr;
    }

    // Setting up cipher context
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                           reinterpret_cast<const unsigned char*>(key.data()),
                           reinterpret_cast<const unsigned char*>(iv->data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        qCritical() << "Encryption failed, cant setup cipher context";
        return nullptr;
    }

    // Creating byte array in which encrypted datbase will be written
    QByteArray *encrypted = new QByteArray;
    encrypted->resize(data->size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));

    // Encrypting
    int len = 0, totalLen = 0;
    if (EVP_EncryptUpdate(ctx,
                          reinterpret_cast<unsigned char*>(encrypted->data()), &len,
                          reinterpret_cast<const unsigned char*>(data->data()), data->size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        qCritical() << "Encryption failed, unable to encrypt data.";
        return nullptr;
    }
    totalLen += len;

    // Encrypting final data
    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(encrypted->data()) + totalLen, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        qCritical() << "Encryption failed, unable to encrypt final data";
        return nullptr;
    }
    totalLen += len;
    encrypted->resize(totalLen);
    EVP_CIPHER_CTX_free(ctx);

    qInfo() << "Successfully encrypted";

    // Returning encrypted data
    QByteArray *ret = new QByteArray;
    ret->append(*iv + *encrypted);
    delete iv;
    delete encrypted;
    return ret;
}

QByteArray* DbManager::decryptData(QByteArray *encryptedData, QByteArray &gotKey) {
    qInfo() << Q_FUNC_INFO;

    QByteArray key(32, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(key.data()), key.size());
    if(!PKCS5_PBKDF2_HMAC_SHA1(reinterpret_cast<char*>(gotKey.data()), gotKey.size(), 0, 0, 1000000, 32, reinterpret_cast<unsigned char*>(key.data()))) {
        qDebug() << "Key derivation func error";
    }

    // Reading iv and cipher data from encrypted byte array
    QByteArray iv = encryptedData->left(16);
    QByteArray cipherText = encryptedData->mid(16);

    // Allocating ciper context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx){
        qCritical() << "Decryption failed, can't allocate cipher context";
        return nullptr;
    }

    // Initializing decryption
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                           reinterpret_cast<const unsigned char*>(key.data()),
                           reinterpret_cast<const unsigned char*>(iv.data())) != 1) {
        qCritical() << "Decryption failed, can't init decryption";
        EVP_CIPHER_CTX_free(ctx);
        return nullptr;
    }

    // Initializating and resizing byte array for decrypted data
    QByteArray *decrypted = new QByteArray;
    decrypted->resize(cipherText.size());

    // Creating decrypted data buffer
    int len = 0, totalLen = 0;
    if (EVP_DecryptUpdate(ctx,
                          reinterpret_cast<unsigned char*>(decrypted->data()), &len,
                          reinterpret_cast<const unsigned char*>(cipherText.data()), cipherText.size()) != 1) {
        qCritical() << "Decryption failed, can't create the buffer for decrypted data";
        EVP_CIPHER_CTX_free(ctx);
        return nullptr;
    }
    totalLen += len;

    // Decryption
    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(decrypted->data()) + totalLen, &len) != 1) {
        qCritical() << "Decrypt failed, final decryption returned error";
        EVP_CIPHER_CTX_free(ctx);
        return nullptr;
    }
    totalLen += len;
    decrypted->resize(totalLen);
    EVP_CIPHER_CTX_free(ctx);

    return decrypted; // Returning decrypted byte array
}

bool DbManager::loadTemporaryDatabase(QSqlDatabase &db, QString &path, std::vector<QString> &tables) {
    qInfo() << Q_FUNC_INFO;

    // Connecting database to RAM
    db.setDatabaseName(":memory:");

    // Returning false if can`t open datatbase
    if (!db.open()) {
        qCritical() << "Can't open database";
        return false;
    }

    QSqlQuery query(db);

    // Executing query that will attach the temporary database as tempDb
    query.prepare("ATTACH DATABASE :path as tempDb");
    query.bindValue(":path", path); // Binding path as :path
    if (!query.exec()) {
        qCritical() << "Can`t attach temporary database, query error: " << query.lastError().text();
        return false;
    }

    // Copying 'create table' queries and names of that tables
    query.prepare("SELECT name, sql FROM tempDb.sqlite_master WHERE type='table' AND name != 'sqlite_sequence' AND name != 'Search'");
    if (!query.exec()) {
        qDebug() << "Can't execute query that will copy queries and tables, error: " << query.lastError().text();
        return false;
    }

    tables.clear(); // Clearing list of tables

    // While query has values
    while (query.next()) {
        QString tableName = query.value(0).toString(); // Name of table
        QString createSQL = query.value(1).toString(); // 'create table' sql query

        qDebug() << "Creating table:" << tableName;

        // Creating table in :memory: database
        QSqlQuery createQuery(db);
        if (!createQuery.exec(createSQL)) {
            qCritical() << "Create table failed:" << createQuery.lastError().text();
            showMsgBox("Can't copy table: " + tableName + ".\nPlease try again.");
            continue;
        }

        // Copying data from the temporary datbase to the :memory: database
        QString copySQL = QString("INSERT INTO [%1] SELECT * FROM tempDb.[%1]").arg(tableName);
        if (!createQuery.exec(copySQL)) {
            qCritical() << "Insert failed:" << createQuery.lastError().text();
            showMsgBox("Can't copy content of table: " + tableName + ".\nPlease try again.");
        } else {
            // The "TablesSetting" is table that is not available to user
            if (tableName != "TablesSettings")tables.push_back(tableName);
        }
    }

    // Detaching database
    query.prepare("DETACH DATABASE tempDb");
    if (!query.exec()) {
        qDebug() << "Can`t detach temporary database, query error: " <<  query.lastError().text();
        return false;
    }

    return true;
}

bool DbManager::checkMasterKey(QByteArray &masterKey) {
    qDebug() << Q_FUNC_INFO;

    if (masterKey.size() < 32) { // If key size is less than 32 bytes needs to append zeroes
        qInfo() << "Appending " << 32-masterKey.size() << " zeroes to key...";
        QByteArray toAppend(32-masterKey.size(), '0');
        masterKey.append(toAppend);
        return false;
    }else if (masterKey.size() > 32) { // Else needs to remove redundant bytes
        qInfo() << "Removing redundant bytes from key...";
        masterKey.remove(32, masterKey.size());
        return false;
    }

    return true;
}

template<typename T>
bool DbManager::deleteTemporaryFile(T &file) {
    qInfo() << Q_FUNC_INFO;

    // Content of temporary file will be overwriten by this array
    QByteArray *array = new QByteArray(file.size(), 0);

    // Openning file
    if (!file.open()) {
        qCritical() << "Can`t open file";
        delete array;
        return false;
    }

    // Writing zeroes to file
    if (!file.write(*array)) {
        qCritical() << "Can't fill file with zeroes";
        file.close();
        file.remove();
        delete array;
        return false;
    }

    file.close();

    // Removing file
    if (!file.remove()) {
        qCritical() << "Can`t delete file";
        delete array;
        return false;
    }

    delete array;
    return true;
}

bool DbManager::loadDb(const QString encryptedDatabase, QByteArray &key, QSqlDatabase *db, std::vector<QString> &tables)
{
    qInfo() << Q_FUNC_INFO;

    // Encrypted database will be open via QFile
    QFile encrypted(encryptedDatabase);

    // Opening file
    encrypted.open(QIODevice::ReadWrite);

    // Creating the temporary file
    QTemporaryFile temp;
    temp.setAutoRemove(false);
    if (!temp.open()) {
        qCritical() << "Can`t open temporary file";
        return false;
    }

    encrypted.seek(0); // Moving the cursor to the start of file

    QByteArray *encData = new QByteArray(encrypted.readAll()); // Reading the content of the encrypted file

    encData = decryptData(encData, key); // Decrypting encrypted data

    if (encData == nullptr || encData->size() == 0) {
        return false;
        qCritical() << "Decrypted data is nullptr or has size of zero";
    }

    // Writing decrypted array to the temporary file
    if (!temp.write(*encData)) {
        qCritical() << "Can`t write decrypted data to temporary file";
        return false;
    }
    delete encData; // Deleting encData
    temp.close(); // Closing file

    // Copying database path to QString
    QString databasePath(temp.fileName());

    // Loading temporary database to :memory: database
    if (!loadTemporaryDatabase(*db, databasePath, tables)) {
        qCritical() << "Can`t load temporary database to :memory:";
        return false;
    }

    // Deleting temporary file
    if (!deleteTemporaryFile(temp)) {
        qCritical() << "Can`t delete temporary database";
        return false;
    }

    return true;
}

bool DbManager::uploadDb(const QString encryptedDatabase, QByteArray &key, QSqlDatabase *db) {
    qInfo() << Q_FUNC_INFO;

    // Creating temporary file
    QTemporaryFile tmp;
    tmp.setAutoRemove(false);
    if (!tmp.open()) {
        qCritical() << "Can`t open temporary file to write new database";
        return false;
    }

    QSqlQuery query(*db);

    // Attaching temporary database
    query.prepare("ATTACH DATABASE :path as tempDb");
    query.bindValue(":path", tmp.fileName());
    if (!query.exec())
    {
        qCritical() << "Can`t attach temporary database, query error: " << query.lastError().text();
        return false;
    }

    // Selecting tables
    query.prepare("SELECT name, sql FROM main.sqlite_master WHERE type='table' AND name != 'sqlite_sequence'");
    if (!query.exec()) {
        qCritical() << "Can't execute the query that selects tables: " << query.lastError().text();
        return false;
    }

    QSqlQuery createQuery(*db); // query that will copy tables to TempDb

    while (query.next()) {
        QString tableName = query.value(0).toString(); // Copying name of table
        QString createSQL = query.value(1).toString(); // Copying 'CREATE TABLE' sql query
        createSQL.replace("CREATE TABLE ", "CREATE TABLE TempDb."); // Now query will create tables in attached TempDb

        // Trying to copy table to temporary database
        if (!createQuery.exec(createSQL)) {
            showMsgBox(QObject::tr("Can't copy table ") + tableName);
            qCritical() << "'CREATE TABLE' failed:" << createQuery.lastError().text() << "\n Name of table: " << tableName;
            continue;
        }

        // Copying content to temporary database
        QString copySQL = QString("INSERT INTO tempDb.[%1] SELECT * FROM [%1]").arg(tableName);
        if (!createQuery.exec(copySQL)) {
            qCritical() << "Insert failed:" << createQuery.lastError().text();
        } else {
            qCritical() << "Table" << tableName << "copied.";
        }
    }


    // Detaching database
    query.prepare("DETACH DATABASE tempDb");
    if (!query.exec())
    {
        qDebug() << "Error in: " << Q_FUNC_INFO;
        qDebug() << "Can`t detach temporary database, query error: " <<  query.lastError().text();
        return false;
    }

    // Opening file
    QFile file(encryptedDatabase);
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical() << "Can't open encrypted database";
        showMsgBox(QObject::tr("Unable to open encrypted database\nTry again please"));
        return false;
    }

    QByteArray *data = new QByteArray(tmp.readAll()); // Reading data from temporary database

    // Writing encrypted data to encrypted database
    if (!file.write(*encryptData(data, key))) {
        qCritical() << "Can't write encrypted data to encrypted database";
        showMsgBox("Unable to write encrypted data\nTry again please");
        return false;
    }
    delete data;

    // Deleting temporary file safely
    if (!deleteTemporaryFile(tmp)) {
        qCritical() << "Can't delete temporary database";
        showMsgBox("Can`t delete temporary database\nTry to delete it manually, path: \n" + tmp.fileName());
        return false;
    }

    return true;
}

bool DbManager::createAndFillDatabase(const QString databasePath, QByteArray &key, QSqlDatabase *db)
{
    qDebug() << Q_FUNC_INFO;

    checkMasterKey(key); // If key length is less than 32 this function will append zeroes to key

    // Creating a temporary file and trying to open it
    QTemporaryFile tmp;
    if (!tmp.open())
    {
        showMsgBox("Can't open temporary file");
        return false;
    }

    db->setDatabaseName(":memory:");

    if (!db->open())
    {
        qDebug() << "Can't open database";
    }

    QSqlQuery query(*db);

    query.prepare("ATTACH DATABASE :path as TempDb");
    query.bindValue(":path", tmp.fileName());
    if (!query.exec())
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


    if (!query.exec())
    {
        qDebug() << "Can't add columns to table:" << query.lastQuery();
        qDebug() << "Query execution error:" << query.lastError().text();
        return false;
    }

    query.prepare(R"(
    CREATE TABLE TempDb.TablesSettings (
        [Table] TEXT,
        [Icon] TEXT NOT NULL DEFAULT 'entry'
    );
    )");

    if (!query.exec())
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
    if (!file.write(*raw))
    {
        qDebug() << "Can't write encrypted raw data to database";
        delete raw;
        return false;
    }
    file.close();


    return true;
}
