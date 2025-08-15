#ifndef DBOPENER_H
#define DBOPENER_H

#include <QDialog>
#include <QString>
#include <QStandardPaths>
#include <QSettings>
#include <QByteArray>
#include <QMessageBox>
#include "IconLoader.h"
#include <QTimer>
#include "dbmanager.h"
#include <QTranslator>


namespace Ui {
class DbOpener;
}

class DbOpener : public QDialog
{
    Q_OBJECT

public:
    explicit DbOpener(QWidget *parent = nullptr, QSqlDatabase *db = nullptr, QString pathToDatabase = "", QByteArray *resultKey = nullptr, QVector<QString> *tables = nullptr, Theme colorTheme = Theme::Dark);
    ~DbOpener();

private slots:
    void button_ok_clicked();
    // void button_cancel_clicked();

    void hidePassword();

private:
    Ui::DbOpener *ui;

    void showMsgBox(const QString &title, const QString &text, const QMessageBox::Icon &icon);

    QSqlDatabase *db;
    QString pathToDatabase;
    QByteArray *resultKey;
    QVector<QString> *tables;
    Theme colorTheme;


    QAction *action_hidePassword;

    QSettings settings = QSettings("AlexRadik", "RadikPass");
};

#endif // DBOPENER_H
