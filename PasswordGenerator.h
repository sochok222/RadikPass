#ifndef PASSWORDGENERATOR_H
#define PASSWORDGENERATOR_H

#include <QDialog>
#include <qregularexpression.h>
#include <time.h>
#include <random>
#include <windows.h>
#include <cmath>

namespace Ui {
class PasswordGenerator;
}

namespace Password {
static const QRegularExpression regExp_uppercase("[A-Z]");
static const QRegularExpression regExp_lowercase("[a-z]");
static const QRegularExpression regExp_numbers("[0-9]");
static const QRegularExpression regExp_special("[^a-z^A-Z^0-9]");

static const QString s_uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const QString s_lowercase = "abcdefghijklmnopqrstuvwxyz";
static const QString s_numbers = "0123456789";
static const QString s_special = "!@#$%^&*-_";

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

    static int getEntropy(const QString &password);
    static Password::Strength getStrength(const int &entropy);

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
    Password::Strength getPasswordStrength(const QString &password);

    int selectedBoxes = 0;

    QString *result;
};

#endif // PASSWORDGENERATOR_H
