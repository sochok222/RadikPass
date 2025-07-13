#include "PasswordGenerator.h"
#include "ui_PasswordGenerator.h"

PasswordGenerator::PasswordGenerator(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PasswordGenerator)
{
    ui->setupUi(this);

    ui->slider_lenght->setRange(5, 128);
    connect(ui->slider_lenght, SIGNAL(valueChanged(int)), ui->label_size, SLOT(setNum(int)));
    connect(ui->slider_lenght, SIGNAL(valueChanged(int)), this, SLOT(generatePassword(int)));
}

PasswordGenerator::~PasswordGenerator() {
    delete ui;
}


void PasswordGenerator::generatePassword(int size) {
    QString buffer;
    qDebug() << "Size: " << size;
    if (ui->checkBox_az->isChecked()) buffer += az;
    if (ui->checkBox_AZ->isChecked()) buffer += AZ;
    if (ui->checkBox_09->isChecked()) buffer += numbers;
    if (ui->checkBox_special->isChecked()) buffer += special;

    srand(time(0));

    QString result;
    for (int i = 0; i < size; i++) {
        result += buffer[rand() % buffer.size()];
    }

    ui->line_result->setText(result);
}
