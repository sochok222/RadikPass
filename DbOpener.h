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

struct AppState {
    QVector<QString> tables; // Tables that are available to user
    QString pathToDatabase;
    QByteArray masterKey;
    bool isDbChanged;
    QSqlDatabase db;
    Theme theme;
};

namespace Ui {
class DbOpener;
}
// This class will create window to allow user open database.
class DbOpener : public QDialog
{
    Q_OBJECT

public:
    // *result to reutrn master-key.
    explicit DbOpener(QWidget *parent = nullptr, AppState *appState = nullptr);
    ~DbOpener();

private slots:
    void button_ok_clicked();
    void button_cancel_clicked();

    void hidePassword();

private:
    Ui::DbOpener *ui;

    void showMsgBox(const QString &title, const QString &text, const QMessageBox::Icon &icon);

    Theme theme;

    QAction *action_hidePassword;

    AppState *appState;

    QSettings settings = QSettings("AlexRadik", "RadikPass");
};

#endif // DBOPENER_H
