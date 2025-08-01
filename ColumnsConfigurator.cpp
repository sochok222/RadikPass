#include "ColumnsConfigurator.h"
#include <QTableWidget>
#include <qmessagebox.h>
#include "ui_ColumnsConfigurator.h"

ColumnsConfigurator::ColumnsConfigurator(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ColumnsConfigurator)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("Configure Columns")); // Setting window title

    // Initializing vector with comboBoxes that used in tableWidget
    rows = {
        {isTitleShown, isTitleAsterisks},
        {isUsernameShown, isUsernameAsterisks},
        {isPasswordShown, isPasswordAsterisks},
        {isURLShown, isURLAsterisks},
        {isNotesShown, isNotesAsterisks},
        {isCreationTimeShown, isCreationTimeAsterisks},
        {isLastChangedShown, isLastChangedAsterisks}
    };

    setupTableWidget();

    // Connecting button to slot
    connect(ui->button_save, SIGNAL(clicked(bool)), this, SLOT(saveChanges()));
}

ColumnsConfigurator::~ColumnsConfigurator() {
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

void ColumnsConfigurator::setupTableWidget() {
    // Disabling selection and editing
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    // Stretching cells to size of widget
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Creating rows and columns
    ui->tableWidget->setRowCount(7);
    ui->tableWidget->setColumnCount(2);

    // Horizontal and vertical headers
    QStringList horizontal = QList<QString>({tr("Shown"), tr("Masked")});
    QStringList vertical = QList<QString>({tr("Title"), tr("Username"), tr("Password"), ("URL"), tr("Notes"),
                                           tr("Creation Time"), tr("Last Changed")});

    // Adding headers to widget
    ui->tableWidget->setVerticalHeaderLabels(vertical);
    ui->tableWidget->setHorizontalHeaderLabels(horizontal);

    fillTableWidget(); // Filling cells in tableWidget
    loadSettings(); // Loading current settings
}

void ColumnsConfigurator::addCheckBox(int row_number, int column_number, QCheckBox *checkBox) {
        // Create a widget that will contain a checkbox
    QWidget *checkBoxWidget = new QWidget();
    QHBoxLayout *layoutCheckBox = new QHBoxLayout(checkBoxWidget); // create a layer with reference to the widget
    layoutCheckBox->addWidget(checkBox);            // Set the checkbox in the layer
    layoutCheckBox->setAlignment(Qt::AlignCenter);  // Center the checkbox
    layoutCheckBox->setContentsMargins(0,0,0,0);    // Set the zero padding

    checkBox->setCheckState(Qt::Unchecked);

    ui->tableWidget->setCellWidget(row_number,column_number, checkBoxWidget);
}


void ColumnsConfigurator::loadSettings() {
    // Loading settings from QSettings
    QSettings settings("AlexRadik", "RadikPass");
    QStringList columnTitle = settings.value("columnTitle").toStringList();
    QStringList columnUsername = settings.value("columnUsername").toStringList();
    QStringList columnPassword = settings.value("columnPassword").toStringList();
    QStringList columnURL = settings.value("columnURL").toStringList();
    QStringList columnNotes = settings.value("columnNotes").toStringList();
    QStringList columnCreationTime = settings.value("columnCreationTime").toStringList();
    QStringList columnLastChanged = settings.value("columnLastChanged").toStringList();

    // Initializing list of settings
    QVector<QStringList*> settingsList = {
                &columnTitle, &columnUsername, &columnPassword, &columnURL, &columnNotes, &columnCreationTime, &columnLastChanged};

    for (int i = 0; i < settingsList.size(); i++) {
        if ((*settingsList[i])[0] == "shown") { // If value is "shown" set checked state to check box in first row
            rows[i].first->setCheckState(Qt::Checked);
        }
        if ((*settingsList[i])[1] == "masked") { // If value is "masked" set checked state to check box in second row
            rows[i].second->setCheckState(Qt::Checked);
        }
    }
}


void ColumnsConfigurator::fillTableWidget() {
    // Adding comboBoxes
    int row = 0;
    for (QPair<QCheckBox*, QCheckBox*> &el : rows) {
        el.first->setCheckState(Qt::Unchecked); el.second->setCheckState(Qt::Unchecked); // Settings unchecked state as default
        addCheckBox(row, 0, el.first); addCheckBox(row, 1, el.second); // Adding CheckBoxes to cells
        row++;
    }
}

void ColumnsConfigurator::saveChanges() {
    QSettings settings("AlexRadik", "RadikPass"); // Loading current settings to QSettings, this will write settings to registry

    // Storing new settings in QStringList
    QStringList columnTitle = QStringList() << "" << "";
    QStringList columnUsername = QStringList() << "" << "";
    QStringList columnPassword = QStringList() << "" << "";
    QStringList columnURL = QStringList() << "" << "";
    QStringList columnNotes = QStringList() << "" << "";
    QStringList columnCreationTime = QStringList() << "" << "";
    QStringList columnLastChanged = QStringList() << "" << "";

    // Creating list to iterate through it
    QVector<QStringList*> settingsList = {
                        &columnTitle, &columnUsername, &columnPassword, &columnURL, &columnNotes, &columnCreationTime, &columnLastChanged};


    // Saving settings to settingsList
    for (int i = 0; i < settingsList.size(); i++) {
        if (rows[i].first->checkState()) {
            (*settingsList[i])[0] = "shown";
        } else (*settingsList[i])[0] = "hidden";
        if (rows[i].second->checkState() == Qt::Checked) {
            (*settingsList[i])[1] = "masked";
        } else (*settingsList[i])[1] = "unmasked";

    }

    settings.setValue("columnTitle", columnTitle);
    settings.setValue("columnUsername", columnUsername);
    settings.setValue("columnPassword", columnPassword);
    settings.setValue("columnURL", columnURL);
    settings.setValue("columnNotes", columnNotes);
    settings.setValue("columnCreationTime", columnCreationTime);
    settings.setValue("columnLastChanged", columnLastChanged);

    this->accept(); // Closing this window
}

