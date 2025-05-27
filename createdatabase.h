#ifndef CREATEDATABASE_H
#define CREATEDATABASE_H

#include <QDialog>
#include <QByteArray>
#include <QMessageBox>
#include <QTranslator>
#include <QScopedPointer>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

namespace Ui {
class CreateDatabase;
}

class CreateDatabase : public QDialog
{
    Q_OBJECT

public:
    explicit CreateDatabase(QWidget *parent = nullptr, QByteArray *masterPassword = nullptr, const QString path = "");
    ~CreateDatabase();

private slots:
    void on_saveButton_clicked();

    void on_linePassword_textChanged(const QString &arg1);

    void on_checkBoxIsAsterisks_checkStateChanged(const Qt::CheckState &arg1);

    void on_lineRepeat_textChanged(const QString &arg1);

private:
    Ui::CreateDatabase *ui;
    QByteArray *key;
    bool isPassHidden = true;
    QValidator *validator;
};

#endif // CREATEDATABASE_H
