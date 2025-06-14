#include "configurecolumns.h"
#include <QTableWidget>
#include <qmessagebox.h>
#include "ui_configurecolumns.h"

ConfigureColumns::ConfigureColumns(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConfigureColumns)
{
    ui->setupUi(this);
    this->setWindowTitle("Configure Columns");
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setRowCount(8);
    ui->tableWidget->setColumnCount(2);

    QStringList horizontal = QList<QString>({tr("Is shown"), tr("Is hiden by asterisk(*)")});
    QStringList vertical = QList<QString>({tr("Title"), tr("Username"), tr("Password"), ("URL"), tr("Notes"),
                                           tr("Creation Time"), tr("Last Changed")});
    ui->tableWidget->setVerticalHeaderLabels(vertical);
    ui->tableWidget->setHorizontalHeaderLabels(horizontal);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    rows = {
        {isTitleShown, isTitleAsterisks},
        {isUsernameShown, isUsernameAsterisks},
        {isPasswordShown, isPasswordAsterisks},
        {isURLShown, isURLAsterisks},
        {isNotesShown, isNotesAsterisks},
        {isCreationTimeShown, isCreationTimeAsterisks},
        {isLastChangedShown, isLastChangedAsterisks}
    };

    setup();
    loadSettings();
}

ConfigureColumns::~ConfigureColumns()
{
    delete ui;

    delete isTitleShown;
    delete isUsernameShown;
    delete isPasswordShown;
    delete isURLShown;
    delete isNotesShown;
    delete isCreationTimeShown;
    delete isLastChangedShown;

    delete isTitleAsterisks;
    delete isUsernameAsterisks;
    delete isPasswordAsterisks;
    delete isURLAsterisks;
    delete isNotesAsterisks;
    delete isCreationTimeAsterisks;
    delete isLastChangedAsterisks;
}


void ConfigureColumns::addCheckBoxAt(int row_number, int column_number, QCheckBox *checkBox)
{
        // Create a widget that will contain a checkbox
    QWidget *checkBoxWidget = new QWidget();
    QHBoxLayout *layoutCheckBox = new QHBoxLayout(checkBoxWidget); // create a layer with reference to the widget
    layoutCheckBox->addWidget(checkBox);            // Set the checkbox in the layer
    layoutCheckBox->setAlignment(Qt::AlignCenter);  // Center the checkbox
    layoutCheckBox->setContentsMargins(0,0,0,0);    // Set the zero padding

    checkBox->setCheckState(Qt::Unchecked);

    ui->tableWidget->setCellWidget(row_number,column_number, checkBoxWidget);
}


void ConfigureColumns::loadSettings()
{
    // Loading settings from QSettings
    QSettings settings("AlexRadik", "RadiPass");
    QStringList columnTitle = settings.value("columnTitle").toStringList();
    QStringList columnUsername = settings.value("columnUsername").toStringList();
    QStringList columnPassword = settings.value("columnPassword").toStringList();
    QStringList columnURL = settings.value("columnURL").toStringList();
    QStringList columnNotes = settings.value("columnNotes").toStringList();
    QStringList columnCreationTime = settings.value("columnCreationTime").toStringList();
    QStringList columnLastChanged = settings.value("columnLastChanged").toStringList();

    QVector<QStringList*> settingsList = {
                &columnTitle, &columnUsername, &columnPassword, &columnURL, &columnNotes, &columnCreationTime, &columnLastChanged};

    for(int i = 0; i < settingsList.size(); i++)
    {
        if((*settingsList[i])[0] == "shown")
        {
            rows[i].first->setCheckState(Qt::Checked);
        }
        if((*settingsList[i])[1] == "masked")
        {
            rows[i].second->setCheckState(Qt::Checked);
        }
    }
}


void ConfigureColumns::setup()
{
    // Filling tableView with cells.
    int row = 0;
    for(QPair<QCheckBox*, QCheckBox*> &el : rows)
    {
        el.first->setCheckState(Qt::Unchecked); el.second->setCheckState(Qt::Unchecked);
        addCheckBoxAt(row, 0, el.first); addCheckBoxAt(row, 1, el.second);
        row++;
    }
}

void ConfigureColumns::on_saveButton_clicked()
{
    QSettings settings("AlexRadik", "RadiPass"); // Loading current settings to QSettings, this will write settings to registry

    // QStringList to store two values in one QSettings field
    QStringList columnTitle = QStringList() << "asd" << "asdf";
    QStringList columnUsername = QStringList() << "" << "";
    QStringList columnPassword = QStringList() << "" << "";
    QStringList columnURL = QStringList() << "" << "";
    QStringList columnNotes = QStringList() << "" << "";
    QStringList columnCreationTime = QStringList() << "" << "";
    QStringList columnLastChanged = QStringList() << "" << "";

    QVector<QStringList*> settingsList = {
                        &columnTitle, &columnUsername, &columnPassword, &columnURL, &columnNotes, &columnCreationTime, &columnLastChanged};


    // Cells
    QVector<QPair<QCheckBox*, QCheckBox*>> rows = {
        {isTitleShown, isTitleAsterisks},
        {isUsernameShown, isUsernameAsterisks},
        {isPasswordShown, isPasswordAsterisks},
        {isURLShown, isURLAsterisks},
        {isNotesShown, isNotesAsterisks},
        {isCreationTimeShown, isCreationTimeAsterisks},
        {isLastChangedShown, isLastChangedAsterisks}
    };

    for(int i = 0; i < settingsList.size(); i++)
    {
        if(rows[i].first->checkState())
        {
            (*settingsList[i])[0] = "shown";
        }else (*settingsList[i])[0] = "hidden";
        if(rows[i].second->checkState() == Qt::Checked)
        {
            (*settingsList[i])[1] = "masked";
        }else (*settingsList[i])[1] = "unmasked";

    }

    // Loading that values to QSettings
    settings.setValue("columnTitle", columnTitle);
    settings.setValue("columnUsername", columnUsername);
    settings.setValue("columnPassword", columnPassword);
    settings.setValue("columnURL", columnURL);
    settings.setValue("columnNotes", columnNotes);
    settings.setValue("columnCreationTime", columnCreationTime);
    settings.setValue("columnLastChanged", columnLastChanged);

    this->accept(); // Closing this window
}

