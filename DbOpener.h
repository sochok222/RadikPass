#ifndef DBOPENER_H
#define DBOPENER_H

#include <QDialog>
#include <QString>
#include <QStandardPaths>
#include <QSettings>
#include <QByteArray>
#include <QMessageBox>
#include "IconLoader.h"
#include <QTranslator>

namespace Ui {
class DbOpener;
}
// This class will create window to allow user open database.
class DbOpener : public QDialog
{
    Q_OBJECT

public:
    // *result to reutrn master-key.
    explicit DbOpener(QWidget *parent = nullptr, QByteArray *result = nullptr, const QString &path = "", Theme theme = Theme::Dark);
    ~DbOpener();

private slots:
    void on_okButton_clicked(); // When user clicks Ok button.

    void on_cancelButton_clicked(); // When user clicks Cancel button.

    void hidePassword();

private:
    Ui::DbOpener *ui;

    Theme theme;

    QAction *action_hidePassword;

    // Loading settings.
    QSettings settings = QSettings("AlexRadik", "RadikPass");

    // Storing master-key.
    QByteArray *masterPassword;
};

#endif // DBOPENER_H
