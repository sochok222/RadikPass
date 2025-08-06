#include "TableAdder.h"
#include "ui_TableAdder.h"
#include <qtimer.h>


TableAdder::TableAdder(QWidget *parent, QSqlDatabase *db, QVector<QString> *tables, Theme theme)
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
    ui->line_name->setMaxLength(15);

    // Creating egular expression for database name
    QRegularExpression rx(R"(^[a-zA-Zа-яА-ЯІіїЇ0-9_]+(\s[a-zA-Zа-яА-ЯІіїЇ0-9_]+)+$)");
    validator = new QRegularExpressionValidator(rx, this);
    ui->line_name->setValidator(validator);

    // Check if database is opened
    if (db == nullptr || !db->isOpen() || tables == nullptr) {
        showMsgBox(tr("Can't add new table, probably you didn't opened database"));
        QTimer::singleShot(0, this, SLOT(close()));
    }

    // Loading icons to QComboBox
    loadIcons();
}


TableAdder::~TableAdder() {
    delete ui;
    delete validator;
}

void TableAdder::showMsgBox(const QString &text) {
    QMessageBox msg;
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}

void TableAdder::loadIcons() {
    QVector<Icon> icons = {Icon::Entry, Icon::Game, Icon::House, Icon::Money, Icon::Office, Icon::Pc, Icon::Programming, Icon::User, Icon::Key};
    for(Icon &ico : icons) {
        ui->comboBox_icon->addItem(QIcon(IconLoader::getIcon(ico, theme)), "");
        model->appendRow(new QStandardItem(IconLoader::getIconName(ico)));
    }

    // Mapping comboBox to QStandardModel.
    mapper->setModel(model);
    // Setting mapper index to first index.
    mapper->toFirst();
    // Synchronizing mapper to comboBox to know which icon user selected.
    connect(ui->comboBox_icon, SIGNAL(currentIndexChanged(int)), mapper, SLOT(setCurrentIndex(int)));
}


TableAdder::rtrnCodes TableAdder::checkIfTableExists(const QString newTable) {
    // Checking if name of new table isn't name that can't be used.
    if (newTable == "TablesSettings" || newTable == "sqlite_master" || newTable == "main")
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


void TableAdder::on_addTableButton_clicked() {
    qDebug() << Q_FUNC_INFO;

    // If nameEdit is empty needs to show message to user that field must be not empty.
    if (ui->line_name->text().size() <= 0) {
        showMsgBox(tr("Name field must be not empty"));
        return;
    }

    // QSqlQuery to execute sql queries.
    QSqlQuery query(*db);

    // Name of table that user wants to create.
    QString tableName = ui->line_name->text();


    rtrnCodes ifExists = checkIfTableExists(tableName);

    // Checking if database is already exists.
    if (ifExists == rtrnCodes::notExists) {
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
        if (!query.exec(command)) {
            showMsgBox(tr("Can't create table\nQuery error: ") + query.lastError().text());
        }

        // Receiving icon that user selected.
        QString ico(mapper->model()->data(model->index(mapper->currentIndex(), 0)).toString());
        // Prepairing query that will set icon in TablesSettings.
        query.prepare("INSERT INTO TablesSettings ([Table], Icon) VALUES ('"+tableName+"', '"+ico+"')");

        // If query wasn't executed needs to show error and delete table.
        if (!query.exec()) {
            query.exec(QString("DROP TABLE [%1]").arg(tableName)); // Deleting table.
            showMsgBox(tr("Can't create table\nQuery error: ") + query.lastError().text());
        }

        // Appending name of table that was created to vector.
        tables->push_back(ui->line_name->text());
        this->close();
    } else if (ifExists == rtrnCodes::exists) { // Showing error if table is already exists.
        showMsgBox(tr("Table with this name already exists\nTry another name"));
    } else if (ifExists == rtrnCodes::cantBeUsed) {// Showing error if table can't be used.
        showMsgBox(tr("Can't create table with this name.\nTry another name."));
    }
}

void TableAdder::on_buttonCancel_clicked() {
    this->close();
}
