#include "edittable.h"
#include "ui_edittable.h"
#include <qsqlerror.h>

#include "dbmanager.h"

void EditTable::showMsgBox(const QString text)
{
    QMessageBox msg;
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}

EditTable::EditTable(QWidget *parent, QSqlDatabase *db, const QString tableName, QListWidget *listWidget)
    : QDialog(parent)
    , ui(new Ui::EditTable)
    , db(db)
    , tableName(tableName)
    , listWidget(listWidget)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Edit Table"));
    ui->nameEdit->setText(tableName);
    ui->comboBox->setCurrentIndex(iconNames.indexOf("bitcoin"));

    ui->nameEdit->setMaxLength(15);
    QRegularExpression rx(R"(^[a-zA-Zа-яА-ЯІіїЇ0-9_]+(\s[a-zA-Zа-яА-ЯІіїЇ0-9_]+)+$)");
    QValidator *validator = new QRegularExpressionValidator(rx, this);

    ui->nameEdit->setValidator(validator);

    QTimer::singleShot(0, this, SLOT(setConnections()));

    if(listWidget == nullptr)
    {
        showMsgBox(tr("Can't load listWidget"));
        QTimer::singleShot(0, this, SLOT(close()));
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
        msg.setText(tr("Field must be not empty"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }

    QString changeName("ALTER TABLE '"+tableName+"' RENAME TO '"+ui->nameEdit->text()+"'");
    QString changeSettingsName("UPDATE TablesSettings SET [Table] = '"+ui->nameEdit->text()+"' WHERE [Table] = '"+tableName+"'");
    QString setNewIcon("UPDATE TablesSettings SET Icon = '"+iconNames[ui->comboBox->currentIndex()]+"' WHERE [Table] = '"+ui->nameEdit->text()+"'");

    DbManager::createBackup(db);

    if(ui->nameEdit->text().size() == 0)
    {
        showMsgBox(tr("Line must be not empty"));
        return;
    }

    if(isNameChanged)
    {
        if(!query.exec(changeName))
        {
            DbManager::loadBackup(db);
            DbManager::deleteBackup(db);
            return;
        }

        if(!query.exec(changeSettingsName))
        {
            DbManager::loadBackup(db);
            DbManager::deleteBackup(db);
            return;
        }
    }

    if(isIconChanged)
    {
        if(!query.exec(setNewIcon))
        {
            DbManager::loadBackup(db);
            DbManager::deleteBackup(db);
            return;
        }
    }

    listWidget->currentItem()->setText(ui->nameEdit->text());
    DbManager::deleteBackup(db);
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

