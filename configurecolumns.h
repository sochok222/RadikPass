#ifndef CONFIGURECOLUMNS_H
#define CONFIGURECOLUMNS_H

#include <QTableWidgetItem>
#include <QCheckBox>
#include <QSettings>
#include <QDialog>
#include <QVector>

namespace Ui {
class ConfigureColumns;
}

class ConfigureColumns : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureColumns(QWidget *parent = nullptr);
    ~ConfigureColumns();

private slots:
    void on_saveButton_clicked();

private:
    Ui::ConfigureColumns *ui;
    void setup();
    void loadSettings();
    // QTableWidgetItems(cells) that i will load to QTableWidget
    QTableWidgetItem *isTitleShown = new QTableWidgetItem("");
    QTableWidgetItem *isUsernameShown = new QTableWidgetItem("");
    QTableWidgetItem *isPasswordShown = new QTableWidgetItem("");
    QTableWidgetItem *isURLShown = new QTableWidgetItem("");
    QTableWidgetItem *isNotesShown = new QTableWidgetItem("");

    QTableWidgetItem *isTitleAsterisks = new QTableWidgetItem("");
    QTableWidgetItem *isUsernameAsterisks = new QTableWidgetItem("");
    QTableWidgetItem *isPasswordAsterisks = new QTableWidgetItem("");
    QTableWidgetItem *isURLAsterisks = new QTableWidgetItem("");
    QTableWidgetItem *isNotesAsterisks = new QTableWidgetItem("");
};

#endif // CONFIGURECOLUMNS_H
