#ifndef PASSWORDGENERATOR_H
#define PASSWORDGENERATOR_H

#include <QDialog>
#include <time.h>
#include <random>

namespace Ui {
class PasswordGenerator;
}

class PasswordGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordGenerator(QWidget *parent = nullptr);
    ~PasswordGenerator();

public slots:
    void generatePassword(int size);

private:
    Ui::PasswordGenerator *ui;

    QString AZ = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QString az = "abcdefghijklmnopqrstuvwxyz";
    QString numbers = "0123456789";
    QString special = "!@#$%^&*-_";

};

#endif // PASSWORDGENERATOR_H
