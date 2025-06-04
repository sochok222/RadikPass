#ifndef CONFIGURECOLUMNS_H
#define CONFIGURECOLUMNS_H

#include <QTableWidgetItem>
#include <QCheckBox>
#include <QSettings>
#include <QDialog>
#include <QVector>
#include <QTranslator>

namespace Ui {
class ConfigureColumns;
}

// This window class allows user to configure columns that will be shown or hidden
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

    void fixRegistry();

    // This QTableWidgetItems are cells that will be loaded to tableView.
    QTableWidgetItem *isTitleShown = new QTableWidgetItem("");
    QTableWidgetItem *isUsernameShown = new QTableWidgetItem("");
    QTableWidgetItem *isPasswordShown = new QTableWidgetItem("");
    QTableWidgetItem *isURLShown = new QTableWidgetItem("");
    QTableWidgetItem *isNotesShown = new QTableWidgetItem("");
    QTableWidgetItem *isCreationTimeShown = new QTableWidgetItem("");
    QTableWidgetItem *isLastChangedShown = new QTableWidgetItem("");

    QTableWidgetItem *isTitleAsterisks = new QTableWidgetItem("");
    QTableWidgetItem *isUsernameAsterisks = new QTableWidgetItem("");
    QTableWidgetItem *isPasswordAsterisks = new QTableWidgetItem("");
    QTableWidgetItem *isURLAsterisks = new QTableWidgetItem("");
    QTableWidgetItem *isNotesAsterisks = new QTableWidgetItem("");
    QTableWidgetItem *isCreationTimeAsterisks = new QTableWidgetItem("");
    QTableWidgetItem *isLastChangedAsterisks = new QTableWidgetItem("");
};

#endif // CONFIGURECOLUMNS_H
