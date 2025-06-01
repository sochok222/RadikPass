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
    // This class needs key to set encryption to new database, and path to show user where new database will be created
    // CreateDatabase class won't create database, DbManager will do this work
    explicit CreateDatabase(QWidget *parent = nullptr, QByteArray *key = nullptr, const QString path = "");
    ~CreateDatabase();

private slots:
    void on_saveButton_clicked(); // When user clicks "Ok" button

    void on_linePassword_textChanged(const QString &arg1); // When text in password line changed

    void on_lineRepeat_textChanged(const QString &arg1); // When text in repeat line changed

    void on_checkBoxIsAsterisks_checkStateChanged(const Qt::CheckState &arg1); // When user want to hide/unhide password in line

    void on_cancelButton_clicked(); // When user cancels action

private:
    Ui::CreateDatabase *ui;

    QByteArray *key; // To store address of master-key by which database will be encrypted

    bool isPassHidden = true; // By this variable program know either to password or not

    QValidator *validator; // This QVidator will check if password that user enters is correct
};

#endif // CREATEDATABASE_H
