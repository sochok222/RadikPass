#ifndef PASSWORDGENERATOR_H
#define PASSWORDGENERATOR_H

#include <QDialog>
#include <time.h>
#include <random>
#include <windows.h>

namespace Ui {
class PasswordGenerator;
}

enum class PasswordStrength {
    VeryWeak,
    Weak,
    Normal,
    Strong,
    VeryStrong
};

class PasswordGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordGenerator(QWidget *parent = nullptr);
    ~PasswordGenerator();

    PasswordStrength checkPassword(QString &password);

public slots:
    void generatePassword();
    void copyPassword();
    void checkBoxPressed(Qt::CheckState state);

private:
    Ui::PasswordGenerator *ui;

    void disableCheckBoxes();
    void enableCheckBoxes();

    QString AZ = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QString az = "abcdefghijklmnopqrstuvwxyz";
    QString numbers = "0123456789";
    QString special = "!@#$%^&*-_";
    int selectedBoxes = 0;

};

#endif // PASSWORDGENERATOR_H
