#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include <qstylehints.h>

MainWindow::MainWindow(QWidget *parent, QByteArray MasterKey, QTranslator *translator, Theme theme)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , key(MasterKey)
    , translator(translator)
    , theme(theme)
{
    qInfo() << Q_FUNC_INFO;
    ui->setupUi(this);

    // Binding shortcuts
    setShortcuts();

    // Setting tooltips
    setTooltips();

    // Connecting buttons and actions to slots
    connectButtons();
    connectActions();

    // Setting search bar
    ui->searchBar->setClearButtonEnabled(true);
    ui->searchBar->setPlaceholderText("Search...");

    // Setting listWidget
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu); // Enabling context menu
    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection); // User can select only one row
    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel); // Enabling smooth scrolling

    // Setting tableView
    ui->tableView->verticalHeader()->setVisible(false); // Disabling vertical headers
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu); // Enabling context menu
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // User can select only rows
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); // User can select only single row
    ui->tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel); // Enabling smooth horizontal scrolling
    ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel); // Enabling smooth vertical scrolling
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Disabling cell editing

    // Connecting tableView to slots
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequested(QPoint))); // Context menu slot
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), SLOT(itemDoubleclicked(QModelIndex))); // Doubleclicked slot

    // Connecting listWidget to context menu slot
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequested(QPoint)));

    if (!key.isEmpty()) { // If given key from database is not empty
        // loadDb() loads selected database to QSqlDatabase object (&db)
        // Path to database is loaded from QSettings
        // tables is list of tables in database, loadDb() changes this list according to loaded database
        if (!DbManager::loadDb(settings.value("Last").toString(), key, &db, tables)) { // Trying to load database
            showMsgBox(tr("Error"), tr("Password is incorrect or database file is damaged\nTry again, please"), QMessageBox::Critical);
            key = 0;
        }

        for(int i = 0; i < tables.size(); i++) { // Adding tables to listWidget in screen
            ui->listWidget->addItem(tables[i]);
        }
    }

    model = new QSqlTableModel(this, db); // Creating model with all tables in it

    // Connecting model update signal to slots that will retranslate headers and configure columns
    connect(model, SIGNAL(modelReset()), SLOT(setHeaders()));
    connect(model, SIGNAL(modelReset()), SLOT(configureColumns()));

    if (tables.size() > 0) { // Loading first table if one or more exists
        model->setTable(tables[0]);
        ui->listWidget->setCurrentRow(0);
    }

    model->select();
    ui->tableView->setModel(model); // Loading model to QTableView

    configureColumns(); // Showing columns according to settings.

    loadIcons(); // Loading icons according to current color theme
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::showMsgBox(const QString &title, const QString &text, const QMessageBox::Icon &icon) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIcon(icon);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}


void MainWindow::openPasswordGenerator() {
    PasswordGenerator *passwordGenerator = new PasswordGenerator(this, 0);
    passwordGenerator->exec();
    delete passwordGenerator;
}


void MainWindow::setHeaders() {
    QVector<QString> headers = {tr("Title"), tr("User Name"),
                                tr("Password"), tr("URL"),
                                tr("Notes"), tr("Creation Time"), tr("Last Changed")};

    for(int i = 0; i < headers.size(); i++) {
        model->setHeaderData(i+1, Qt::Horizontal, headers[i]); // Loading text to header of column in tableView
    }
}

void MainWindow::connectActions() {
    // File menu actions
    connect(ui->actionClose, SIGNAL(triggered()), SLOT(close()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(createDatabase()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openDatabase()));

    // Entry menu actions
    connect(ui->actionCopy_User_Name, SIGNAL(triggered()), SLOT(copyUsername()));
    connect(ui->actionCopy_Password, SIGNAL(triggered()), SLOT(copyPassword()));
    connect(ui->actionOpenUrl, SIGNAL(triggered()), SLOT(openUrl()));
    connect(ui->actionCopyUrl, SIGNAL(triggered()), SLOT(copyUrl()));
    connect(ui->actionCopy_User_Name, SIGNAL(triggered()), SLOT(copyUsername()));
    connect(ui->actionAdd_Entry, SIGNAL(triggered()), SLOT(addEntry()));
    connect(ui->actionEdit_Entry, SIGNAL(triggered()), SLOT(editRow()));
    connect(ui->actionDuplicate_Entry, SIGNAL(triggered()), SLOT(duplicateEntry()));
    connect(ui->actionDelete_Entry, SIGNAL(triggered()), SLOT(deleteEntry()));

    // View menu actions
    connect(ui->actionConfigure_Columns, SIGNAL(triggered()), SLOT(cfgColumns()));

    // action opens PasswordGenerator
    connect(ui->actionPassword_Generator, SIGNAL(triggered()), this, SLOT(openPasswordGenerator()));

    // Language menu actions
    connect(ui->actionLanguageUkrainian, SIGNAL(triggered(bool)), SLOT(setUkrainianLanguage()));
    connect(ui->actionLanguageEnglish, SIGNAL(triggered(bool)), SLOT(setEnglishLanguage()));
    connect(ui->actionLanguageGerman, SIGNAL(triggered(bool)), SLOT(setGermanLanguage()));

    setColorThemeActions(); // setting actions in "Color Theme" menu
}

void MainWindow::connectButtons() {
    // Connecting buttons to slots
    connect(ui->buttonNew, SIGNAL(clicked()), SLOT(createDatabase()));
    connect(ui->buttonOpen, SIGNAL(clicked()), SLOT(openDatabase()));
    connect(ui->buttonSave, SIGNAL(clicked()), SLOT(saveAll()));
    connect(ui->buttonAddEntry, SIGNAL(clicked()), SLOT(addEntry()));
    connect(ui->buttonCopyUsername, SIGNAL(clicked()), SLOT(copyUsername()));
    connect(ui->buttonCopyPassword, SIGNAL(clicked()), SLOT(copyPassword()));
    connect(ui->buttonDeleteEntry, SIGNAL(clicked()), SLOT(deleteEntry()));
    connect(ui->menuEntry, SIGNAL(aboutToShow()), SLOT(configureEntryMenu()));
}

void MainWindow::setTooltips() {
    // Setting hints to toolbar
    ui->buttonNew->setToolTip(tr("New Database"));
    ui->buttonOpen->setToolTip(tr("Open Database"));
    ui->buttonSave->setToolTip(tr("Save"));
    ui->buttonAddEntry->setToolTip(tr("Add Entry"));
    ui->buttonCopyUsername->setToolTip(tr("Copy Username"));
    ui->buttonCopyPassword->setToolTip(tr("Copy Password"));
    ui->buttonDeleteEntry->setToolTip(tr("Delete Entry"));
}

void MainWindow::setShortcuts() {
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

void MainWindow::setColorThemeActions() {
    // Resetting actions
    action_systemTheme.reset(new QAction(tr("System")));
    action_darkTheme.reset(new QAction(tr("Dark")));
    action_lightTheme.reset(new QAction(tr("Light")));

    // Resetting group for actions and adding actions to it
    group_colorThemes.reset(new QActionGroup(this));
    group_colorThemes->addAction(action_systemTheme.data());
    group_colorThemes->addAction(action_darkTheme.data());
    group_colorThemes->addAction(action_lightTheme.data());

    // Connecting actions to slots
    connect(action_systemTheme.data(), SIGNAL(triggered(bool)), SLOT(setSystemColorTheme()));
    connect(action_darkTheme.data(), SIGNAL(triggered(bool)), SLOT(setDarkColorTheme()));
    connect(action_lightTheme.data(), SIGNAL(triggered(bool)), SLOT(setLightColorTheme()));

    // Setting actions as checkable
    action_systemTheme->setCheckable(true);
    action_lightTheme->setCheckable(true);
    action_darkTheme->setCheckable(true);

    // Setting checked action according to current theme
    if (settings.value("theme") == "system") action_systemTheme->setChecked(true);
    else if (settings.value("theme") == "dark") action_darkTheme->setChecked(true);
    else if (settings.value("theme") == "light") action_lightTheme->setChecked(true);

    // Loading actions to toolbar
    ui->menuChange_color_theme->addAction(action_systemTheme.data());
    ui->menuChange_color_theme->addAction(action_darkTheme.data());
    ui->menuChange_color_theme->addAction(action_lightTheme.data());
}


void MainWindow::loadIcons() {
    loadIconsToListWidget();

    // Loading icons to buttons
    ui->buttonOpen->setIcon(IconLoader::getIcon(Icon::Open, theme));
    ui->buttonNew->setIcon(IconLoader::getIcon(Icon::Create, theme));
    ui->buttonSave->setIcon(IconLoader::getIcon(Icon::Save, theme));
    ui->buttonAddEntry->setIcon(IconLoader::getIcon(Icon::Add, theme));
    ui->buttonCopyUsername->setIcon(IconLoader::getIcon(Icon::User, theme));
    ui->buttonCopyPassword->setIcon(IconLoader::getIcon(Icon::Key, theme));
    ui->buttonDeleteEntry->setIcon(IconLoader::getIcon(Icon::Trash, theme));

    // Loading icons to windows toolbar
    // File menu
    ui->actionNew->setIcon(IconLoader::getIcon(Icon::Create, theme));
    ui->actionOpen->setIcon(IconLoader::getIcon(Icon::Open, theme));
    ui->actionClose->setIcon(IconLoader::getIcon(Icon::Close, theme));
    // Entry menu
    ui->actionCopy_User_Name->setIcon(IconLoader::getIcon(Icon::User, theme));
    ui->actionCopy_Password->setIcon(IconLoader::getIcon(Icon::Key, theme));
    ui->menuUrl->setIcon(IconLoader::getIcon(Icon::Link, theme));
    ui->actionOpenUrl->setIcon(IconLoader::getIcon(Icon::OpenBrowser, theme));
    ui->actionCopyUrl->setIcon(IconLoader::getIcon(Icon::Copy, theme));
    ui->actionAdd_Entry->setIcon(IconLoader::getIcon(Icon::Entry, theme));
    ui->actionEdit_Entry->setIcon(IconLoader::getIcon(Icon::Edit, theme));
    ui->actionDuplicate_Entry->setIcon(IconLoader::getIcon(Icon::Duplicate, theme));
    ui->actionDelete_Entry->setIcon(IconLoader::getIcon(Icon::Trash, theme));
    // View menu
    ui->actionChange_Language->setIcon(IconLoader::getIcon(Icon::Language, theme));
    ui->actionConfigure_Columns->setIcon(IconLoader::getIcon(Icon::Settings, theme));
    ui->menuChange_color_theme->setIcon(IconLoader::getIcon(Icon::Color, theme));
}


void MainWindow::configureColumns() {
    // Loading settings
    QStringList columnTitle = settings.value("columnTitle").toStringList();
    QStringList columnUsername = settings.value("columnUsername").toStringList();
    QStringList columnPassword = settings.value("columnPassword").toStringList();
    QStringList columnURL = settings.value("columnURL").toStringList();
    QStringList columnNotes = settings.value("columnNotes").toStringList();
    QStringList columnCreationTime = settings.value("columnCreationTime").toStringList();
    QStringList columnLastChanged = settings.value("columnLastChanged").toStringList();

    ui->tableView->setColumnHidden(0,true); // Hiding column with id of row

    // Showing/hiding and masking/unmasing columns according to settings
    if (columnTitle.value(0) == "shown")
        ui->tableView->setColumnHidden(1, false);
    else ui->tableView->setColumnHidden(1, true);
    if (columnTitle.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(1, 0);
    else ui->tableView->setItemDelegateForColumn(1, maskColumn);

    if (columnUsername.value(0) == "shown")
        ui->tableView->setColumnHidden(2, false);
    else ui->tableView->setColumnHidden(2, true);
    if (columnUsername.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(2, 0);
    else ui->tableView->setItemDelegateForColumn(2, maskColumn);

    if (columnPassword.value(0) == "shown")
        ui->tableView->setColumnHidden(3, false);
    else ui->tableView->setColumnHidden(3, true);
    if (columnPassword.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(3, 0);
    else ui->tableView->setItemDelegateForColumn(3, maskColumn);

    if (columnURL.value(0) == "shown")
        ui->tableView->setColumnHidden(4, false);
    else ui->tableView->setColumnHidden(4, true);
    if (columnURL.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(4, 0);
    else ui->tableView->setItemDelegateForColumn(4, maskColumn);

    if (columnNotes.value(0) == "shown")
        ui->tableView->setColumnHidden(5, false);
    else ui->tableView->setColumnHidden(5, true);
    if (columnNotes.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(5, 0);
    else ui->tableView->setItemDelegateForColumn(5, maskColumn);

    if (columnCreationTime.value(0) == "shown")
        ui->tableView->setColumnHidden(6, false);
    else ui->tableView->setColumnHidden(6, true);
    if (columnCreationTime.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(6, 0);
    else ui->tableView->setItemDelegateForColumn(6, maskColumn);

    if (columnLastChanged.value(0) == "shown")
        ui->tableView->setColumnHidden(7, false);
    else ui->tableView->setColumnHidden(7, true);
    if (columnLastChanged.value(1) == "unmasked")
        ui->tableView->setItemDelegateForColumn(7, 0);
    else ui->tableView->setItemDelegateForColumn(7, maskColumn);
}

bool fileExists(const QString path)
{
    qInfo() << Q_FUNC_INFO;
    QFile file(path);
    if (file.exists())
        return true;
    return false;
}

void MainWindow::itemDoubleclicked(const QModelIndex &pos)
{
    qInfo() << Q_FUNC_INFO;

    if (pos.column() == 1) {
        copyText(pos.data().toString());
        ui->statusbar->showMessage(tr("Title copied"), 3000);
    }
    else if (pos.column() == 2) {
        copyText(pos.data().toString());
        ui->statusbar->showMessage(tr("Username copied"), 3000);
    }
    else if (pos.column() == 3) {
        copyText(pos.data().toString());
        ui->statusbar->showMessage(tr("Password copied"), 3000);
    }
    else if (pos.column() == 4) {
        QDesktopServices::openUrl(QUrl(pos.data().toString()));
        ui->statusbar->showMessage(tr("Link opened"), 3000);
    }
    else if (pos.column() == 5) {
        copyText(pos.data().toString());
        ui->statusbar->showMessage(tr("Notes copied"), 3000);
    }
}

void MainWindow::customMenuRequested(QPoint pos) {
    qInfo() << Q_FUNC_INFO;

    if (ui->listWidget->count() == 0) // Checking if database is opened.
        return;


    if (ui->tableView->indexAt(pos).isValid() && ui->tableView->underMouse()) { // If mouse cursor is hovering tableView and entry
        menu_contextMenu.reset(new QMenu(this)); // Resetting context menu

        // Resetting actions for context menu
        action_copyUsername.reset(new QAction(IconLoader::getIcon(Icon::User, theme), tr("Copy User Name"), this));
        action_copyPassword.reset(new QAction(IconLoader::getIcon(Icon::Key, theme), tr("Copy Password"), this));
        action_edit.reset(new QAction(IconLoader::getIcon(Icon::Edit, theme), tr("Edit"), this));
        action_add.reset(new QAction(IconLoader::getIcon(Icon::Entry, theme), tr("Add new"), this));
        action_delete.reset(new QAction(IconLoader::getIcon(Icon::Trash, theme), tr("Delete"), this));
        action_configureColumns.reset(new QAction(IconLoader::getIcon(Icon::Settings, theme), tr("Configure colums"), this));

        // Resseting url menu and adding actions to it
        menu_url.reset(new QMenu(tr("URL"), this));
        menu_url->setIcon(IconLoader::getIcon(Icon::Link, theme));
        action_copyUrl.reset(new QAction(IconLoader::getIcon(Icon::Copy, theme), tr("Copy"), this));
        action_OpenUrl.reset(new QAction(IconLoader::getIcon(Icon::OpenBrowser, theme), tr("Open"), this));
        menu_url->addAction(action_copyUrl.data());
        menu_url->addAction(action_OpenUrl.data());

        // Connecting actions
        connect(action_copyUsername.data(), SIGNAL(triggered()), SLOT(copyUsername()));
        connect(action_copyPassword.data(), SIGNAL(triggered()), SLOT(copyPassword()));
        connect(action_add.data(), SIGNAL(triggered()), SLOT(addEntry()));
        connect(action_delete.data(), SIGNAL(triggered()), SLOT(deleteEntry()));
        connect(action_edit.data(), SIGNAL(triggered()), SLOT(editRow()));
        connect(action_copyUrl.data(), SIGNAL(triggered()), SLOT(copyUrl()));
        connect(action_OpenUrl.data(), SIGNAL(triggered()), SLOT(openUrl()));
        connect(action_configureColumns.data(), SIGNAL(triggered()), SLOT(cfgColumns()));

        // Adding actions to context menu
        menu_contextMenu->addAction(action_copyUsername.data());
        menu_contextMenu->addAction(action_copyPassword.data());
        menu_contextMenu->addMenu(menu_url.data());
        menu_contextMenu->addAction(action_edit.data());
        menu_contextMenu->addAction(action_delete.data());
        menu_contextMenu->addAction(action_add.data());
        menu_contextMenu->addAction(action_configureColumns.data());

        menu_contextMenu->popup(ui->tableView->viewport()->mapToGlobal(pos)); // Showing context menu at pos
    } else if (ui->tableView->underMouse()) { // If mouse cursor is hovering only tableView
        menu_contextMenu.reset(new QMenu(this)); // Resetting context menu

        // Resetting actions
        action_add.reset(new QAction(IconLoader::getIcon(Icon::Add, theme), tr("Add new"), this)); // Resetting QAction *actionAdd object
        action_configureColumns.reset(new QAction(IconLoader::getIcon(Icon::Settings, theme), tr("Configure colums"), this));

        // Connecting actions
        connect(action_add.data(), SIGNAL(triggered()), SLOT(addEntry()));
        connect(action_configureColumns.data(), SIGNAL(triggered()), SLOT(cfgColumns()));

        // Adding actions to context menu
        menu_contextMenu->addAction(action_add.data());
        menu_contextMenu->addAction(action_configureColumns.data());

        menu_contextMenu->popup(ui->tableView->viewport()->mapToGlobal(pos)); // Showing context menu at pos
    } else if (ui->listWidget->indexAt(pos).isValid() && ui->listWidget->underMouse()) { // If mouse cursor is hovering listWidget and his row
        menu_contextMenu.reset(new QMenu(this)); // Resetting actions

        // Resetting actions
        action_delete.reset(new QAction(IconLoader::getIcon(Icon::Trash, theme), tr("Delete"), this));
        action_add.reset(new QAction(IconLoader::getIcon(Icon::Add, theme), tr("Add Table"), this));
        action_edit.reset(new QAction(IconLoader::getIcon(Icon::Edit, theme), tr("Edit"), this));

        // Connecting actions
        connect(action_delete.data(), SIGNAL(triggered()), SLOT(deleteTable()));
        connect(action_add.data(), SIGNAL(triggered()), SLOT(createTable()));
        connect(action_edit.data(), SIGNAL(triggered()), SLOT(editTable()));

        // Adding actions to context menu
        menu_contextMenu->addAction(action_add.data());
        menu_contextMenu->addAction(action_edit.data());
        menu_contextMenu->addAction(action_delete.data());

        menu_contextMenu->popup(ui->listWidget->viewport()->mapToGlobal(pos)); // Showing context menu at pos
    } else if (ui->listWidget->underMouse()) { // If cursor is hovering only listWidget
        menu_contextMenu.reset(new QMenu(this)); // Resetting context menu
        action_add.reset(new QAction(IconLoader::getIcon(Icon::Add, theme), tr("Add Table"), this)); // Resetting add action
        connect(action_add.data(), SIGNAL(triggered()), SLOT(createTable())); // Connecting actions
        menu_contextMenu->addAction(action_add.data()); // Adding action to context menu
        menu_contextMenu->popup(ui->listWidget->viewport()->mapToGlobal(pos)); // Showing context menu at pos
    }
}

void MainWindow::copyUrl(){
    qInfo() << Q_FUNC_INFO;

    if (!hasSelectedRow()) // If user selected no row, do
        return;

    QModelIndex idx = model->index(ui->tableView->currentIndex().row(), 4);

    copyText(idx.data().toString()); // Copying text to clipboard
}

void MainWindow::openUrl() {
    qInfo() << Q_FUNC_INFO;

    if (!hasSelectedRow()) // If user selected no row, do
        return;

    QModelIndex idx = model->index(ui->tableView->currentIndex().row(), 4);
    QDesktopServices::openUrl(idx.data().toString());
}

void MainWindow::copyUsername() {
    qInfo() << Q_FUNC_INFO;

    if (!hasSelectedRow()) // If user selected no row, do
        return;

    QSqlQuery query(db);
    QString getId = QString("SELECT id FROM %1 LIMIT 1 OFFSET %2").arg(model->tableName()).arg(ui->tableView->currentIndex().row());
    query.prepare(getId);
    if (!query.exec()) {
        showMsgBox(tr("Error"), tr("Unable to copy.\nTry again please."), QMessageBox::Icon::Critical);
        qCritical() << "Unable to copy User Name, Error: " << query.lastError() << "\n Last query: " << query.lastQuery();
        return;
    }
    query.next();
    QString id = query.value(0).toString();

    query.prepare("SELECT [User Name] FROM "+model->tableName()+" WHERE id == "+id);
    if (!query.exec()) {
        showMsgBox(tr("Error"), tr("Unable to copy.\nTry again please."), QMessageBox::Icon::Critical);
        qCritical() << "Unable to copy User Name, Error: " << query.lastError() << "\n Last query: " << query.lastQuery();
        return;
    }
    query.next();

    copyText(query.value(0).toString());

    ui->statusbar->showMessage(tr("Username copied."), 30000);
}


void MainWindow::copyText(const QString &text) {
    qInfo() << Q_FUNC_INFO;
    OpenClipboard(0);
    EmptyClipboard();
    HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, text.size()+1);
    strcpy_s((char*)hGlob, text.size()+1, &text.toStdString()[0]);
    SetClipboardData(CF_TEXT, hGlob);
    CloseClipboard();
}


void MainWindow::copyPassword() {
    qInfo() << Q_FUNC_INFO;

    if (!hasSelectedRow()) // If user selected no row, do
        return;

    QSqlQuery query(db);
    QString getId = QString("SELECT id FROM %1 LIMIT 1 OFFSET %2").arg(model->tableName()).arg(ui->tableView->currentIndex().row());
    query.prepare(getId);
    if (!query.exec()) {
        showMsgBox(tr("Error"), tr("Unable to copy.\nTry again please."), QMessageBox::Icon::Critical);
        qCritical() << "Unable to copy User Name, Error: " << query.lastError() << "\n Last query: " << query.lastQuery();
        return;
    }
    query.next();
    QString id = query.value(0).toString();

    query.prepare("SELECT [Password] FROM "+model->tableName()+" WHERE id == "+id);
    if (!query.exec()) {
        showMsgBox(tr("Error"), tr("Unable to copy.\nTry again please."), QMessageBox::Icon::Critical);
        qCritical() << "Unable to copy User Name, Error: " << query.lastError() << "\n Last query: " << query.lastQuery();
        return;
    }
    query.next();

    copyText(query.value(0).toString());

    ui->statusbar->showMessage(tr("Password copied."), 3000);
}

void MainWindow::editRow() {
    qInfo() << Q_FUNC_INFO;

    if (!hasSelectedRow()) // If user has no selected row return.
        return;

    EntryEditor *editEntry = new EntryEditor(this, ui->tableView, &db, model);
    editEntry->exec();
    delete editEntry;
    isChanged = true;
    model->select();

    ui->tableView->update();
    configureColumns();
}

void MainWindow::deleteTable() {
    qInfo() << Q_FUNC_INFO;
    int index = ui->listWidget->currentIndex().row();

    QMessageBox::StandardButton question = QMessageBox::question(
        this, "RadikPass", tr("Delete Table?"),
        QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes); // Creates MessageBox with buttons

    if (question == QMessageBox::Yes) {
        if (tables.size() != 1) {
            if (DbManager::deleteTable(&db, ui->listWidget->item(index)->text())) {
                isChanged = true;
                tables.erase(tables.begin() + index);
                ui->listWidget->takeItem(index);
                model->setTable(tables[tables.size() - 1]);
                model->select();
                ui->tableView->update();
                configureColumns();
                ui->statusbar->showMessage(tr("Table deleted."), 3000);
            } else
                showMsgBox(tr("Error"), tr("Unable to delete this table.\nTry again please."), QMessageBox::Icon::Critical);
        } else {
            QMessageBox::information(this, "RadikPass", tr("At least one table must exists"),
                                                                               QMessageBox::Ok, QMessageBox::Ok);
        }
    }

    configureColumns();
}

void MainWindow::deleteEntry() {
    qInfo() << Q_FUNC_INFO;

    if (!hasSelectedRow()) // If user selected no row, do
        return;

    int index = ui->tableView->currentIndex().row(); // Index of selected row

    QMessageBox::StandardButton question = QMessageBox::question(
        this, "RadikPass", tr("Delete row?"),
        QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes); // Creates MessageBox with buttons

    if (question == QMessageBox::Yes) {
        model->removeRow(index);
        model->select();
        ui->tableView->setModel(model);
        isChanged = true;
        ui->statusbar->showMessage(tr("Entry deleted."), 30000);
    }
}

void MainWindow::on_listWidget_currentTextChanged(const QString &currentText) {
    qInfo() << Q_FUNC_INFO; // Writing function names to see where error appears, all this messages shown in Application Output

    // Updating last used index
    lastUsedTable = ui->listWidget->currentRow();

    // Updating QSqlModel and QTableView to see selected table
    model->setTable(currentText);
    model->select();
    ui->tableView->setModel(model);
    configureColumns();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    qInfo() << Q_FUNC_INFO;
    if (isChanged) {
        QMessageBox::StandardButton question = QMessageBox::question(
            this, "RadikPass", tr("Save changes?"),
            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes); // Creates MessageBox with buttons
        if (question == QMessageBox::Yes) {
            DbManager::uploadDb(settings.value("Last").toString(), key, &db);
            event->accept();
        }else if (question == QMessageBox::No) {
            event->accept();
        }else if (question == QMessageBox::Cancel) {
            event->ignore();
        }
    }
}


void MainWindow::cfgColumns() {
    ColumnsConfigurator *cfgColumns = new ColumnsConfigurator(this);
    cfgColumns->exec();
    delete cfgColumns;
    configureColumns();
}

void MainWindow::createTable() {
    qInfo() << Q_FUNC_INFO; // Writing function names to see where error appears, all this messages shown in Application Output

    isChanged = true; // If user do some changes needs to change this state to true to ask if save changes on exit

    TableAdder *addTable = new TableAdder(this, &db, &tables, theme); // This dialog will create new table in QSqlDatabase object and append new table to tables list
    addTable->exec(); // Showing dialog
    delete addTable;

    if (ui->listWidget->count() != tables.size()) { // If current ListWidget size has lower value than tables this means that user added new table
        ui->listWidget->clear(); // Clearing list of tables
        for(int i = 0; i < tables.size(); i++) {
            ui->listWidget->addItem(tables[i]);  // Adding lists to QListWidget
        }
        ui->listWidget->setCurrentRow(tables.size()-1);

        // Selecting last table (this is table that user added)
        model->setTable(tables[tables.size() - 1]);
        model->select();
    }
    loadIconsToListWidget();
}

void MainWindow::addEntry() {
    qInfo() << Q_FUNC_INFO; // Writing function names to see where error appears, all this messages shown in Application Output

    isChanged = true; // If user do some changes needs to change this state to true to ask if save changes on exit

    EntryAdder *dialog = new EntryAdder(this, &db, model->tableName(), theme); // Create dialog that will add row to table
    dialog->exec();    // Showing dialog
    delete dialog;

    // Updating QSqlModel and QTableView to see changes
    model->setTable(model->tableName());
    model->select();
    ui->tableView->update();
    configureColumns();


}

void MainWindow::openDatabase() {
    if (isChanged) {
        QMessageBox::StandardButton question = QMessageBox::question(
            this, "RadikPass", tr("Save changes?"),
            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes); // Creates MessageBox with buttons
        // If user clicks
        // Yes - must save changes
        // No - do not save changes
        // Cancel - do not close program
        if (question == QMessageBox::Yes) {
            qDebug() << "Clicked yes";
            DbManager::uploadDb(settings.value("Last").toString(), key, &db);
        }else if (question == QMessageBox::No) {
            qDebug() << "Clicked no";
        }else if (question == QMessageBox::Cancel) {
            qDebug() << "Clicked cancel";
            return;
        }
    }

    QString file = QFileDialog::getOpenFileName(this, tr("Open encrypted database"),
                                                QStandardPaths::writableLocation(QStandardPaths::AppDataLocation), "*.db");
    if (!file.isEmpty()) {
        DbOpener *openDb = new DbOpener(this, &key, file, theme);
        openDb->exec();
    } else return;

    if (!key.isEmpty()) {
        if (!DbManager::loadDb(file, key, &db, tables)) {
            showMsgBox(tr("Error"), tr("Password is incorrect or file is damaged."), QMessageBox::Critical);
        }
        settings.setValue("Last", file);
        ui->listWidget->clear();
        for(int i = 0; i < tables.size(); i++) {
            ui->listWidget->addItem(tables[i]);
        }
        if (tables.size() > 0) { // If one or more table exists we select current first table
            model->setTable(tables[0]);
            ui->listWidget->setCurrentRow(0);
        }
        model->select();
        ui->tableView->update();
    }
    configureColumns();

    loadIcons();
}

void MainWindow::createDatabase() {
    qInfo() << Q_FUNC_INFO;
    QString databasePath = QFileDialog::getSaveFileName(this, tr("Create new database"),
                                            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation), "*.db"); // Asking user to chose path and name of new database
    if (databasePath == "") return; // If user closed window this means that datbase path is null and function execution terminates

    DbCreator *createNew = new DbCreator(this, &key, databasePath); // Creating window where user can set password to new database
    createNew->exec();
    delete createNew;

    if (key != "") { // If key is empty this means that user closed createNew dialog
        if (!DbManager::createAndFillDatabase(databasePath, key, &db)) { // This must create new .db file and encrypt it
            showMsgBox(tr("Error"), tr("Unable to create new database.\nTry again please."), QMessageBox::Critical);
            return;
        }

        DbManager::loadDb(databasePath, key, &db, tables); // Loads database to to the QSqlDatabase instance
        settings.setValue("Last", databasePath); // Setting last used database path to recently created

        ui->listWidget->clear(); // Clears listWidget that holds all tables

        for(int i = 0; i < tables.size(); i++) { // Loading all tables that available to user from tables vector
            ui->listWidget->addItem(tables[i]);
        }

        if (tables.size() > 0)
            ui->listWidget->setCurrentRow(0); // Select first item in listWidget, item changes connected to slot on_listWidget_currentTextChanged();
    }

    configureColumns(); // Showing columns according to settings.
    loadIconsToListWidget(); // Loading icons to ListWidget with tables.
}

void MainWindow::configureEntryMenu() { // This function will disable or enable actions in View menu in toolbar
    qInfo() << Q_FUNC_INFO;

    if (ui->tableView->selectionModel()->hasSelection()) { // If row is not selected in TableView this will disable actions and vise versa
        ui->actionCopy_User_Name->setEnabled(true);
        ui->actionCopy_Password->setEnabled(true);
        ui->menuUrl->setEnabled(true);
        ui->actionEdit_Entry->setEnabled(true);
        ui->actionDuplicate_Entry->setEnabled(true);
        ui->actionDelete_Entry->setEnabled(true);
    } else if (ui->listWidget->count() == 0) { // Checking if database is opened
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

void MainWindow::loadIconsToListWidget() { // This will load icons to ListWidget,
    QSqlQuery query(db); // New query to read data from database
    for(int i = 0; i < ui->listWidget->count(); i++) { // For every item in ListWidget(table)
        // Loading name of icon from TablesSettings
        query.prepare("SELECT Icon FROM TablesSettings WHERE [Table] = :name");
        query.bindValue(":name", ui->listWidget->item(i)->text());
        if (!query.exec()) {
            showMsgBox(tr("Error"), tr("Unable to load icon to table"), QMessageBox::Critical);
        }

        query.next(); // Loading value to query
        if (theme == Theme::Dark) ui->listWidget->item(i)->setIcon(QIcon(":/icons/dark/resources/icons/dark/"+query.value(0).toString()+".png")); // Set icon in ListWidget row
        else ui->listWidget->item(i)->setIcon(QIcon(":/icons/light/resources/icons/light/"+query.value(0).toString()+".png"));
    }
}

void MainWindow::editTable() {
    qInfo() << Q_FUNC_INFO;

    // Creating TableEditor window where user can change name or/and icon of table
    TableEditor *editTable = new TableEditor(this, &db, ui->listWidget->currentItem()->text(), ui->listWidget, theme);
    editTable->exec();
    delete editTable;

    // Selecting in table model table that user was editing.
    model->setTable(ui->listWidget->currentItem()->text());
    model->select();

    isChanged = true; // If user makes some changes needs to set this to true

    loadIconsToListWidget(); // Loading icons to ListWidget with tables
    configureColumns(); // Showing columns according to settings.

}

void MainWindow::duplicateEntry() {
    qInfo() << Q_FUNC_INFO;

    if (!hasSelectedRow()) // If no rows are selected
        return;

    QSqlQuery query(db);
    QString id = model->data(model->index(ui->tableView->currentIndex().row(), 0)).toString(); // Id of row which user wants to duplicate

    // Duplicating row
    query.prepare("INSERT INTO ["+model->tableName()+"] (Title, [User Name], Password, URL, Notes, [Creation Time], [Last Changed]) SELECT Title, [User Name], Password, URL, Notes, [Creation Time], [Last Changed] FROM ["+model->tableName()+"] WHERE id = "+id);
    if (!query.exec()) {
        showMsgBox(tr("Error"), tr("Unable to duplicate entry.\nTry again, please."), QMessageBox::Critical);
        qCritical() << "Can't duplicate entry, last query: " << query.lastQuery() << "\nQuery error: " << query.lastError();
        return;
    }

    isChanged = true; // Setting isChanged state to true to mark that user done some changes in database

    ui->statusbar->showMessage(tr("Entry duplicated"), 3); // Showing message to statusbar

    // Updating tableView
    model->select();
    ui->tableView->update();
}

void MainWindow::saveAll() {
    qInfo() << Q_FUNC_INFO;
    // Writing changes to file
    DbManager::uploadDb(settings.value("Last").toString(), key, &db);
}

bool MainWindow::hasSelectedRow() {
    if (ui->tableView->selectionModel()->selectedRows().size() == 0) // If count of selected rows equals to zero
        return false;

    return true;
}

void MainWindow::setUkrainianLanguage() {
    qInfo() << Q_FUNC_INFO;

    settings.setValue("Language", "uk"); // Changing value in settings

    qApp->removeTranslator(translator); // removing the old translator

    if (translator->load(":/translations/resources/translations/uk.qm")) // Trying to load translator from resource
        qApp->installTranslator(translator);
    else {
        showMsgBox(tr("Error"), tr("Unable to load ukrainian translation"), QMessageBox::Critical);
        return;
    }

    ui->retranslateUi(this); // Retranslating program

    setColorThemeActions(); // Retranslating actions
    setTooltips(); // Retranslating tooltips
    setHeaders(); // Retranslating headers in tableView
}

void MainWindow::setEnglishLanguage() {
    qInfo() << Q_FUNC_INFO;

    settings.setValue("Language", "en"); // Changing value in settings

    qApp->removeTranslator(translator); // removing the old translator

    if (translator->load(":/translations/resources/translations/en.qm")) // Trying to load translator from resource
        qApp->installTranslator(translator);
    else {
        showMsgBox(tr("Error"), tr("Unable to load english translation"), QMessageBox::Critical);
    }

    ui->retranslateUi(this); // Retranslating program

    setColorThemeActions(); // Retranslating actions
    setTooltips(); // Retranslating tooltips
    setHeaders(); // Retranslating headers in tableView
}

void MainWindow::setGermanLanguage() {
    qInfo() << Q_FUNC_INFO;

    settings.setValue("Language", "ge"); // Changing value in settings

    qApp->removeTranslator(translator); // removing the old translator

    if (translator->load(":/translations/resources/translations/ge.qm")) // Trying to load translator from resource
        qApp->installTranslator(translator);
    else {
        showMsgBox(tr("Error"), tr("Unable to load german translation"), QMessageBox::Critical);
    }
    ui->retranslateUi(this); // Retranslating program

    setColorThemeActions(); // Retranslating actions
    setTooltips(); // Retranslating tooltips
    setHeaders(); // Retranslating headers in tableView
}

void MainWindow::setSystemColorTheme() {
    qInfo() << Q_FUNC_INFO;

    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
        // Reading theme from resource
        QFile styleFile(":/themes/resources/themes/dark.qss");
        styleFile.open(QFile::ReadOnly);
        QString style(styleFile.readAll());
        styleFile.close();

        this->theme = Theme::Dark; // Setting theme value
        qApp->setStyleSheet(style); // Applying style to program
    } else {
        // Reading theme from resource
        QFile styleFile(":/themes/resources/themes/light.qss");
        styleFile.open(QFile::ReadOnly);
        QString style(styleFile.readAll());
        styleFile.close();

        this->theme = Theme::Light; // Setting theme value
        qApp->setStyleSheet(style); // Applying style to program
    }
    settings.setValue("theme", "system");

    loadIcons(); // Updating icons
}

void MainWindow::setDarkColorTheme() {
    qInfo() << Q_FUNC_INFO;

    // Reading theme from resource
    QFile styleFile(":/themes/resources/themes/dark.qss");
    styleFile.open(QFile::ReadOnly);
    QString style(styleFile.readAll());
    styleFile.close();

    this->theme = Theme::Dark; // Setting theme value
    settings.setValue("theme", "dark");
    qApp->setStyleSheet(style); // Applying style to program
    loadIcons(); // Updating icons
}

void MainWindow::setLightColorTheme()
{
    qInfo() << Q_FUNC_INFO;
    // Reading theme from resource
    QFile styleFile(":/themes/resources/themes/light.qss");
    styleFile.open(QFile::ReadOnly);
    QString style(styleFile.readAll());
    styleFile.close();

    this->theme = Theme::Light;
    settings.setValue("theme", "light");
    qApp->setStyleSheet(style); // Applying style to program
    loadIcons(); // Updating icons
}


void MainWindow::on_searchBar_textChanged(const QString &arg1) {
    if (arg1.size() == 0) { // If user cleared text in search bar
        if (lastUsedTable <= ui->listWidget->count()) {
            ui->listWidget->setCurrentRow(lastUsedTable);
            model->setTable(ui->listWidget->currentItem()->text());
            model->select();
            return;
        } else {
            ui->listWidget->setCurrentRow(0);
            model->setTable(ui->listWidget->currentItem()->text());
            model->select();
            return;
        }
        return;
    }

    DbManager::search(arg1, &db, ui->listWidget); // Filling "Search" table with found rows

    // Setting "Search" table and updating
    model->setTable("Search");
    model->select();
}
