#include "configurecolumns.h"
#include <QTableWidget>
#include "ui_configurecolumns.h"

ConfigureColumns::ConfigureColumns(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConfigureColumns)
{
    ui->setupUi(this);
    this->setWindowTitle("Configure Columns");
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setRowCount(5);
    ui->tableWidget->setColumnCount(2);

    QStringList horizontal = QList<QString>({tr("Is shown"), tr("Is hiden by asterisk(*)")});
    QStringList vertical = QList<QString>({tr("Title"), tr("Username"), tr("Password"), ("URL"), tr("Notes")});
    ui->tableWidget->setVerticalHeaderLabels(vertical);
    ui->tableWidget->setHorizontalHeaderLabels(horizontal);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);


    setup();
}

ConfigureColumns::~ConfigureColumns()
{
    delete ui;
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

    // Basic value is unchecked, so I don't need to set Unchecked to item if column is not showing
    if(columnTitle[0] == "shown")
        ui->tableWidget->item(0,0)->setCheckState(Qt::Checked);
    if(columnTitle[1] == "masked")
        ui->tableWidget->item(0,1)->setCheckState(Qt::Checked);

    if(columnUsername[0] == "shown")
        ui->tableWidget->item(1,0)->setCheckState(Qt::Checked);
    if(columnUsername[1] == "masked")
        ui->tableWidget->item(1,1)->setCheckState(Qt::Checked);

    if(columnPassword[0] == "shown")
        ui->tableWidget->item(2,0)->setCheckState(Qt::Checked);
    if(columnPassword[1] == "masked")
        ui->tableWidget->item(2,1)->setCheckState(Qt::Checked);

    if(columnURL[0] == "shown")
        ui->tableWidget->item(3,0)->setCheckState(Qt::Checked);
    if(columnURL[1] == "masked")
        ui->tableWidget->item(3,1)->setCheckState(Qt::Checked);

    if(columnNotes[0] == "shown")
        ui->tableWidget->item(4,0)->setCheckState(Qt::Checked);
    if(columnNotes[1] == "masked")
        ui->tableWidget->item(4,1)->setCheckState(Qt::Checked);
}


void ConfigureColumns::setup()
{
    QVector<QPair<QTableWidgetItem*, QTableWidgetItem*>> rows = {
        {isTitleShown, isTitleAsterisks},
        {isUsernameShown, isUsernameAsterisks},
        {isPasswordShown, isPasswordAsterisks},
        {isURLShown, isURLAsterisks},
        {isNotesShown, isNotesAsterisks},
    };
    // Setting all items like it is checkbox
    int row = 0;
    for(QPair<QTableWidgetItem*, QTableWidgetItem*> &el : rows)
    {
        el.first->setCheckState(Qt::Unchecked); el.second->setCheckState(Qt::Unchecked);
        ui->tableWidget->setItem(row, 0, el.first); ui->tableWidget->setItem(row, 1, el.second);
        row++;
    }

    loadSettings();
}

void ConfigureColumns::on_saveButton_clicked()
{
    QSettings settings("AlexRadik", "RadiPass");

    QStringList columnTitle = QStringList() << "" << "";
    QStringList columnUsername = QStringList() << "" << "";
    QStringList columnPassword = QStringList() << "" << "";
    QStringList columnURL = QStringList() << "" << "";
    QStringList columnNotes = QStringList() << "" << "";

    // First values is show/hide column setting, second value is mask/unmask column setting
    //////////////////////////////////////////////////////////
    if(ui->tableWidget->item(0,0)->checkState() == Qt::Checked)
        columnTitle[0] = "shown";
    else columnTitle[0] = "hidden";


    if(ui->tableWidget->item(1,0)->checkState() == Qt::Checked)
        columnUsername[0] = "shown";
    else columnUsername[0] = "hidden";

    if(ui->tableWidget->item(2,0)->checkState() == Qt::Checked)
        columnPassword[0] = "shown";
    else columnPassword[0] = "hidden";

    if(ui->tableWidget->item(3,0)->checkState() == Qt::Checked)
        columnURL[0] = "shown";
    else columnURL[0] = "hidden";

    if(ui->tableWidget->item(4,0)->checkState() == Qt::Checked)
        columnNotes[0] = "shown";
    else columnNotes[0] = "hidden";
    //////////////////////////////////////////////////////////
    if(ui->tableWidget->item(0,1)->checkState() == Qt::Checked)
        columnTitle[1] = "masked";
    else columnTitle[1] = "unmasked";

    if(ui->tableWidget->item(1,1)->checkState() == Qt::Checked)
        columnUsername[1] = "masked";
    else columnUsername[1] = "unmasked";

    if(ui->tableWidget->item(2,1)->checkState() == Qt::Checked)
        columnPassword[1] = "masked";
    else columnPassword[1] = "unmasked";

    if(ui->tableWidget->item(3,1)->checkState() == Qt::Checked)
        columnURL[1] = "masked";
    else columnURL[1] = "unmasked";

    if(ui->tableWidget->item(4,1)->checkState() == Qt::Checked)
        columnNotes[1] = "masked";
    else columnNotes[1] = "unmasked";
    //////////////////////////////////////////////////////////
    settings.setValue("columnTitle", columnTitle);
    settings.setValue("columnUsername", columnUsername);
    settings.setValue("columnPassword", columnPassword);
    settings.setValue("columnURL", columnURL);
    settings.setValue("columnNotes", columnNotes);
    this->close();
}

