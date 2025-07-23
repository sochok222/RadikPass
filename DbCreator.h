#ifndef DBCREATOR_H
#define DBCREATOR_H

#include <QDialog>
#include <QByteArray>
#include <QMessageBox>
#include <QTranslator>
#include <QScopedPointer>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "IconLoader.h"
#include "PasswordGenerator.h"

namespace Ui {
class DbCreator;
}

class DbCreator : public QDialog
{
    Q_OBJECT

public:
    // This class needs key to set encryption to new database, and path to show user where new database will be created
    // DbCreator class won't create database, DbManager will do this work
    explicit DbCreator(QWidget *parent = nullptr, QByteArray *key = nullptr, const QString path = "", Theme theme = Theme::Dark);
    ~DbCreator();

private slots:
    void on_saveButton_clicked(); // When user clicks "Ok" button

    void on_linePassword_textChanged(const QString &arg1); // When text in password line changed

    void on_lineRepeat_textChanged(const QString &arg1); // When text in repeat line changed

    void on_cancelButton_clicked(); // When user cancels action

    void hidePassword();

    void openPasswordGenerator();

private:
    Ui::DbCreator *ui;
    QByteArray *key; // To store address of master-key by which database will be encrypted
    bool repeatEnabled; // By this variable program know either to password or not
    QValidator *validator; // This QVidator will check if password that user enters is correct
    QAction *action_hidePassword;
    QAction *action_openPasswordGenerator;
    Theme theme;
};

#endif // DBCREATOR_H
