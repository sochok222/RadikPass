#include "PasswordGenerator.h"
#include "ui_PasswordGenerator.h"

PasswordGenerator::PasswordGenerator(QWidget *parent, QString *result)
    : QDialog(parent)
    , ui(new Ui::PasswordGenerator)
{
    ui->setupUi(this);
    ui->line_result->setReadOnly(true);

    if (!result) ui->button_apply->hide();

    ui->checkBox_Lowercase->setCheckState(Qt::Checked);
    ui->checkBox_Uppercase->setChecked(Qt::Checked);
    ui->checkBox_Numbers->setChecked(Qt::Checked);
    ui->checkBox_Special->setChecked(Qt::Unchecked);
    selectedBoxes = 3;

    connect(ui->checkBox_Numbers, SIGNAL(checkStateChanged(Qt::CheckState)), this, SLOT(checkBoxPressed(Qt::CheckState)));
    connect(ui->checkBox_Lowercase, SIGNAL(checkStateChanged(Qt::CheckState)), this, SLOT(checkBoxPressed(Qt::CheckState)));
    connect(ui->checkBox_Uppercase, SIGNAL(checkStateChanged(Qt::CheckState)), this, SLOT(checkBoxPressed(Qt::CheckState)));
    connect(ui->checkBox_Special, SIGNAL(checkStateChanged(Qt::CheckState)), this, SLOT(checkBoxPressed(Qt::CheckState)));


    ui->slider_lenght->setRange(5, 64);
    connect(ui->slider_lenght, SIGNAL(valueChanged(int)), this, SLOT(generatePassword()));
    connect(ui->button_copy, SIGNAL(clicked(bool)), this, SLOT(copyPassword()));
    connect(ui->button_regenerate, SIGNAL(clicked(bool)), this, SLOT(generatePassword()));
    srand(time(0));

    ui->slider_lenght->setValue(14);
}

PasswordGenerator::~PasswordGenerator() {
    delete ui;
}


QString PasswordGenerator::generator(int lenght, int flags) {
    QString buffer;

    if (flags & PasswordOptions::Lowercase) buffer += Lowercase;
    if (flags & PasswordOptions::Uppercase) buffer += Uppercase;
    if (flags & PasswordOptions::Numbers) buffer +=  Numbers;
    if (flags & PasswordOptions::Special) buffer += Special;

    QString result;
    for (int i = 0; i < ui->slider_lenght->value(); i++) {
        result.append(buffer[rand() % buffer.size()]);
    }
    return result;
}


PasswordOptions::Strength PasswordGenerator::checkPassword(QString &password) {
    int pool = 0;

    if (ui->checkBox_Lowercase->isChecked()) pool += Lowercase.size();
    if (ui->checkBox_Uppercase->isChecked()) pool += Uppercase.size();
    if (ui->checkBox_Numbers->isChecked()) pool +=  Numbers.size();
    if (ui->checkBox_Special->isChecked()) pool += Special.size();

    int strength = std::log(std::pow(pool, password.size())) / std::log(2);

    if (strength > 0 && strength <= 35) return PasswordOptions::VeryWeak;
    else if (strength > 35 && strength <= 50) return PasswordOptions::Weak;
    else if (strength > 50 && strength <= 65) return PasswordOptions::Normal;
    else if (strength > 65 && strength <= 80) return PasswordOptions::Strong;
    else return PasswordOptions::VeryStrong;
}


void PasswordGenerator::checkBoxPressed(Qt::CheckState state) {
    switch(state) {
    case Qt::Checked:
        selectedBoxes++;

        if (selectedBoxes == 2)
            enableCheckBoxes();
        break;
    case Qt::Unchecked:
        selectedBoxes--;
        if (selectedBoxes == 1)
            disableCheckBoxes();
    default:
        break;
    }
    generatePassword();
}


void PasswordGenerator::disableCheckBoxes() {
    if (ui->checkBox_Numbers->isChecked()) ui->checkBox_Numbers->setDisabled(true);
    else if (ui->checkBox_Lowercase->isChecked()) ui->checkBox_Lowercase->setDisabled(true);
    else if (ui->checkBox_Uppercase->isChecked()) ui->checkBox_Uppercase->setDisabled(true);
    else ui->checkBox_Special->setDisabled(true);
}


void PasswordGenerator::enableCheckBoxes() {
    ui->checkBox_Numbers->setEnabled(true);
    ui->checkBox_Lowercase->setEnabled(true);
    ui->checkBox_Uppercase->setEnabled(true);
    ui->checkBox_Special->setEnabled(true);
}

void PasswordGenerator::copyPassword() {
    OpenClipboard(0);
    EmptyClipboard();
    HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, ui->line_result->text().size() + 1);

    strcpy_s((char*)hGlob, ui->line_result->text().size() + 1, &ui->line_result->text().toStdString()[0]);
    SetClipboardData(CF_TEXT, hGlob);
    CloseClipboard();
}


void PasswordGenerator::generatePassword() {
    qInfo() << Q_FUNC_INFO;
    ui->label_size->setText(&"Size: " [ui->slider_lenght->value()]);
    QString buffer;

    int flags = 0;
    if (ui->checkBox_Lowercase->isChecked()) flags |= PasswordOptions::Lowercase;
    if (ui->checkBox_Uppercase->isChecked()) flags |= PasswordOptions::Uppercase;
    if (ui->checkBox_Numbers->isChecked()) flags |=  PasswordOptions::Numbers;
    if (ui->checkBox_Special->isChecked()) flags |= PasswordOptions::Special;

    QString password = generator(ui->slider_lenght->value(), flags);
    ui->line_result->setText(password);

    switch (checkPassword(password)) {
    case PasswordOptions::VeryWeak:
        ui->label_passwordScore->setText("Very weak");
        break;
    case PasswordOptions::Weak:
        ui->label_passwordScore->setText("Weak");
        break;
    case PasswordOptions::Normal:
        ui->label_passwordScore->setText("Normal");
        break;
    case PasswordOptions::Strong:
        ui->label_passwordScore->setText("Strong");
        break;
    case PasswordOptions::VeryStrong:
        ui->label_passwordScore->setText("Very Strong");
        break;
    default:
        break;
    }
}

void PasswordGenerator::on_button_apply_clicked() {
    *result = ui->line_result->text();
    this->close();
}

