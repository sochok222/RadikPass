#include "createdatabase.h"
#include "ui_createdatabase.h"

CreateDatabase::CreateDatabase(QWidget *parent, QByteArray *masterPassword, const QString path)
    : QDialog(parent)
    , ui(new Ui::CreateDatabase)
    , key(masterPassword)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Create Database"));

    if(path != "")
    {
        ui->labelPath->setText(path);
    }else {
        QMessageBox msg;
        msg.setText(tr("Can't load path"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        this->close();
    }
    ui->linePassword->setMaxLength(50);
    ui->lineRepeat->setMaxLength(50);

    ui->checkBoxIsAsterisks->setToolTip(tr("Show/Hide password using asterisks"));
    ui->linePassword->setToolTip(tr("Password can contain letters, digits and special symbols only"));
    ui->labelRepeat->setToolTip(tr("Password quality for now is based on lenght"));


    ui->checkBoxIsAsterisks->setChecked(true);

    ui->linePassword->setEchoMode(QLineEdit::Password);
    ui->lineRepeat->setEchoMode(QLineEdit::Password);

    QRegularExpression rx(R"(^[a-zA-Zа-яА-Я0-9!@#$&().+,/-]*$)");
    validator = new QRegularExpressionValidator(rx, this);

    ui->linePassword->setValidator(validator);
}

CreateDatabase::~CreateDatabase()
{
    delete ui;
    delete validator;
}


void CreateDatabase::on_saveButton_clicked()
{
    if(true)
    {
        *key = ui->linePassword->text().toUtf8();
        this->close();
    }
}

void CreateDatabase::on_linePassword_textChanged(const QString &arg1)
{
    switch(ui->linePassword->text().size())
    {
    case 1:
        ui->labelPasswordQuality->setText(tr("Weak"));
        break;
    case 8:
        ui->labelPasswordQuality->setText(tr("Normal"));
        break;
    case 16:
        ui->labelPasswordQuality->setText(tr("Strong"));
        break;
    case 25:
        ui->labelPasswordQuality->setText(tr("Very Strong"));
        break;
    }
}

void CreateDatabase::on_lineRepeat_textChanged(const QString &arg1)
{
    if(isPassHidden)
    {
        if(arg1!=ui->linePassword->text() && ui->lineRepeat->text().size() > 0)
        {
            QPalette lineRepeatPalette;
            lineRepeatPalette.setColor(QPalette::Base, QColor(235,135,135));
            ui->lineRepeat->setPalette(lineRepeatPalette);
        }else
        {
            QPalette lineRepeatPalette;
            ui->lineRepeat->setPalette(lineRepeatPalette);
        }
    }
}

void CreateDatabase::on_checkBoxIsAsterisks_checkStateChanged(const Qt::CheckState &arg1)
{
    switch(arg1)
    {
        case Qt::Checked:
        {
            ui->lineRepeat->setEchoMode(QLineEdit::Password);
            ui->lineRepeat->setDisabled(false);
            ui->labelRepeat->setDisabled(false);
            isPassHidden = true;
            break;
        }
        case Qt::Unchecked:
        {
            ui->linePassword->setEchoMode(QLineEdit::Normal);
            ui->lineRepeat->clear();
            ui->lineRepeat->setEnabled(false);
            ui->labelRepeat->setEnabled(false);
            isPassHidden = false;
            break;
        }
        case Qt::PartiallyChecked:
        {
            break;
        }
    }
}



