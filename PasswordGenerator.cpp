#include "PasswordGenerator.h"
#include "ui_PasswordGenerator.h"

PasswordGenerator::PasswordGenerator(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PasswordGenerator)
{
    ui->setupUi(this);
    ui->line_result->setReadOnly(true);


    ui->checkBox_az->setCheckState(Qt::Checked);
    ui->checkBox_AZ->setChecked(Qt::Checked);
    ui->checkBox_09->setChecked(Qt::Checked);
    ui->checkBox_special->setChecked(Qt::Unchecked);
    selectedBoxes = 3;

    connect(ui->checkBox_09, SIGNAL(checkStateChanged(Qt::CheckState)), this, SLOT(checkBoxPressed(Qt::CheckState)));
    connect(ui->checkBox_az, SIGNAL(checkStateChanged(Qt::CheckState)), this, SLOT(checkBoxPressed(Qt::CheckState)));
    connect(ui->checkBox_AZ, SIGNAL(checkStateChanged(Qt::CheckState)), this, SLOT(checkBoxPressed(Qt::CheckState)));
    connect(ui->checkBox_special, SIGNAL(checkStateChanged(Qt::CheckState)), this, SLOT(checkBoxPressed(Qt::CheckState)));


    ui->slider_lenght->setRange(5, 128);
    connect(ui->slider_lenght, SIGNAL(valueChanged(int)), ui->label_size, SLOT(setNum(int)));
    connect(ui->slider_lenght, SIGNAL(valueChanged(int)), this, SLOT(generatePassword()));
    connect(ui->button_copy, SIGNAL(clicked(bool)), this, SLOT(copyPassword()));
    connect(ui->button_regenerate, SIGNAL(clicked(bool)), this, SLOT(generatePassword()));
    srand(time(0));

    ui->slider_lenght->setValue(14);
}

PasswordGenerator::~PasswordGenerator() {
    delete ui;
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
    if (ui->checkBox_09->isChecked()) ui->checkBox_09->setDisabled(true);
    else if (ui->checkBox_az->isChecked()) ui->checkBox_az->setDisabled(true);
    else if (ui->checkBox_AZ->isChecked()) ui->checkBox_AZ->setDisabled(true);
    else ui->checkBox_special->setDisabled(true);
}


void PasswordGenerator::enableCheckBoxes() {
    ui->checkBox_09->setEnabled(true);
    ui->checkBox_az->setEnabled(true);
    ui->checkBox_AZ->setEnabled(true);
    ui->checkBox_special->setEnabled(true);
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
    QString buffer;
    qDebug() << "Size: " << ui->slider_lenght->value();
    if (ui->checkBox_az->isChecked()) buffer += az;
    if (ui->checkBox_AZ->isChecked()) buffer += AZ;
    if (ui->checkBox_09->isChecked()) buffer +=  numbers;
    if (ui->checkBox_special->isChecked()) buffer += special;

    QString result;
    for (int i = 0; i < ui->slider_lenght->value(); i++) {
        result.append(buffer[rand() % buffer.size()]);
    }

    ui->line_result->setText(result);
}
