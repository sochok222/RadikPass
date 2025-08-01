#ifndef COLUMNSCONFIGURATOR_H
#define COLUMNSCONFIGURATOR_H

#include <QCheckBox>
#include <QSettings>
#include <QDialog>
#include <QVector>
#include <QTranslator>

namespace Ui {
class ColumnsConfigurator;
}

// This window class allows user to configure columns that will be shown or hidden
class ColumnsConfigurator : public QDialog
{
    Q_OBJECT

public:
    explicit ColumnsConfigurator(QWidget *parent = nullptr);
    ~ColumnsConfigurator();

private slots:
    void saveChanges(); // Slot for button_save

private:
    Ui::ColumnsConfigurator *ui;

    // Setting up tableWidget
    void setupTableWidget();

    // Adds checkbox to tableWidget
    void addCheckBox(int row_number, int column_number, QCheckBox *checkBox);

    // Fills tableWidget
    void fillTableWidget();

    // Loads current settings from QSettings
    void loadSettings();

    // Preparing checkBoxes
    QCheckBox *isTitleShown = new QCheckBox("");
    QCheckBox *isUsernameShown = new QCheckBox("");
    QCheckBox *isPasswordShown = new QCheckBox("");
    QCheckBox *isURLShown = new QCheckBox("");
    QCheckBox *isNotesShown = new QCheckBox("");
    QCheckBox *isCreationTimeShown = new QCheckBox("");
    QCheckBox *isLastChangedShown = new QCheckBox("");

    QCheckBox *isTitleAsterisks = new QCheckBox("");
    QCheckBox *isUsernameAsterisks = new QCheckBox("");
    QCheckBox *isPasswordAsterisks = new QCheckBox("");
    QCheckBox *isURLAsterisks = new QCheckBox("");
    QCheckBox *isNotesAsterisks = new QCheckBox("");
    QCheckBox *isCreationTimeAsterisks = new QCheckBox("");
    QCheckBox *isLastChangedAsterisks = new QCheckBox("");

    // Vector for checkBoxes
    QVector<QPair<QCheckBox*, QCheckBox*>> rows;
};

#endif // COLUMNSCONFIGURATOR_H
