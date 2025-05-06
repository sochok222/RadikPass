#include "edittable.h"
#include "ui_edittable.h"
#include <qsqlerror.h>

#include "databaseloader.h"

void EditTable::showMsgBox(const QString text)
{
    QMessageBox msg;
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}

EditTable::EditTable(QWidget *parent, QSqlDatabase *db, QMap<QString, QIcon *> *icons, const QString tableName, QListWidget *listWidget)
    : QDialog(parent)
    , ui(new Ui::EditTable)
    , db(db)
    , icons(icons)
    , tableName(tableName)
    , listWidget(listWidget)
{
    ui->setupUi(this);
    ui->nameEdit->setText(tableName);
    ui->comboBox->setCurrentIndex(iconNames.indexOf("bitcoin"));

    ui->nameEdit->setMaxLength(15);
    QRegularExpression rx(R"(^[a-zA-Z_]+(\s[a-zA-Z_]+)+$)");
    QValidator *validator = new QRegularExpressionValidator(rx, this);

    ui->nameEdit->setValidator(validator);

    QTimer::singleShot(0, this, SLOT(setConnections()));

    if(listWidget == nullptr)
    {
        showMsgBox("Can't load listWidget");
        QTimer::singleShot(0, this, SLOT(close())); // TODO: this don't close the window
    }

    if(icons == nullptr)
    {
        showMsgBox("Can't find icons");
        QTimer::singleShot(0, this, SLOT(close())); // TODO: this don't close the window
    }
    for(QString el : iconNames)
    {
        ui->comboBox->addItem(*icons->value(el), "");
    }
}

EditTable::~EditTable()
{
    delete ui;
}

void EditTable::setConnections()
{
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), SLOT(iconChanged()));
    connect(ui->nameEdit, SIGNAL(textChanged(QString)), SLOT(textChanged()));
}

void EditTable::saveChanges()
{
    QSqlQuery query(*db);

    if(ui->nameEdit->text().size() <= 0)
    {
        QMessageBox msg;
        msg.setText("Field must be not empty");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }

    QString changeName("ALTER TABLE '"+tableName+"' RENAME TO '"+ui->nameEdit->text()+"'");
    QString changeSettingsName("UPDATE TablesSettings SET [Table] = '"+ui->nameEdit->text()+"' WHERE [Table] = '"+tableName+"'");
    QString setNewIcon("UPDATE TablesSettings SET Icon = '"+iconNames[ui->comboBox->currentIndex()]+"' WHERE [Table] = '"+ui->nameEdit->text()+"'");

    DatabaseLoader::createBackup(db);

    if(ui->nameEdit->text().size() == 0)
    {
        showMsgBox("Line must be not null!");
        return;
    }

    if(isNameChanged)
    {
        if(!query.exec(changeName))
        {
            DatabaseLoader::loadBackup(db);
            DatabaseLoader::deleteBackup(db);
            return;
        }

        if(!query.exec(changeSettingsName))
        {
            DatabaseLoader::loadBackup(db);
            DatabaseLoader::deleteBackup(db);
            return;
        }
    }

    if(isIconChanged)
    {
        if(!query.exec(setNewIcon))
        {
            DatabaseLoader::loadBackup(db);
            DatabaseLoader::deleteBackup(db);
            return;
        }
    }

    listWidget->currentItem()->setText(ui->nameEdit->text());
    DatabaseLoader::deleteBackup(db);
}


void EditTable::closeEvent(QCloseEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    if(isIconChanged || isNameChanged)
    {
        QMessageBox::StandardButton question = QMessageBox::question(
            this, "RadiPass", "Save changes?",
            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes); // Creates MessageBox with buttons
        if(question == QMessageBox::Yes)
        {
            saveChanges();
            event->accept();
        }else if(question == QMessageBox::No)
        {
            event->accept();
        }else if(question == QMessageBox::Cancel)
        {
            event->ignore();
        }
    }
    event->accept();
}

void EditTable::on_addTableButton_clicked()
{
    saveChanges();
    this->accept();
}


void EditTable::on_buttonCancel_clicked()
{
    this->accept();
}

