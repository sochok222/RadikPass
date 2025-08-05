#ifndef DBCREATOR_H
#define DBCREATOR_H

#include <QDialog>
#include <QByteArray>
#include <QMessageBox>
#include <QTranslator>
#include <QScopedPointer>
#include <QSqlDatabase>
#include "IconLoader.h"
#include "DbManager.h"
#include "PasswordGenerator.h"

/*
 * This class must create database and load it to *resultDb, save path to 'last' in QSettings and copy it to *resultPath and load user-available tables to *tables
*/


namespace Ui {
class DbCreator;
}

class DbCreator : public QDialog
{
    Q_OBJECT

public:
    explicit DbCreator(QWidget *parent = nullptr, QSqlDatabase *resultDb = nullptr, QString *resultPath = nullptr, QByteArray *resultKey = nullptr, Theme theme = Theme::Dark);
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
    void showMsgBox(const QString &title, const QString &text, const QMessageBox::Icon &icon);
    QSqlDatabase *resultDb;
    QString *resultPath;
    bool repeatEnabled;
    QAction *action_hidePassword;
    QAction *action_openPasswordGenerator;
    Theme theme;
    PasswordGenerator *window_PasswordGenerator;
};

#endif // DBCREATOR_H
