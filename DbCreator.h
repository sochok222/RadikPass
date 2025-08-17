#ifndef DBCREATOR_H
#define DBCREATOR_H

#include <QDialog>
#include <QByteArray>
#include <QMessageBox>
#include <QTranslator>
#include <QScopedPointer>
#include <QSqlDatabase>
#include "IconLoader.h"
#include <QSettings>
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
    explicit DbCreator(QWidget *parent = nullptr, QSqlDatabase *resultDb = nullptr, QString *resultPath = nullptr, QByteArray *resultKey = nullptr, QVector<QString> *resultTables = nullptr, Theme theme = Theme::Dark);
    ~DbCreator();

private slots:
    void hidePassword();
    void button_save_clicked();

    void openPasswordGenerator();

    void lineEdit_repeat_textChanged(const QString &arg1);

    void lineEdit_password_textChanged(const QString &arg1);

private:
    Ui::DbCreator *ui;
    void showMsgBox(const QString &title, const QString &text, const QMessageBox::Icon &icon);
    QSqlDatabase *m_resultDb;
    QByteArray *m_resultKey;
    QVector<QString> *m_resultTables;
    QString *m_resultPath;
    bool m_repeatEnabled;
    QAction *m_action_hidePassword;
    QAction *m_action_openPasswordGenerator;
    Theme m_theme;
    QSettings m_settings;
    PasswordGenerator *m_window_PasswordGenerator;
};

#endif // DBCREATOR_H
