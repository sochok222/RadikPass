#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <qstylehints.h>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent, QByteArray MasterKey, QTranslator *translator, QString theme)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , key(MasterKey)
    , translator(translator)
    , theme(theme)
{
    qDebug() << Q_FUNC_INFO;
    ui->setupUi(this);
    setWindowTitle("RadiPass");

    // Shortcuts initialization
    setupShortcuts();

    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(ui->actionLanguageUkrainian, SIGNAL(triggered(bool)), SLOT(setUkrainianLanguage()));
    connect(ui->actionLanguageEnglish, SIGNAL(triggered(bool)), SLOT(setEnglishLanguage()));
    connect(ui->actionLanguageGerman, SIGNAL(triggered(bool)), SLOT(setGermanLanguage()));

    setColorThemeActions();

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);


    // Setting hints to toolbar
    ui->buttonNew->setToolTip(tr("New Database"));
    ui->buttonOpen->setToolTip(tr("Open Database"));
    ui->buttonSave->setToolTip(tr("Save"));
    ui->buttonAddEntry->setToolTip(tr("Add Entry"));
    ui->buttonCopyUsername->setToolTip(tr("Copy Username"));
    ui->buttonCopyPassword->setToolTip(tr("Copy Password"));
    ui->buttonDeleteEntry->setToolTip(tr("Delete Entry"));

    // Connecting toolbar buttons to slots
    connect(ui->buttonNew, SIGNAL(clicked()), SLOT(createDatabase()));
    connect(ui->buttonOpen, SIGNAL(clicked()), SLOT(openDatabase()));
    connect(ui->buttonSave, SIGNAL(clicked()), SLOT(saveAll()));
    connect(ui->buttonAddEntry, SIGNAL(clicked()), SLOT(addEntry()));
    connect(ui->buttonCopyUsername, SIGNAL(clicked()), SLOT(copyUsername()));
    connect(ui->buttonCopyPassword, SIGNAL(clicked()), SLOT(copyPassword()));
    connect(ui->buttonDeleteEntry, SIGNAL(clicked()), SLOT(deleteEntry()));
    connect(ui->menuEntry, SIGNAL(aboutToShow()), SLOT(configureEntryMenu()));

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
    connect(ui->actionDelete_Entry, SIGNAL(triggered()), SLOT(deleteEntry()));
    // Connect View menu actions
    connect(ui->actionConfigure_Columns, SIGNAL(triggered()), SLOT(cfgColumns()));


    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(customMenuRequested(QPoint)));
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(customMenuRequested(QPoint)));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(itemDoubleclicked(QModelIndex)));

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Settings cells in QTableView not editable


    loadIcons();


    if(!key.isEmpty()) // If given key from database is not empty
    {
        // loadDb() loads selected database to QSqlDatabase object (&db)
        // Path to database is loaded from QSettings
        // tables is list of tables in database, loadDb() changes this list according to loaded database
        if(!DbManager::loadDb(settings.value("Last").toString(), key, &db, tables)) // Trying to load database
        {                                                                                // If not it seems that key is not right
            QMessageBox msg;                                                             // or file is broken or something else
            msg.setText(tr("Password is incorrect or database file is damaged\nTry again, please"));
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec(); // Showing message if can't open database
            key = 0;
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
    configureColumns(); // Showing columns according to settings.
    setIconsInListWidget();
}
MainWindow::~MainWindow()
{
    delete ui;
}

void::MainWindow::setupShortcuts()
{
    // Creating shortcuts.
    shortcutOpen.reset(new QShortcut(QKeySequence::Open, this));
    shortcutClose.reset(new QShortcut(QKeySequence::Close, this));
    shortcutDuplicate.reset(new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), this));
    shortcutDelete.reset(new QShortcut(QKeySequence::Delete, this));
    shortcutNewDatabase.reset(new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N), this));
    shortcutAddEntry.reset(new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_N), this));

    // Connecting shortcuts to slots.
    connect(shortcutOpen.data(), SIGNAL(activated()), SLOT(openDatabase()));
    connect(shortcutClose.data(), SIGNAL(activated()), this, SLOT(close()));
    connect(shortcutDuplicate.data(), SIGNAL(activated()), this, SLOT(duplicateEntry()));
    connect(shortcutDelete.data(), SIGNAL(activated()), this, SLOT(deleteEntry()));
    connect(shortcutNewDatabase.data(), SIGNAL(activated()), this, SLOT(createDatabase()));
    connect(shortcutAddEntry.data(), SIGNAL(activated()), this, SLOT(addEntry()));
}

void MainWindow::setColorThemeActions()
{
    actionSystem.reset(new QAction(tr("System")));
    actionDark.reset(new QAction(tr("Dark")));
    actionLight.reset(new QAction(tr("Light")));

    colorThemeGroup.reset(new QActionGroup(this));
    colorThemeGroup->addAction(actionSystem.data());
    colorThemeGroup->addAction(actionDark.data());
    colorThemeGroup->addAction(actionLight.data());

    connect(actionSystem.data(), SIGNAL(triggered(bool)), SLOT(setSystemColorTheme()));
    connect(actionDark.data(), SIGNAL(triggered(bool)), SLOT(setDarkColorTheme()));
    connect(actionLight.data(), SIGNAL(triggered(bool)), SLOT(setLightColorTheme()));

    actionSystem->setCheckable(true);
    actionLight->setCheckable(true);
    actionDark->setCheckable(true);
    if(settings.value("theme") == "system") actionSystem->setChecked(true);
    else if(settings.value("theme") == "dark") actionDark->setChecked(true);
    else if(settings.value("theme") == "light") actionLight->setChecked(true);

    ui->menuChange_color_theme->addAction(actionSystem.data());
    ui->menuChange_color_theme->addAction(actionDark.data());
    ui->menuChange_color_theme->addAction(actionLight.data());
}


void MainWindow::loadIcons()
{
    // Setting icons to toolbar
    ui->buttonOpen->setIcon(IconLoader::getIcon(Icon::open, theme));
    ui->buttonNew->setIcon(IconLoader::getIcon(Icon::create, theme));
    ui->buttonSave->setIcon(IconLoader::getIcon(Icon::save, theme));
    ui->buttonAddEntry->setIcon(IconLoader::getIcon(Icon::add, theme));
    ui->buttonCopyUsername->setIcon(IconLoader::getIcon(Icon::user, theme));
    ui->buttonCopyPassword->setIcon(IconLoader::getIcon(Icon::key, theme));
    ui->buttonDeleteEntry->setIcon(IconLoader::getIcon(Icon::trash, theme));


    // Setting icons to windows toolbar
    // File menu
    ui->actionNew->setIcon(IconLoader::getIcon(Icon::create, theme));
    ui->actionOpen->setIcon(IconLoader::getIcon(Icon::open, theme));
    ui->actionClose->setIcon(IconLoader::getIcon(Icon::close, theme));
    // Entry menu
    ui->actionCopy_User_Name->setIcon(IconLoader::getIcon(Icon::user, theme));
    ui->actionCopy_Password->setIcon(IconLoader::getIcon(Icon::key, theme));
    ui->menuUrl->setIcon(IconLoader::getIcon(Icon::link, theme));
    ui->actionOpenUrl->setIcon(IconLoader::getIcon(Icon::openBrowser, theme));
    ui->actionCopyUrl->setIcon(IconLoader::getIcon(Icon::copy, theme));
    ui->actionAdd_Entry->setIcon(IconLoader::getIcon(Icon::entry, theme));
    ui->actionEdit_Entry->setIcon(IconLoader::getIcon(Icon::edit, theme));
    ui->actionDuplicate_Entry->setIcon(IconLoader::getIcon(Icon::duplicate, theme));
    ui->actionDelete_Entry->setIcon(IconLoader::getIcon(Icon::trash, theme));

    // View menu
    ui->actionChange_Language->setIcon(IconLoader::getIcon(Icon::language, theme));
    ui->actionConfigure_Columns->setIcon(IconLoader::getIcon(Icon::settings, theme));
    ui->menuChange_color_theme->setIcon(IconLoader::getIcon(Icon::color, theme));
}


void MainWindow::configureColumns() // Showing or hiding columns in tableView according to settings
{
    // Bassically ui->tableView->setColumnHidden is false, so I don't need to set it to false if column is need to be shown
    QStringList columnTitle = settings.value("columnTitle").toStringList();
    QStringList columnUsername = settings.value("columnUsername").toStringList();
    QStringList columnPassword = settings.value("columnPassword").toStringList();
    QStringList columnURL = settings.value("columnURL").toStringList();
    QStringList columnNotes = settings.value("columnNotes").toStringList();
    QStringList columnCreationTime = settings.value("columnCreationTime").toStringList();
    QStringList columnLastChanged = settings.value("columnLastChanged").toStringList();

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

    if(columnCreationTime.value(0) == "shown")
        ui->tableView->setColumnHidden(6, false);
    else ui->tableView->setColumnHidden(6, true);
    if(columnCreationTime.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(6, 0);
    else ui->tableView->setItemDelegateForColumn(6, maskColumn);

    if(columnLastChanged.value(0) == "shown")
        ui->tableView->setColumnHidden(7, false);
    else ui->tableView->setColumnHidden(7, true);
    if(columnLastChanged.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(7, 0);
    else ui->tableView->setItemDelegateForColumn(7, maskColumn);
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
        ui->statusbar->showMessage(tr("Username copied"), 3000);
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

    if(ui->listWidget->count() == 0) // Checking if at leas one row in listWidget exists.
        return;


    if(ui->tableView->indexAt(pos).isValid() && ui->tableView->underMouse()) // If mouse cursor is on tableView and on row
    {
        mainContextMenu.reset(new QMenu(this)); // Resetting QMenuenu object
        actionCopyUsername.reset(new QAction(IconLoader::getIcon(Icon::user, theme), tr("Copy User Name"), this)); // Resetting QAction *actionCopyUsername object
        actionCopyPassword.reset(new QAction(IconLoader::getIcon(Icon::key, theme), tr("Copy Password"), this)); // Resetting QAction *actionCopyPass object
        actionEdit.reset(new QAction(IconLoader::getIcon(Icon::edit, theme), tr("Edit"), this)); // Resetting QAction *actionEdit object
        actionAdd.reset(new QAction(IconLoader::getIcon(Icon::entry, theme), tr("Add new"), this)); // Resetting QAction *actionAdd object
        actionDelete.reset(new QAction(IconLoader::getIcon(Icon::trash, theme), tr("Delete"), this)); // Resetting QAction *actionDelete object
        actionCfgColumns.reset(new QAction(IconLoader::getIcon(Icon::settings, theme), tr("Configure colums"), this));

        // Setuping list of action to url
        subMenuUrl.reset(new QMenu(tr("URL"), this));
        subMenuUrl->setIcon(IconLoader::getIcon(Icon::link, theme));
        actionCopyUrl.reset(new QAction(IconLoader::getIcon(Icon::copy, theme), tr("Copy"), this));
        actionOpenUrl.reset(new QAction(IconLoader::getIcon(Icon::openBrowser, theme), tr("Open"), this));
        subMenuUrl->addAction(actionCopyUrl.data());
        subMenuUrl->addAction(actionOpenUrl.data());


        connect(actionCopyUsername.data(), SIGNAL(triggered()), SLOT(copyUsername()));
        connect(actionCopyPassword.data(), SIGNAL(triggered()), SLOT(copyPassword()));
        connect(actionAdd.data(), SIGNAL(triggered()), SLOT(addEntry()));
        connect(actionDelete.data(), SIGNAL(triggered()), SLOT(deleteEntry())); // Connecting signal of delete button to deleteRow slot
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
        actionCopyUsername.reset(new QAction(IconLoader::getIcon(Icon::user, theme), tr("Copy User Name"), this)); // Resetting QAction *actionCopyUsername object
        actionCopyPassword.reset(new QAction(IconLoader::getIcon(Icon::key, theme), tr("Copy Password"), this)); // Resetting QAction *actionCopyPass object
        actionEdit.reset(new QAction(IconLoader::getIcon(Icon::edit, theme), tr("Edit"), this)); // Resetting QAction *actionEdit object
        actionAdd.reset(new QAction(IconLoader::getIcon(Icon::add, theme), tr("Add new"), this)); // Resetting QAction *actionAdd object
        actionDelete.reset(new QAction(IconLoader::getIcon(Icon::trash, theme), tr("Delete"), this)); // Resetting QAction *actionDelete object
        actionCfgColumns.reset(new QAction(IconLoader::getIcon(Icon::settings, theme), tr("Configure colums"), this));
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
        actionDelete.reset(new QAction(IconLoader::getIcon(Icon::trash, theme), tr("Delete"), this));
        actionAdd.reset(new QAction(IconLoader::getIcon(Icon::add, theme), tr("Add Table"), this));
        actionEdit.reset(new QAction(IconLoader::getIcon(Icon::edit, theme), tr("Edit"), this));

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
        actionDelete.reset(new QAction(IconLoader::getIcon(Icon::trash, theme), tr("Delete"), this));
        actionAdd.reset(new QAction(IconLoader::getIcon(Icon::add, theme), tr("Add Table"), this));
        actionEdit.reset(new QAction(IconLoader::getIcon(Icon::edit, theme), tr("Edit"), this));
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

    ui->statusbar->showMessage(tr("Username copied."), 30000);
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

    ui->statusbar->showMessage(tr("Password copied."), 3000);
}

void MainWindow::editRow()
{
    qDebug() << Q_FUNC_INFO;

    if(!hasSelectedRow()) // If user has no selected row return.
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
            if(DbManager::deleteTable(&db, ui->listWidget->item(index)->text()))
            {
                isChanged = true;
                tables.erase(tables.begin() + index);
                ui->listWidget->takeItem(index);
                model->setTable(tables[tables.size() - 1]);
                model->select();
                ui->tableView->update();
                configureColumns();
                ui->statusbar->showMessage(tr("Table deleted."), 3000);
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

void MainWindow::deleteEntry()
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
        ui->statusbar->showMessage(tr("Entry deleted."), 30000);
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
            DbManager::uploadDb(settings.value("Last").toString(), key, &db);
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

    AddTable *addTable = new AddTable(this, &db, &tables, theme); // This dialog will create new table in QSqlDatabase object and append new table to tables list
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

    AddEntry *dialog = new AddEntry(this, &db, model->tableName()); // Create dialog that will add row to table
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
            DbManager::uploadDb(settings.value("Last").toString(), key, &db);
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
        if(!DbManager::loadDb(file, key, &db, tables))
        {
            QMessageBox msg;
            msg.setText(tr("Password is incorrect"));
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

// When user clicks button that must create new database this func will be called
void MainWindow::createDatabase()
{
    qDebug() << Q_FUNC_INFO;
    QString databasePath = QFileDialog::getSaveFileName(this, tr("Create new database"),
                                            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation), "*.db"); // Asking user to chose path and name of new database
    if(databasePath == "") return; // If user closed window this means that datbase path is null and function execution terminates

    CreateDatabase *createNew = new CreateDatabase(this, &key, databasePath); // Creating window where user can set password to new database
    createNew->exec();
    delete createNew;

    if(key != "") // If key is empty this means that user closed createNew dialog
    {
        if(!DbManager::createAndFillDatabase(databasePath, key, &db)) // This must create new .db file and encrypt it
        {
            QMessageBox msg;
            msg.setText(tr("Can't create new database"));
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
            return;
        }

        DbManager::loadDb(databasePath, key, &db, tables); // Loads database to to the QSqlDatabase instance
        settings.setValue("Last", databasePath); // Setting last used database path to recently created

        ui->listWidget->clear(); // Clears listWidget that holds all tables

        for(int i = 0; i < tables.size(); i++) // Loading all tables that avaible to user from tables vector
        {
            ui->listWidget->addItem(tables[i]);
        }

        if(tables.size() > 0)
            ui->listWidget->setCurrentRow(0); // Select first item in listWidget, item changes connected to slot on_listWidget_currentTextChanged();
    }

    configureColumns(); // Showing columns according to settings.
    setIconsInListWidget(); // Loading icons to ListWidget with tables.
}

void MainWindow::configureEntryMenu() // This function will disable or enable actions in View menu in toolbar
{
    qDebug() << Q_FUNC_INFO;


    if(ui->tableView->selectionModel()->hasSelection()) // If row is not selected in TableView this will disable actions and vise versa
    {
        ui->actionCopy_User_Name->setEnabled(true);
        ui->actionCopy_Password->setEnabled(true);
        ui->menuUrl->setEnabled(true);
        ui->actionEdit_Entry->setEnabled(true);
        ui->actionDuplicate_Entry->setEnabled(true);
        ui->actionDelete_Entry->setEnabled(true);
    }else if (ui->listWidget->count() > 0) // Checking if at least one table exists.
    {
        ui->actionCopy_User_Name->setEnabled(false);
        ui->actionCopy_Password->setEnabled(false);
        ui->menuUrl->setEnabled(false);
        ui->actionEdit_Entry->setEnabled(false);
        ui->actionDuplicate_Entry->setEnabled(false);
        ui->actionDelete_Entry->setEnabled(false);
        ui->actionAdd_Entry->setEnabled(true);
    } else {
        ui->actionCopy_User_Name->setEnabled(false);
        ui->actionCopy_Password->setEnabled(false);
        ui->menuUrl->setEnabled(false);
        ui->actionEdit_Entry->setEnabled(false);
        ui->actionDuplicate_Entry->setEnabled(false);
        ui->actionDelete_Entry->setEnabled(false);
        ui->actionAdd_Entry->setEnabled(false);
    }
}

void MainWindow::setIconsInListWidget() // This will load icons to ListWidget,
{
    QSqlQuery query(db); // New query to read data from database
    for(int i = 0; i < ui->listWidget->count(); i++) // For every item in ListWidget(table)
    {
        query.prepare("SELECT Icon FROM TablesSettings WHERE [Table] = :name"); // Loading name of icon from TablesSettings
        query.bindValue(":name", ui->listWidget->item(i)->text());
        if(!query.exec())
        {
            qDebug() << query.lastError(); // TODO: handle this in QMessageBox to show user error
            break;
        }

        query.next();
        ui->listWidget->item(i)->setIcon(QIcon(":/icons/"+theme+"/resources/icons/"+theme+"/"+query.value(0).toString()+".png")); // Set icon in ListWidget row
    }
}

// When user wants to edit row in table
void MainWindow::editTable()
{
    qDebug() << Q_FUNC_INFO;

    EditTable *editTable = new EditTable(this, &db, ui->listWidget->currentItem()->text(), ui->listWidget, theme); // Creating EditTable object that will ask user to make some changes in row
    editTable->exec();
    delete editTable;

    // Selecting in table model table that user was editing.
    model->setTable(ui->listWidget->currentItem()->text());
    model->select();

    isChanged = true; // If user makes some changes needs to set this to true

    setIconsInListWidget(); // Loading icons to ListWidget with tables
    configureColumns(); // Showing columns according to settings.
}

void MainWindow::duplicateEntry()
{
    qDebug() << Q_FUNC_INFO;

    if(!hasSelectedRow())
        return;

    QSqlQuery query(db);
    QString id = model->data(model->index(ui->tableView->currentIndex().row(), 0)).toString(); // id - this is id of row which needs to be duplicated

    query.prepare("INSERT INTO "+model->tableName()+" (Title, [User Name], Password, URL, Notes, [Creation Time], [Last Changed]) SELECT Title, [User Name], Password, URL, Notes, [Creation Time], [Last Changed] FROM "+model->tableName()+" WHERE id = "+id);
    if(!query.exec())
    {
        QMessageBox msg;
        msg.setText(tr("Can't duplicate entry"));
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
    DbManager::uploadDb(settings.value("Last").toString(), key, &db);
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

    if(translator->load(":/translations/resources/translations/uk.qm"))
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
    setColorThemeActions();
}

void MainWindow::setEnglishLanguage()
{
    qDebug() << Q_FUNC_INFO;
    settings.setValue("Language", "en");
    qApp->removeTranslator(translator);
    if(translator->load(":/translations/resources/translations/en.qm"))
        qApp->installTranslator(translator);
    else
    {
        qDebug() << "Can't load english translation";
        QMessageBox msg;
        msg.setText("Can't load english translation");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
    ui->retranslateUi(this);
    setColorThemeActions();
}

void MainWindow::setGermanLanguage()
{
    qDebug() << Q_FUNC_INFO;
    settings.setValue("Language", "ge");
    qApp->removeTranslator(translator);
    if(translator->load(":/translations/resources/translations/ge.qm"))
        qApp->installTranslator(translator);
    else
    {
        qDebug() << "Can't load german translation";
        QMessageBox msg;
        msg.setText("Can't load german translation");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
    ui->retranslateUi(this);
    setColorThemeActions();
}

void MainWindow::setSystemColorTheme()
{
    qDebug() << Q_FUNC_INFO;

    if(QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    {
        QFile  styleFile(":/themes/resources/themes/dark.qss");
        styleFile.open(QFile::ReadOnly);

        this->theme = "dark";

        QString  style(styleFile.readAll());
        styleFile.close();

        qApp->setStyleSheet(style);
    }else {
        QFile  styleFile(":/themes/resources/themes/light.qss");
        styleFile.open(QFile::ReadOnly);

        this->theme = "light";

        QString  style(styleFile.readAll());
        styleFile.close();

        qApp->setStyleSheet(style);
    }
    settings.setValue("theme", "system");
    setIconsInListWidget();
    loadIcons();
}

void MainWindow::setDarkColorTheme()
{
    qDebug() << Q_FUNC_INFO;

    QFile  styleFile(":/themes/resources/themes/dark.qss");
    styleFile.open(QFile::ReadOnly);

    this->theme = "dark";
    settings.setValue("theme", "dark");

    QString  style(styleFile.readAll());
    styleFile.close();

    qApp->setStyleSheet(style);

    setIconsInListWidget();
    loadIcons();
}

void MainWindow::setLightColorTheme()
{
    qDebug() << Q_FUNC_INFO;

    QFile  styleFile(":/themes/resources/themes/light.qss");
    styleFile.open(QFile::ReadOnly);

    this->theme = "light";
    settings.setValue("theme", "light");

    QString  style(styleFile.readAll());
    styleFile.close();

    qApp->setStyleSheet(style);

    setIconsInListWidget();
    loadIcons();
}
