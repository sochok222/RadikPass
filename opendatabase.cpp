#include "opendatabase.h"
#include "ui_opendatabase.h"

OpenDatabase::OpenDatabase(QWidget *parent, QByteArray *result)
    : QDialog(parent)
    , ui(new Ui::OpenDatabase)
{
    ui->setupUi(this);
    ui->okButton->setDefault(true);

    ui->databasePathLabel->setText(settings.value("Last").toString());
    masterPassword = result;
    QSettings setting("AlexRadik", "RadiPass");
    ui->okButton->setAutoDefault(true);
    this->setWindowTitle(tr("Open Database"));
}

OpenDatabase::~OpenDatabase()
{
    delete ui;
}



void OpenDatabase::on_okButton_clicked()
{
    if(ui->passwordLine->text().size() > 0)
    {
        *masterPassword = ui->passwordLine->text().toUtf8();
        this->close();
    }else {
        QMessageBox msg;
        msg.setText(tr("Password line must be not empty"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
}


void OpenDatabase::on_cancelButton_clicked()
{
    settings.setValue("Last", "");
    this->destroy(true,true);
}


