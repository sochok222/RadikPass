#ifndef PASSWORDGENERATOR_H
#define PASSWORDGENERATOR_H

#include <QDialog>
#include <time.h>
#include <random>
#include <windows.h>
#include <cmath>

namespace Ui {
class PasswordGenerator;
}

namespace PasswordOptions {
enum Combinations {
    Uppercase = 0x0001,
    Lowercase = 0x0010,
    Numbers = 0x0100,
    Special = 0x1000
};
enum Strength {
    Weak,
    Normal,
    Strong,
    VeryStrong
};
}

class PasswordGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordGenerator(QWidget *parent = nullptr, QString *result = nullptr);
    ~PasswordGenerator();

    PasswordOptions::Strength checkPassword(QString &password);
    QString generator(int length, int flags);

private slots:
    void copyPassword();
    void checkBoxPressed(Qt::CheckState state);
    void generatePassword();

    void on_button_close_clicked();

    void on_button_save_clicked();

private:
    Ui::PasswordGenerator *ui;

    void disableCheckBoxes();
    void enableCheckBoxes();

    QString Uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QString Lowercase = "abcdefghijklmnopqrstuvwxyz";
    QString Numbers = "0123456789";
    QString Special = "!@#$%^&*-_";
    int selectedBoxes = 0;

    QString *result;
};

#endif // PASSWORDGENERATOR_H
