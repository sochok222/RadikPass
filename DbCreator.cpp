#include "DbCreator.h"
#include "ui_DbCreator.h"

DbCreator::DbCreator(QWidget *parent, QByteArray *key, const QString path)
    : QDialog(parent)
    , ui(new Ui::DbCreator)
    , key(key)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("Create Database")); // Setting window title

    if (path != "") // If path is not empty
    {
        ui->labelPath->setText("Path: " + path); // Writing path to window
    }else { // Else showing error to user
        QMessageBox msg;
        msg.setText(tr("Can't load path"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        this->close();
    }

    // Setting maximum length to password lines
    ui->linePassword->setMaxLength(50);
    ui->lineRepeat->setMaxLength(50);

    // Setting tooltips
    ui->checkBoxIsAsterisks->setToolTip(tr("Show/Hide password using asterisks"));
    ui->linePassword->setToolTip(tr("Password can contain letters, digits and special symbols only"));
    ui->labelRepeat->setToolTip(tr("Password quality for now is based on lenght"));

    ui->checkBoxIsAsterisks->setChecked(true); // Set checked as default

    // Setting echo mode to password lines
    ui->linePassword->setEchoMode(QLineEdit::Password);
    ui->lineRepeat->setEchoMode(QLineEdit::Password);

    // Creating validator for password
    QRegularExpression rx(R"(^[a-zA-Zа-яА-Я0-9!@#$&().+,/-]*$)");
    validator = new QRegularExpressionValidator(rx, this);
    // Setting validator for password lines
    ui->linePassword->setValidator(validator);
    ui->lineRepeat->setValidator(validator);
}

DbCreator::~DbCreator()
{
    delete ui;
    delete validator;
}


void DbCreator::on_saveButton_clicked()
{
    *key = ui->linePassword->text().toUtf8(); // Returning key that user entered
    this->close(); // Closing window
}

void DbCreator::on_linePassword_textChanged(const QString &arg1)
{
    // Checking password quality depending on length
    switch(ui->linePassword->text().size()) {
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

void DbCreator::on_lineRepeat_textChanged(const QString &arg1)
{
    // If password is hidden
    if (isPassHidden) {
        if (arg1!=ui->linePassword->text() && ui->lineRepeat->text().size() > 0) { // If password in repeat line is not the same as in password line
            // Setting red palette to repeat line
            QPalette lineRepeatPalette;
            lineRepeatPalette.setColor(QPalette::Base, QColor(235,135,135));
            ui->lineRepeat->setPalette(lineRepeatPalette);
        }else {
            // Else set default palette
            QPalette lineRepeatPalette;
            ui->lineRepeat->setPalette(lineRepeatPalette);
        }
    }
}

void DbCreator::on_checkBoxIsAsterisks_checkStateChanged(const Qt::CheckState &arg1)
{
    switch(arg1) {
    case Qt::Checked: // If QCheckBox is checked
    {
        ui->linePassword->setEchoMode(QLineEdit::Password); // Setting password echo mode to password line
        ui->lineRepeat->setEchoMode(QLineEdit::Password); // Setting password echo mode to repeat password line

        ui->lineRepeat->setEnabled(true); // Enabling repeat password line
        ui->labelRepeat->setEnabled(true); // Enabling label near repeat password line

        ui->lineRepeat->show(); // Showing repeat password line
        ui->labelRepeat->show(); // Showing label near repeat password line

        isPassHidden = true; // isPassHidden used to check if needs to check repeat line

        break;
    }
    case Qt::Unchecked:
    {
        ui->linePassword->setEchoMode(QLineEdit::Normal); // Setting normal mode to password line

        ui->lineRepeat->clear(); // Clearing repeat password line

        // Disabling repeat password line and label near it
        ui->lineRepeat->setEnabled(false);
        ui->labelRepeat->setEnabled(false);
        // Hiding
        ui->lineRepeat->hide();
        ui->labelRepeat->hide();

        isPassHidden = false; // isPassHidden used to check if needs to check repeat line

        break;
    }
    case Qt::PartiallyChecked:
    {
        break;
    }
    }
}


void DbCreator::on_cancelButton_clicked()
{
    this->reject(); // Closing window
}

