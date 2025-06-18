#ifndef OPENDATABASE_H
#define OPENDATABASE_H

#include <QDialog>
#include <QString>
#include <QStandardPaths>
#include <QSettings>
#include <QByteArray>
#include <QMessageBox>
#include <QTranslator>

namespace Ui {
class OpenDatabase;
}
// This class will create window to allow user open database.
class OpenDatabase : public QDialog
{
    Q_OBJECT

public:
    // *result to reutrn master-key.
    explicit OpenDatabase(QWidget *parent = nullptr, QByteArray *result = nullptr, const QString &path = "");
    ~OpenDatabase();

private slots:
    void on_okButton_clicked(); // When user clicks Ok button.

    void on_cancelButton_clicked(); // When user clicks Cancel button.

private:
    Ui::OpenDatabase *ui;

    // Loading settings.
    QSettings settings = QSettings("AlexRadik", "RadiPass");

    // Storing master-key.
    QByteArray *masterPassword;
};

#endif // OPENDATABASE_H
