#include "addtable.h"
#include "ui_addtable.h"

AddTable::AddTable(QWidget *parent, QSqlDatabase *db, std::vector<QString> *tables, QMap<QString, QIcon*> *icons)
    : QDialog(parent)
    , ui(new Ui::AddTable)
    , tables(tables)
    , db(db)
    , icons(icons)
{
    ui->setupUi(this);
    ui->nameEdit->setMaxLength(15);
    QRegularExpression rx(R"(^[a-zA-Zа-яА-ЯІіїЇ0-9_]+(\s[a-zA-Zа-яА-ЯІіїЇ0-9_]+)+$)");
    QValidator *validator = new QRegularExpressionValidator(rx, this);

    ui->nameEdit->setValidator(validator);

    if(db == nullptr || tables == nullptr)
    {
        QMessageBox msg;
        msg.setText(tr("Can't add new table, probably you didn't opened database"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        this->close();
    }
    for(QString el : iconNames)
    {
        ui->comboBox->addItem(*icons->value(el), "");
    }
}

AddTable::~AddTable()
{
    delete ui;
}


bool checkIfTableExists(const std::vector<QString> allTables, const QString newTable)
{
    for(int i = 0; i < allTables.size(); i++)
    {
        if(allTables[i] == newTable)
            return true;
    }
    return false;
}


void AddTable::on_addTableButton_clicked()
{
    qDebug() << Q_FUNC_INFO;

    if(ui->nameEdit->text().size() <= 0)
    {
        QMessageBox msg;
        msg.setText(tr("Field must be not empty"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }

    QSqlQuery qry(*db);

    QString tableName = ui->nameEdit->text();

    if(!checkIfTableExists(*tables, ui->nameEdit->text()))
    {
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

        QSqlQuery query(*db);
        if(!query.exec(command))
        {
            QMessageBox msg;
            msg.setText(tr("Can't create table\nQuery error: ") + query.lastError().text());
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
        }

        query.prepare("INSERT INTO TablesSettings ([Table], Icon) VALUES ('"+tableName+"', '"+iconNames[ui->comboBox->currentIndex()]+"')");

        if(!query.exec())
        {
            QMessageBox msg;
            msg.setText(tr("Can't create table\nQuery error: ") + query.lastError().text());
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
        }

        tables->push_back(ui->nameEdit->text());
        this->close();
    }else {
        QMessageBox msg;
        msg.setText(tr("Table with this name already exists\nTry another name"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
}

void AddTable::on_buttonCancel_clicked()
{
    this->close();
}
