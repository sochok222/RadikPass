#ifndef OPENDATABASE_H
#define OPENDATABASE_H

#include <QDialog>
#include <QString>
#include <QStandardPaths>
#include <QSettings>
#include <QByteArray>
#include <QMessageBox>

namespace Ui {
class OpenDatabase;
}

class OpenDatabase : public QDialog
{
    Q_OBJECT

public:
    explicit OpenDatabase(QWidget *parent = nullptr, QByteArray *result = nullptr);
    ~OpenDatabase();

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::OpenDatabase *ui;
    QSettings settings = QSettings("AlexRadik", "RadiPass");
    QByteArray *masterPassword;
};

#endif // OPENDATABASE_H
