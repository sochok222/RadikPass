#include "mainwindow.h"
#include "./ui_mainwindow.h"


void MainWindow::configureColumns() // Showing or hiding columns in tableView according to settings
{
    // Bassically ui->tableView->setColumnHidden is false, so I don't need to set it to false if column is need to be shown
    QStringList columnTitle = settings.value("columnTitle").toStringList();
    QStringList columnUsername = settings.value("columnUsername").toStringList();
    QStringList columnPassword = settings.value("columnPassword").toStringList();
    QStringList columnURL = settings.value("columnURL").toStringList();
    QStringList columnNotes = settings.value("columnNotes").toStringList();

    ui->tableView->setColumnHidden(0,true); // Column with id of row is always hidden!

    if(columnTitle.value(0) == "shown")
        ui->tableView->setColumnHidden(1, false);
    else ui->tableView->setColumnHidden(1, true);
    if(columnTitle.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(1, 0);
    else ui->tableView->setItemDelegateForColumn(1, maskColumn);

    if(columnUsername.value(0) == "shown")
        ui->tableView->setColumnHidden(2, false);
    else ui->tableView->setColumnHidden(2, true);
    if(columnUsername.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(2, 0);
    else ui->tableView->setItemDelegateForColumn(2, maskColumn);

    if(columnPassword.value(0) == "shown")
        ui->tableView->setColumnHidden(3, false);
    else ui->tableView->setColumnHidden(3, true);
    if(columnPassword.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(3, 0);
    else ui->tableView->setItemDelegateForColumn(3, maskColumn);

    if(columnURL.value(0) == "shown")
        ui->tableView->setColumnHidden(4, false);
    else ui->tableView->setColumnHidden(4, true);
    if(columnURL.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(4, 0);
    else ui->tableView->setItemDelegateForColumn(4, maskColumn);

    if(columnNotes.value(0) == "shown")
        ui->tableView->setColumnHidden(5, false);
    else ui->tableView->setColumnHidden(5, true);
    if(columnNotes.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(5, 0);
    else ui->tableView->setItemDelegateForColumn(5, maskColumn);
}


MainWindow::MainWindow(QWidget *parent, QByteArray MasterKey, QTranslator *translator, QString theme)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , key(MasterKey)
    , translator(translator)
    , theme(theme)
{
    qDebug() << Q_FUNC_INFO; // Writing function names to see where error appears, all this messages shown in Application Output
    ui->setupUi(this);

    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->actionLanguageUkrainian, SIGNAL(triggered(bool)), SLOT(setUkrainianLanguage()));
    connect(ui->actionLanguageEnglish, SIGNAL(triggered(bool)), SLOT(setEnglishLanguage()));

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->verticalHeader()->setVisible(false);

    // Setting icons to toolbar
    ui->buttonOpen->setIcon(IconLoader::getIcon(Icon::open, theme));
    ui->buttonNew->setIcon(IconLoader::getIcon(Icon::create, theme));
    ui->buttonAddEntry->setIcon(IconLoader::getIcon(Icon::add, theme));
    ui->buttonCopyUsername->setIcon(IconLoader::getIcon(Icon::user, theme));
    ui->buttonCopyPassword->setIcon(IconLoader::getIcon(Icon::key, theme));
    ui->buttonDeleteEntry->setIcon(*icons["trash"]);

    // Setting hints to toolbar
    ui->buttonNew->setToolTip("New Database");
    ui->buttonOpen->setToolTip("Open Database");
    ui->buttonSave->setToolTip("Save");
    ui->buttonAddEntry->setToolTip("Add Entry");
    ui->buttonCopyUsername->setToolTip("Copy Username");
    ui->buttonCopyPassword->setToolTip("Copy Password");
    ui->buttonDeleteEntry->setToolTip("Delete Entry");

    // Connecting toolbar buttons to slots
    connect(ui->buttonNew, SIGNAL(clicked()), SLOT(createDatabase()));
    connect(ui->buttonOpen, SIGNAL(clicked()), SLOT(openDatabase()));
    connect(ui->buttonSave, SIGNAL(clicked()), SLOT(saveAll()));
    connect(ui->buttonAddEntry, SIGNAL(clicked()), SLOT(addEntry()));
    connect(ui->buttonCopyUsername, SIGNAL(clicked()), SLOT(copyUsername()));
    connect(ui->buttonCopyPassword, SIGNAL(clicked()), SLOT(copyPassword()));
    connect(ui->buttonDeleteEntry, SIGNAL(clicked()), SLOT(deleteRow()));

    // Setting icons to windows toolbar
    // File menu
    ui->actionNew->setIcon(*icons["createNew"]);
    ui->actionOpen->setIcon(*icons["open"]);
    ui->actionClose->setIcon(*icons["close"]);
    // Entry menu
    ui->actionCopy_User_Name->setIcon(*icons["user"]);
    ui->actionCopy_Password->setIcon(*icons["key"]);
    ui->menuUrl->setIcon(*icons["url"]);
    ui->actionOpenUrl->setIcon(*icons["openUrl"]);
    ui->actionCopyUrl->setIcon(*icons["copy"]);
    ui->actionAdd_Entry->setIcon(*icons["entry"]);
    ui->actionEdit_Entry->setIcon(*icons["edit"]);
    ui->actionDuplicate_Entry->setIcon(*icons["duplicate"]);
    ui->actionDelete_Entry->setIcon(*icons["trash"]);

    connect(ui->menuEntry, SIGNAL(aboutToShow()), SLOT(configureEntryMenu()));

    // View menu
    ui->actionChange_Language->setIcon(*icons["language"]);

    ui->menuChange_color_theme->setIcon(*icons["colorScheme"]);

    QActionGroup *colorSchemeGroup = new QActionGroup(this);

    QAction *actionSystem = new QAction("System");
    QAction *actionDark = new QAction("Dark");
    QAction *actionLight = new QAction("Light");

    colorSchemeGroup->addAction(actionSystem);
    colorSchemeGroup->addAction(actionDark);
    colorSchemeGroup->addAction(actionLight);

    connect(actionSystem, SIGNAL(triggered(bool)), SLOT(setSystemColorTheme()));
    connect(actionDark, SIGNAL(triggered(bool)), SLOT(setDarkColorTheme()));
    connect(actionLight, SIGNAL(triggered(bool)), SLOT(setLightColorTheme()));

    actionSystem->setCheckable(true);
    actionLight->setCheckable(true);
    actionDark->setCheckable(true);
    actionDark->setChecked(true);

    ui->menuChange_color_theme->addAction(actionSystem);
    ui->menuChange_color_theme->addAction(actionDark);
    ui->menuChange_color_theme->addAction(actionLight);


    ui->actionShow_Toolbar->setCheckable(true);
    ui->actionShow_Toolbar->setChecked(true);

    ui->actionConfigure_Columns->setIcon(*icons["settings"]);

    // Connecting File menu
    connect(ui->actionClose, SIGNAL(triggered()), SLOT(close()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(createDatabase()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openDatabase()));

    // Connecting Entry menu actions
    connect(ui->actionCopy_User_Name, SIGNAL(triggered()), SLOT(copyUsername()));
    connect(ui->actionCopy_Password, SIGNAL(triggered()), SLOT(copyPassword()));
    connect(ui->actionOpenUrl, SIGNAL(triggered()), SLOT(openUrl()));
    connect(ui->actionCopyUrl, SIGNAL(triggered()), SLOT(copyUrl()));
    connect(ui->actionCopy_User_Name, SIGNAL(triggered()), SLOT(copyUsername()));
    connect(ui->actionAdd_Entry, SIGNAL(triggered()), SLOT(addEntry()));
    connect(ui->actionEdit_Entry, SIGNAL(triggered()), SLOT(editRow()));
    connect(ui->actionDuplicate_Entry, SIGNAL(triggered()), SLOT(duplicateEntry()));
    connect(ui->actionDelete_Entry, SIGNAL(triggered()), SLOT(deleteRow()));
    // Connect View menu actions
    connect(ui->actionConfigure_Columns, SIGNAL(triggered()), SLOT(cfgColumns()));


    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(customMenuRequested(QPoint)));
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(customMenuRequested(QPoint)));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(itemDoubleclicked(QModelIndex)));


    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Settings cells in QTableView not editable



    if(!key.isEmpty()) // If given key from database is not empty
    {
        // loadDb() loads selected database to QSqlDatabase object (&db)
        // Path to database is loaded from QSettings
        // tables is list of tables in database, loadDb() changes this list according to loaded database
        if(!DatabaseLoader::loadDb(settings.value("Last").toString(), key, &db, tables)) // Trying to load database
        {                                                                                // If not it seems that key is not right
            QMessageBox msg;                                                             // or file is broken or something else
            msg.setText(tr("Password is uncorrect or database file is damaged\nTry again, please"));
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec(); // Showing message if can't open database
        }

        for(int i = 0; i < tables.size(); i++) // Adding tables to listWidget in screen
        {
            ui->listWidget->addItem(tables[i]);
        }
    }

    model = new QSqlTableModel(this, db); // Creating model with all tables in it
    if(tables.size() > 0) // If one or more table exists we select current first table
    {
        model->setTable(tables[0]);
        ui->listWidget->setCurrentRow(0);
    }
    model->select();
    ui->tableView->setModel(model); // Loading model to QTableView
    configureColumns();
    setIconsInListWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/// This function checks if file in given path exists
bool fileExists(const QString path)
{
    qDebug() << Q_FUNC_INFO;
    QFile file(path);
    if(file.exists())
        return true;
    return false;
}

void MainWindow::itemDoubleclicked(const QModelIndex &pos)
{
    qDebug() << Q_FUNC_INFO;

    if(pos.column() == 1)
    {
        copyText(pos.data().toString());
        ui->statusbar->showMessage(tr("Title copied"), 3000);
    }
    else if(pos.column() == 2)
    {
        copyText(pos.data().toString());
        ui->statusbar->showMessage(tr("User Name copied"), 3000);
    }
    else if(pos.column() == 3)
    {
        copyText(pos.data().toString());
        ui->statusbar->showMessage(tr("Password copied"), 3000);
    }
    else if(pos.column() == 4)
    {
        QDesktopServices::openUrl(QUrl(pos.data().toString()));
        ui->statusbar->showMessage(tr("Link opened"), 3000);
    }
    else if(pos.column() == 5)
    {
        copyText(pos.data().toString());
        ui->statusbar->showMessage(tr("Notes copied"), 3000);
    }
}

void MainWindow::customMenuRequested(QPoint pos){
    qDebug() << Q_FUNC_INFO;
    if(ui->tableView->indexAt(pos).isValid() && ui->tableView->underMouse()) // If mouse cursor is on tableView and on row
    {
        mainContextMenu.reset(new QMenu(this)); // Resetting QMenu *menu object
        actionCopyUsername.reset(new QAction(*icons["user"], tr("Copy User Name"), this)); // Resetting QAction *actionCopyUsername object
        actionCopyPassword.reset(new QAction(*icons["key"], tr("Copy Password"), this)); // Resetting QAction *actionCopyPass object
        actionEdit.reset(new QAction(*icons["edit"], tr("Edit"), this)); // Resetting QAction *actionEdit object
        actionAdd.reset(new QAction(*icons["entry"], tr("Add new"), this)); // Resetting QAction *actionAdd object
        actionDelete.reset(new QAction(*icons["trash"], tr("Delete"), this)); // Resetting QAction *actionDelete object
        actionCfgColumns.reset(new QAction(*icons["settings"], tr("Configure colums"), this));

        // Setuping list of action to url
        subMenuUrl.reset(new QMenu("URL", this));
        subMenuUrl->setIcon(*icons["url"]);
        actionCopyUrl.reset(new QAction(*icons["copy"], tr("Copy"), this));
        actionOpenUrl.reset(new QAction(*icons["openUrl"], tr("Open"), this));
        subMenuUrl->addAction(actionCopyUrl.data());
        subMenuUrl->addAction(actionOpenUrl.data());


        connect(actionCopyUsername.data(), SIGNAL(triggered()), SLOT(copyUsername()));
        connect(actionCopyPassword.data(), SIGNAL(triggered()), SLOT(copyPassword()));
        connect(actionAdd.data(), SIGNAL(triggered()), SLOT(addEntry()));
        connect(actionDelete.data(), SIGNAL(triggered()), SLOT(deleteRow())); // Connecting signal of delete button to deleteRow slot
        connect(actionEdit.data(), SIGNAL(triggered()), SLOT(editRow()));
        connect(actionCopyUrl.data(), SIGNAL(triggered()), SLOT(copyUrl()));
        connect(actionOpenUrl.data(), SIGNAL(triggered()), SLOT(openUrl()));
        connect(actionCfgColumns.data(), SIGNAL(triggered()), SLOT(cfgColumns()));


        mainContextMenu->addAction(actionCopyUsername.data());
        mainContextMenu->addAction(actionCopyPassword.data());
        mainContextMenu->addMenu(subMenuUrl.data());
        mainContextMenu->addAction(actionEdit.data());
        mainContextMenu->addAction(actionDelete.data());
        mainContextMenu->addAction(actionAdd.data());
        mainContextMenu->addAction(actionCfgColumns.data());
        mainContextMenu->popup(ui->tableView->viewport()->mapToGlobal(pos));
    }else if(ui->tableView->underMouse()) {
        mainContextMenu.reset(new QMenu(this)); // Resetting QMenu *menu object
        actionCopyUsername.reset(new QAction(*icons["user"], tr("Copy User Name"), this)); // Resetting QAction *actionCopyUsername object
        actionCopyPassword.reset(new QAction(*icons["key"], tr("Copy Password"), this)); // Resetting QAction *actionCopyPass object
        actionEdit.reset(new QAction(*icons["edit"], tr("Edit"), this)); // Resetting QAction *actionEdit object
        actionAdd.reset(new QAction(*icons["entry"], tr("Add new"), this)); // Resetting QAction *actionAdd object
        actionDelete.reset(new QAction(*icons["trash"], tr("Delete"), this)); // Resetting QAction *actionDelete object
        actionCfgColumns.reset(new QAction(*icons["settings"], tr("Configure colums"), this));
        actionCopyUsername->setDisabled(true);
        actionCopyPassword->setDisabled(true);
        actionDelete->setDisabled(true);
        actionEdit->setDisabled(true);

        subMenuUrl.reset(new QMenu(tr("URL"), this));
        subMenuUrl->setDisabled(true);

        connect(actionAdd.data(), SIGNAL(triggered()), SLOT(addEntry()));
        connect(actionCfgColumns.data(), SIGNAL(triggered()), SLOT(cfgColumns()));

        mainContextMenu->addAction(actionCopyUsername.data());
        mainContextMenu->addAction(actionCopyPassword.data());
        mainContextMenu->addMenu(subMenuUrl.data());
        mainContextMenu->addAction(actionEdit.data());
        mainContextMenu->addAction(actionDelete.data());
        mainContextMenu->addAction(actionAdd.data());
        mainContextMenu->addAction(actionCfgColumns.data());
        mainContextMenu->popup(ui->tableView->viewport()->mapToGlobal(pos));
    }else if(ui->listWidget->indexAt(pos).isValid() && ui->listWidget->underMouse()) // If mouse cursor is at listWidget and not on tableView
    {
        mainContextMenu.reset(new QMenu(this));
        actionDelete.reset(new QAction(*icons["trash"], tr("Delete"), this));
        actionAdd.reset(new QAction(*icons["add"], tr("Add Table"), this));
        actionEdit.reset(new QAction(*icons["edit"], tr("Edit"), this));

        connect(actionDelete.data(), SIGNAL(triggered()), SLOT(deleteTable()));
        connect(actionAdd.data(), SIGNAL(triggered()), SLOT(createTable()));
        connect(actionEdit.data(), SIGNAL(triggered()), SLOT(editTable()));

        mainContextMenu->addAction(actionAdd.data());
        mainContextMenu->addAction(actionEdit.data());
        mainContextMenu->addAction(actionDelete.data());
        mainContextMenu->popup(ui->listWidget->viewport()->mapToGlobal(pos));
    }else if(ui->listWidget->underMouse())
    {
        mainContextMenu.reset(new QMenu(this));
        actionDelete.reset(new QAction(*icons["trash"], tr("Delete"), this));
        actionAdd.reset(new QAction(*icons["add"], tr("Add Table"), this));
        actionEdit.reset(new QAction(*icons["edit"], tr("Edit"), this));
        actionDelete->setDisabled(true);
        actionEdit->setDisabled(true);

        connect(actionAdd.data(), SIGNAL(triggered()), SLOT(createTable()));

        mainContextMenu->addAction(actionAdd.data());
        mainContextMenu->addAction(actionEdit.data());
        mainContextMenu->addAction(actionDelete.data());
        mainContextMenu->popup(ui->listWidget->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::copyUrl()
{
    qDebug() << Q_FUNC_INFO;

    if(!hasSelectedRow()) // If user selected no row, do
        return;

    QModelIndex idx = model->index(ui->tableView->currentIndex().row(), 4); // 4 - is column of url
    copyText(idx.data().toString());
}

void MainWindow::openUrl()
{
    qDebug() << Q_FUNC_INFO;

    if(!hasSelectedRow()) // If user selected no row, do
        return;

    QModelIndex idx = model->index(ui->tableView->currentIndex().row(), 4); // 4 - is column of url
    QDesktopServices::openUrl(idx.data().toString());
}

void MainWindow::copyUsername()
{
    qDebug() << Q_FUNC_INFO;

    if(!hasSelectedRow()) // If user selected no row, do
        return;

    QSqlQuery query(db);
    QString getId = QString("SELECT id FROM %1 LIMIT 1 OFFSET %2").arg(model->tableName()).arg(ui->tableView->currentIndex().row());
    query.prepare(getId);
    if(!query.exec())
    {
        QMessageBox msg;
        msg.setText(tr("Can't get row id"));
        msg.setInformativeText(tr("Query error: ") + query.lastError().text());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }
    query.next();
    QString id = query.value(0).toString();

    query.prepare("SELECT [User Name] FROM "+model->tableName()+" WHERE id == "+id);
    if(!query.exec())
    {
        QMessageBox msg;
        msg.setText(tr("Can't get data from database"));
        msg.setInformativeText(tr("Query error: ") + query.lastError().text());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }
    query.next();

    copyText(query.value(0).toString());
}


void MainWindow::copyText(const QString text)
{
    qDebug() << Q_FUNC_INFO;
    OpenClipboard(0);
    EmptyClipboard();
    HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, 64);

    std::string str = text.toStdString();
    const char* st = str.c_str();

    strcpy_s((char*)hGlob, 64, st);
    SetClipboardData(CF_TEXT, hGlob);
    CloseClipboard();
}


void MainWindow::copyPassword()
{
    qDebug() << Q_FUNC_INFO;

    if(!hasSelectedRow()) // If user selected no row, do
        return;

    QSqlQuery query(db);
    QString getId = QString("SELECT id FROM %1 LIMIT 1 OFFSET %2").arg(model->tableName()).arg(ui->tableView->currentIndex().row());
    query.prepare(getId);
    if(!query.exec())
    {
        QMessageBox msg;
        msg.setText(tr("Can't get row id"));
        msg.setInformativeText(tr("Query error: ") + query.lastError().text());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }
    query.next();
    QString id = query.value(0).toString();

    query.prepare("SELECT [Password] FROM "+model->tableName()+" WHERE id == "+id);
    if(!query.exec())
    {
        QMessageBox msg;
        msg.setText(tr("Can't get row id"));
        msg.setInformativeText(tr("Query error: ") + query.lastError().text());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }
    query.next();

    copyText(query.value(0).toString());
}

void MainWindow::editRow()
{
    qDebug() << Q_FUNC_INFO;

    if(!hasSelectedRow()) // If user selected no row, do
        return;

    EditEntry *editEntry = new EditEntry(this, ui->tableView, &db, model);
    editEntry->exec();
    delete editEntry;
    isChanged = true;
    model->select();
    ui->tableView->update();
    configureColumns();
}

void MainWindow::deleteTable()
{
    qDebug() << Q_FUNC_INFO;
    int index = ui->listWidget->currentIndex().row();

    QMessageBox::StandardButton question = QMessageBox::question(
        this, "RadiPass", tr("Delete Table?"),
        QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes); // Creates MessageBox with buttons
    // If user clicks
    // Yes - delete row
    // No - do nothing
    if(question == QMessageBox::Yes)
    {
        if(tables.size() != 1)
        {
            if(DatabaseLoader::deleteTable(&db, ui->listWidget->item(index)->text()))
            {
                isChanged = true;
                tables.erase(tables.begin() + index);
                ui->listWidget->takeItem(index);
                model->setTable(tables[tables.size() - 1]);
                model->select();
                ui->tableView->update();
                configureColumns();
            }else
            {
                QMessageBox msg;
                msg.setText(tr("Can`t delete this table"));
                msg.button(QMessageBox::Ok);
                msg.exec();
            }
        }else {
            QMessageBox::StandardButton information = QMessageBox::information(this, "RadiPass", tr("At least one table must exists"),
                                                                               QMessageBox::Ok, QMessageBox::Ok);
        }
    }
    configureColumns();
}

void MainWindow::deleteRow()
{
    qDebug() << Q_FUNC_INFO;

    if(!hasSelectedRow()) // If user selected no row, do
        return;

    int index = ui->tableView->currentIndex().row(); // Index of selected row

    QMessageBox::StandardButton question = QMessageBox::question(
        this, "RadiPass", tr("Delete row?"),
        QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes); // Creates MessageBox with buttons
    // If user clicks
    // Yes - delete row
    // No - do nothing
    if(question == QMessageBox::Yes)
    {
        model->removeRow(index);
        model->select();
        ui->tableView->setModel(model);
        isChanged = true;
    }
}

/// If user selects another table this function executes
void MainWindow::on_listWidget_currentTextChanged(const QString &currentText)
{
    qDebug() << Q_FUNC_INFO; // Writing function names to see where error appears, all this messages shown in Application Output

    // Updating QSqlModel and QTableView to see selected table
    model->setTable(currentText);
    model->select();
    ui->tableView->setModel(model);
    configureColumns();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    if(isChanged)
    {
        QMessageBox::StandardButton question = QMessageBox::question(
            this, "RadiPass", tr("Save changes?"),
            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes); // Creates MessageBox with buttons
        if(question == QMessageBox::Yes)
        {
            DatabaseLoader::uploadDb(settings.value("Last").toString(), key, &db);
            event->accept();
        }else if(question == QMessageBox::No)
        {
            event->accept();
        }else if(question == QMessageBox::Cancel)
        {
            event->ignore();
        }
    }
}


void MainWindow::cfgColumns()
{
    ConfigureColumns *cfgColumns = new ConfigureColumns(this);
    cfgColumns->exec();
    delete cfgColumns;
    configureColumns();
}

// If user hits "Add table" button this function executes
void MainWindow::createTable()
{
    qDebug() << Q_FUNC_INFO; // Writing function names to see where error appears, all this messages shown in Application Output

    isChanged = true; // If user do some changes needs to change this state to true to ask if save changes on exit

    AddTable *addTable = new AddTable(this, &db, &tables, &icons); // This dialog will create new table in QSqlDatabase object and append new table to tables list
    addTable->exec(); // Showing dialog
    delete addTable;

    if(ui->listWidget->count() != tables.size()) // If current ListWidget size has lower value than tables this means that user added new table
    {
        ui->listWidget->clear(); // Clearing list of tables
        for(int i = 0; i < tables.size(); i++) // Adding lists to QListWidget
        {
            ui->listWidget->addItem(tables[i]);
        }
        ui->listWidget->setCurrentRow(tables.size()-1);

        // Selecting last table (this is table that user added)
        model->setTable(tables[tables.size() - 1]);
        model->select();
        ui->tableView->setModel(model);
    }
    configureColumns();
    setIconsInListWidget();
}

/// When user hits "Add data" button this function executes
void MainWindow::addEntry()
{
    qDebug() << Q_FUNC_INFO; // Writing function names to see where error appears, all this messages shown in Application Output

    isChanged = true; // If user do some changes needs to change this state to true to ask if save changes on exit

    AddPasswordDialog *dialog = new AddPasswordDialog(this, db, model->tableName()); // Create dialog that will add row to table
    dialog->exec();    // Showing dialog
    delete dialog;

    // Updating QSqlModel and QTableView to see changes
    model->setTable(model->tableName());
    model->select();
    ui->tableView->update();
    configureColumns();
}

void MainWindow::openDatabase()
{
    qDebug() << Q_FUNC_INFO;


    if(isChanged)
    {
        QMessageBox::StandardButton question = QMessageBox::question(
            this, "RadiPass", tr("Save changes?"),
            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes); // Creates MessageBox with buttons
        // If user clicks
        // Yes - must save changes
        // No - do not save changes
        // Cancel - do not close program
        if(question == QMessageBox::Yes)
        {
            qDebug() << "Clicked yes";
            DatabaseLoader::uploadDb(settings.value("Last").toString(), key, &db);
        }else if(question == QMessageBox::No)
        {
            qDebug() << "Clicked no";
        }else if(question == QMessageBox::Cancel)
        {
            qDebug() << "Clicked cancel";
            return;
        }
    }

    QString file = QFileDialog::getOpenFileName(this, tr("Open encrypted database"),
                                                QStandardPaths::writableLocation(QStandardPaths::AppDataLocation), "*.db");
    if(!file.isEmpty())
    {
        OpenDatabase *openDb = new OpenDatabase(this, &key);
        openDb->exec();
    } else return;

    if (!key.isEmpty())
    {
        if(!DatabaseLoader::loadDb(file, key, &db, tables))
        {
            QMessageBox msg;
            msg.setText(tr("Password is uncorrect"));
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
        }
        settings.setValue("Last", file);
        ui->listWidget->clear();
        for(int i = 0; i < tables.size(); i++)
        {
            ui->listWidget->addItem(tables[i]);
        }
        if(tables.size() > 0) // If one or more table exists we select current first table
        {
            model->setTable(tables[0]);
            ui->listWidget->setCurrentRow(0);
        }
        model->select();
        ui->tableView->update();
        configureColumns();
    }
    configureColumns();
    setIconsInListWidget();
}

void MainWindow::createDatabase() // This slot creates dialog to create new database
{
    qDebug() << Q_FUNC_INFO;
    QString databasePath = QFileDialog::getSaveFileName(this, tr("Create new database"),
                                                        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation), "*.db");
    if(databasePath == "") return; // If user closed window this means that datbase path is null and function execution terminates

    isChanged = true;
    CreateDatabase *createNew = new CreateDatabase(this, &key, databasePath);
    createNew->exec();
    delete createNew;

    if(key != "")
    {
        if(!DatabaseLoader::createAndFillDatabase(databasePath, key, &db))
        {
            QMessageBox msg;
            msg.setText(tr("Can't create new database"));
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
            return;
        }
        DatabaseLoader::loadDb(databasePath, key, &db, tables);
        settings.setValue("Last", databasePath);
        ui->listWidget->clear();
        for(int i = 0; i < tables.size(); i++)
        {
            ui->listWidget->addItem(tables[i]);
        }
        if(tables.size() > 0)
            model->setTable(tables[tables.size() - 1]);
        model->select();
    }
    configureColumns();
}

void MainWindow::configureEntryMenu()
{
    qDebug() << Q_FUNC_INFO;
    if(!ui->tableView->selectionModel()->hasSelection())
    {
        ui->actionCopy_User_Name->setDisabled(true);
        ui->actionCopy_Password->setDisabled(true);
        ui->menuUrl->setDisabled(true);
        ui->actionEdit_Entry->setDisabled(true);
        ui->actionDuplicate_Entry->setDisabled(true);
        ui->actionDelete_Entry->setDisabled(true);
    }else
    {
        ui->actionCopy_User_Name->setDisabled(false);
        ui->actionCopy_Password->setDisabled(false);
        ui->menuUrl->setDisabled(false);
        ui->actionEdit_Entry->setDisabled(false);
        ui->actionDuplicate_Entry->setDisabled(false);
        ui->actionDelete_Entry->setDisabled(false);
    }
}

void MainWindow::setIconsInListWidget()
{
    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        QSqlQuery query(db);
        query.prepare("SELECT Icon FROM TablesSettings WHERE [Table] = :name");
        query.bindValue(":name", ui->listWidget->item(i)->text());
        if(!query.exec())
            qDebug() << query.lastError();
        query.next();
        QString icon = query.value(0).toString();
        ui->listWidget->item(i)->setIcon(*icons[icon]);
    }
}

void MainWindow::editTable()
{
    qDebug() << Q_FUNC_INFO;

    EditTable *editTable = new EditTable(this, &db, &icons, ui->listWidget->currentItem()->text(), ui->listWidget);
    editTable->exec();
    delete editTable;
    isChanged = true;
    setIconsInListWidget();
}

void MainWindow::duplicateEntry()
{
    qDebug() << Q_FUNC_INFO;

    QSqlQuery query(db);
    QModelIndex idx = model->index(ui->tableView->currentIndex().row(), 0); // 0 column is column of id
    QString id = model->data(idx).toString();

    query.prepare("INSERT INTO "+model->tableName()+" (Title, [User Name], Password, URL, Notes) SELECT Title, [User Name], Password, URL, Notes FROM "+model->tableName()+" WHERE id = "+id);
    if(!query.exec())
    {
        QMessageBox msg;
        msg.setText(tr("Can't duplicate columns"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }
    isChanged = true;
    ui->statusbar->showMessage(tr("Entry duplicated"), 3);
    model->select();
    ui->tableView->update(); // Updating tableView
}

void MainWindow::saveAll()
{
    DatabaseLoader::uploadDb(settings.value("Last").toString(), key, &db);
}

bool MainWindow::hasSelectedRow()
{
    // Get selected rows count
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    int selected = selectionModel->selectedRows().size();


    if(selected <= 0) // If user selected no row
        return false;

    return true;
}

void MainWindow::setUkrainianLanguage()
{
    qDebug() << Q_FUNC_INFO;
    settings.setValue("Language", "uk");
    qApp->removeTranslator(translator); // remove the old translator

    // load the new translator
    QString path = QApplication::applicationDirPath();
    path.append("/Translations/");
    if(translator->load(":/translations/resources/translations/uk.qm")) //Here Path and Filename has to be entered because the system didn't find the QM Files else
        qApp->installTranslator(translator);
    else
    {
        qDebug() << "Can't load ukrainian translation";
        QMessageBox msg;
        msg.setText("Can't load ukrainian translation");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
    ui->retranslateUi(this);
}

void MainWindow::setEnglishLanguage()
{
    qDebug() << Q_FUNC_INFO;
    settings.setValue("Language", "en");
    qApp->removeTranslator(translator); // Just reset language, english language is native for this program
    ui->retranslateUi(this);
}

void MainWindow::setSystemColorTheme()
{
    qDebug() << Q_FUNC_INFO;
}

void MainWindow::setDarkColorTheme()
{
    qDebug() << Q_FUNC_INFO;

    QFile  styleFile(":/themes/resources/themes/dark.qss");
    styleFile.open(QFile::ReadOnly);

    QString  style(styleFile.readAll());
    styleFile.close();

    qApp->setStyleSheet(style);
}

void MainWindow::setLightColorTheme()
{
    qDebug() << Q_FUNC_INFO;

    QFile  styleFile(":/themes/resources/themes/light.qss");
    styleFile.open(QFile::ReadOnly);

    QString  style(styleFile.readAll());
    styleFile.close();

    qApp->setStyleSheet(style);
}
