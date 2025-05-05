#ifndef CREATEDATABASE_H
#define CREATEDATABASE_H

#include <QDialog>
#include <QByteArray>

namespace Ui {
class CreateDatabase;
}

class CreateDatabase : public QDialog
{
    Q_OBJECT

public:
    explicit CreateDatabase(QWidget *parent = nullptr, QByteArray *masterPassword = nullptr);
    ~CreateDatabase();

private slots:
    void on_saveButton_clicked();

    void on_masterPassword_textChanged(const QString &arg1);

private:
    Ui::CreateDatabase *ui;
    QByteArray *key;
};

#endif // CREATEDATABASE_H
