#include "createdatabase.h"
#include "ui_createdatabase.h"

CreateDatabase::CreateDatabase(QWidget *parent, QByteArray *masterPassword)
    : QDialog(parent)
    , ui(new Ui::CreateDatabase)
    , key(masterPassword)
{
    ui->setupUi(this);
}

CreateDatabase::~CreateDatabase()
{
    delete ui;
}



void CreateDatabase::on_saveButton_clicked()
{
    if(true)
    {
        *key = ui->masterPassword->text().toUtf8();
        this->close();
    }
}


void CreateDatabase::on_masterPassword_textChanged(const QString &arg1)
{

}

