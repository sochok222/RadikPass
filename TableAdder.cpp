#include "TableAdder.h"
#include "ui_TableAdder.h"

TableAdder::TableAdder(QWidget *parent, QSqlDatabase *db, std::vector<QString> *tables, QString theme)
    : QDialog(parent)
    , ui(new Ui::TableAdder)
    , tables(tables)
    , db(db)
    , theme(theme)
{
    ui->setupUi(this);

    // Setting name of window
    this->setWindowTitle(tr("Add Table"));

    // Maximum length is 15
    ui->nameEdit->setMaxLength(15);

    // Limmitation of user input by QRegularExpression
    QRegularExpression rx(R"(^[a-zA-Zа-яА-ЯІіїЇ0-9_]+(\s[a-zA-Zа-яА-ЯІіїЇ0-9_]+)+$)");
    // Setting expression to validator
    validator = new QRegularExpressionValidator(rx, this);
    // Applyint validator to nameEdit
    ui->nameEdit->setValidator(validator);

    // Check if database is opened
    if(db == nullptr || !db->isOpen() || tables == nullptr)
    {
        QMessageBox msg;
        msg.setText(tr("Can't add new table, probably you didn't opened database"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        this->close();
    }

    // Loading icons to QComboBox
    loadIcons();
}


TableAdder::~TableAdder()
{
    delete ui;
    delete validator;
}

void TableAdder::loadIcons()
{
    // Vector with names of icons
    QVector<QString> sysIcons = {"entry", "game", "house", "money", "net", "office", "pc", "programming", "user", "key"};

    // Iterating through vector to load icons to comboBox and QStandardItemModel.
    for(const QString &sysIcon : sysIcons)
    {
        ui->comboBox->addItem(QIcon(":/icons/"+theme+"/resources/icons/"+theme+"/"+sysIcon+".png"), "");
        model->appendRow(new QStandardItem(sysIcon));
    }

    // TODO: in future implement user icons loading.
    // QDirIterator it(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation),{"*.png"}, QDir::Files);

    // while(it.hasNext())
    // {
    //     QString icon = it.next();

    //     ui->comboBox->addItem(icon, "");
    //     model->appendRow(new QStandardItem(icon));
    // }

    // Mapping comboBox to QStandardModel.
    mapper->setModel(model);
    // Setting mapper index to first index.
    mapper->toFirst();
    // Synchronizing mapper to comboBox to know which icon user selected.
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), mapper, SLOT(setCurrentIndex(int)));
}


TableAdder::rtrnCodes TableAdder::checkIfTableExists(const QString newTable)
{
    // Checking if name of new table isn't name that can't be used.
    if(newTable == "TablesSettings" || newTable == "sqlite_master" || newTable == "main")
        return rtrnCodes::cantBeUsed;

    QSqlQuery query(*db); // New query to execute database commands.

    // Selecting all names from sqlite_master to search if table exists.
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=:name");
    // Binding value of new table.
    query.bindValue(":name", newTable);
    // Executing
    query.exec();

    // If at least one value exists returning that table already existing
    while(query.next())
        return rtrnCodes::exists;
    // else returning that table is not existing
    return rtrnCodes::notExists;
}


void TableAdder::on_addTableButton_clicked()
{
    qDebug() << Q_FUNC_INFO;

    // If nameEdit is empty needs to show message to user that field must be not empty.
    if(ui->nameEdit->text().size() <= 0)
    {
        QMessageBox msg;
        msg.setText(tr("Field must be not empty"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }

    // QSqlQuery to execute sql queries.
    QSqlQuery query(*db);

    // Name of table that user wants to create.
    QString tableName = ui->nameEdit->text();


    rtrnCodes ifExists = checkIfTableExists(tableName);

    // Checking if database is already exists.
    if(ifExists == rtrnCodes::notExists)
    {
        // Statement that will create table.
        QString command = QString(R"(
        CREATE TABLE [%1] (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            Title TEXT,
            [User Name] TEXT,
            Password TEXT,
            URL TEXT,
            Notes TEXT,
            [Creation Time] TEXT,
            [Last Changed] TEXT
        );
        )").arg(tableName);

        // If query wasn't executed needs to show error.
        if(!query.exec(command))
        {
            QMessageBox msg;
            msg.setText(tr("Can't create table\nQuery error: ") + query.lastError().text());
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
        }

        // Receiving icon that user selected.
        QString ico(mapper->model()->data(model->index(mapper->currentIndex(), 0)).toString());
        // Prepairing query that will set icon in TablesSettings.
        query.prepare("INSERT INTO TablesSettings ([Table], Icon) VALUES ('"+tableName+"', '"+ico+"')");

        // If query wasn't executed needs to show error and delete table.
        if(!query.exec())
        {
            // Deleting table.
            query.exec(QString("DROP TABLE [%1]").arg(tableName));
            // Showing error.
            QMessageBox msg;
            msg.setText(tr("Can't create table\nQuery error: ") + query.lastError().text());
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
        }

        // Appending name of table that was created to vector.
        tables->push_back(ui->nameEdit->text());

        // Closing window.
        this->close();
    }else if (ifExists == rtrnCodes::exists){ // Showing error if table is already exists.
        QMessageBox msg;
        msg.setText(tr("Table with this name already exists\nTry another name"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }else if(ifExists == rtrnCodes::cantBeUsed) // Showing error if table can't be used.
    {
        QMessageBox msg;
        msg.setText(tr("Can't create table with this name.\nTry another name."));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
}

void TableAdder::on_buttonCancel_clicked()
{
    // Closing window.
    this->close();
}
