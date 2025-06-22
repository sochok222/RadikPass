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

EditTable::EditTable(QWidget *parent, QSqlDatabase *db, const QString tableName, QListWidget *listWidget, QString theme)
    : QDialog(parent)
    , ui(new Ui::EditTable)
    , db(db)
    , tableName(tableName)
    , listWidget(listWidget)
    , theme(theme)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Edit Table"));
    ui->nameEdit->setText(tableName);

    if(db == nullptr || !db->isOpen())
    {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Can`t open database"));
        msg.addButton(QMessageBox::Ok);
        msg.exec();
        this->close();
    }

    model = new QStandardItemModel;
    mapper = new QDataWidgetMapper;

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
    if(theme == "")
    {
        showMsgBox(tr("Can't load current theme"));
        QTimer::singleShot(0, this, SLOT(close()));
    }
    loadIcons();
}

EditTable::~EditTable()
{
    delete ui;
    delete mapper;
    delete model;
}

void EditTable::loadIcons()
{
    // Load system icons
    QVector<QString> sysIcons = {"entry", "game", "house", "money", "net", "office", "pc", "programming", "user", "key"};

    for(const QString &sysIcon : sysIcons)
    {
        QString icon = ":/icons/"+theme+"/resources/icons/"+theme+"/"+sysIcon+".png";
        ui->comboBox->addItem(QIcon(icon), "");
        model->appendRow(new QStandardItem(sysIcon));
    }

    // QDirIterator it(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation),{"*.png"}, QDir::Files);

    // while(it.hasNext())
    // {
    //     QString icon = it.next();

    //     ui->comboBox->addItem(icon, "");
    //     model->appendRow(new QStandardItem(icon));
    // }

    mapper->setModel(model);
    mapper->toFirst();
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), mapper, SLOT(setCurrentIndex(int)));
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

    QString changeName = "ALTER TABLE '"+tableName+"' RENAME TO '"+ui->nameEdit->text()+"'";
    QString restoreName = "ALTER TABLE '"+ui->nameEdit->text()+"' RENAME TO '"+tableName+"'";
    QString changeSettingName = "UPDATE TablesSettings SET [Table] = '"+ui->nameEdit->text()+"' WHERE [Table] = '"+tableName+"'";
    QString ico(mapper->model()->data(model->index(mapper->currentIndex(), 0)).toString());
    QString setNewIcon("UPDATE TablesSettings SET Icon = '"+ico+"' WHERE [Table] = '"+ui->nameEdit->text()+"'");

    if(ui->nameEdit->text().size() == 0)
    {
        showMsgBox(tr("Line must be not empty"));
        return;
    }

    if(isNameChanged)
    {
        if(!query.exec(changeName)) {
            QMessageBox msg;
            msg.setText(tr("Unable to change name of table, try to reopen database."));
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
            return;
        }
        if(!query.exec(changeSettingName)) {
            query.exec(restoreName);
            QMessageBox msg;
            msg.setText(tr("Unable to change name of table, try to reopen database."));
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
            return;
        }
    }

    if(isIconChanged)
    {
        if(!query.exec(setNewIcon))
        {
            QMessageBox msg;
            msg.setText(tr("Unable to change icon of table, try again."));
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
        }
    }

    listWidget->currentItem()->setText(ui->nameEdit->text());
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

void EditTable::on_buttonSave_clicked()
{
    saveChanges();
    this->accept();
}


void EditTable::on_buttonCancel_clicked()
{
    this->accept();
}

