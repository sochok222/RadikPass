#include "createdatabase.h"
#include "ui_createdatabase.h"

CreateDatabase::CreateDatabase(QWidget *parent, QByteArray *masterPassword, const QString path)
    : QDialog(parent)
    , ui(new Ui::CreateDatabase)
    , key(masterPassword)
{
    ui->setupUi(this);

    if(path != "")
    {
        ui->labelPath->setText(path);
    }else {
        QMessageBox msg;
        msg.setText("Can't load path");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        this->close();
    }
}

CreateDatabase::~CreateDatabase()
{
    delete ui;
}



void CreateDatabase::on_saveButton_clicked()
{
    if(true)
    {
        *key = ui->linePassword->text().toUtf8();
        this->close();
    }
}


void CreateDatabase::on_masterPassword_textChanged(const QString &arg1)
{

}


void CreateDatabase::on_buttonIsAsterisks_clicked()
{
    if(ui->checkBoxIsAsterisks->isChecked())
    {
        ui->checkBoxIsAsterisks->setChecked(false);
        ui->lineRepeat->setEnabled(false);
        ui->labelRepeat->setEnabled(false);
    }else
    {
        ui->checkBoxIsAsterisks->setChecked(true);
        ui->lineRepeat->setDisabled(false);
        ui->labelRepeat->setDisabled(false);
    }

}

