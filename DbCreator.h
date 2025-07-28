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
    void hidePassword();

    void openPasswordGenerator();

    void clicked_button_save();

    void clicked_button_cancel();

    void lineEdit_repeat_textChanged(const QString &arg1);

    void lineEdit_password_textChanged(const QString &arg1);

private:
    Ui::DbCreator *ui;
    QByteArray *key; // To store address of master-key by which database will be encrypted
    bool repeatEnabled; // By this variable program know either to password or not
    QAction *action_hidePassword;
    QAction *action_openPasswordGenerator;
    Theme theme;
    PasswordGenerator *window_PasswordGenerator;
};

#endif // DBCREATOR_H
