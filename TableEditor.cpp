#include "TableEditor.h"
#include "ui_TableEditor.h"
#include <qsqlerror.h>

void TableEditor::showMsgBox(const QString text) {
    QMessageBox msg;
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}

TableEditor::TableEditor(QWidget *parent, QSqlDatabase *db, const QString tableName, QListWidget *listWidget, Theme theme)
    : QDialog(parent)
    , ui(new Ui::TableEditor)
    , db(db)
    , tableName(tableName)
    , listWidget(listWidget)
    , theme(theme)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Edit Table"));
    ui->nameEdit->setText(tableName);

    if (db == nullptr || !db->isOpen()) {
        showMsgBox(tr("Can`t open database"));
        this->close();
    }

    model = new QStandardItemModel;
    mapper = new QDataWidgetMapper;

    ui->nameEdit->setMaxLength(15);
    QRegularExpression rx(R"(^[a-zA-Zа-яА-ЯІіїЇ0-9_]+(\s[a-zA-Zа-яА-ЯІіїЇ0-9_]+)+$)");
    QValidator *validator = new QRegularExpressionValidator(rx, this);

    ui->nameEdit->setValidator(validator);

    QTimer::singleShot(0, this, SLOT(setConnections()));

    if (listWidget == nullptr) {
        showMsgBox(tr("Can't load listWidget"));
        QTimer::singleShot(0, this, SLOT(close()));
    }
    loadIcons();
}

TableEditor::~TableEditor() {
    delete ui;
    delete mapper;
    delete model;
}

void TableEditor::loadIcons() {
    QVector<Icon> icons = {Icon::Entry, Icon::Game, Icon::House, Icon::Money, Icon::Office, Icon::Pc, Icon::Programming, Icon::User, Icon::Key};
    for(Icon &ico : icons) {
        ui->comboBox->addItem(QIcon(IconLoader::getIcon(ico, theme)), "");
        model->appendRow(new QStandardItem(IconLoader::getIconName(ico)));
    }

    mapper->setModel(model);
    mapper->toFirst();
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), mapper, SLOT(setCurrentIndex(int)));
}

void TableEditor::setConnections() {
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), SLOT(iconChanged()));
    connect(ui->nameEdit, SIGNAL(textChanged(QString)), SLOT(textChanged()));
}

bool TableEditor::saveChanges() {
    QSqlQuery query(*db);

    if (ui->nameEdit->text().size() <= 0) {
        showMsgBox(tr("Name field must be not empty."));
        return false;
    }

    QString changeName = "ALTER TABLE '"+tableName+"' RENAME TO '"+ui->nameEdit->text()+"'";
    QString restoreName = "ALTER TABLE '"+ui->nameEdit->text()+"' RENAME TO '"+tableName+"'";
    QString changeSettingName = "UPDATE TablesSettings SET [Table] = '"+ui->nameEdit->text()+"' WHERE [Table] = '"+tableName+"'";
    QString ico(mapper->model()->data(model->index(mapper->currentIndex(), 0)).toString());
    QString setNewIcon("UPDATE TablesSettings SET Icon = '"+ico+"' WHERE [Table] = '"+ui->nameEdit->text()+"'");

    if (ui->nameEdit->text().size() == 0) {
        showMsgBox(tr("Line must be not empty"));
        return false;
    }

    if (isNameChanged) {
        if (!query.exec(changeName)) {
            showMsgBox(tr("Unable to change name of table, try to reopen database."));
            return false;
        }
        if (!query.exec(changeSettingName)) {
            query.exec(restoreName);
            showMsgBox(tr("Unable to change name of table, try to reopen database."));
            return false;
        }
    }

    if (isIconChanged) {
        if (!query.exec(setNewIcon)) {
            showMsgBox(tr("Unable to change icon of table, try to reopen database."));
            return false;
        }
    }

    listWidget->currentItem()->setText(ui->nameEdit->text());

    return true;
}


void TableEditor::closeEvent(QCloseEvent *event) {
    qDebug() << Q_FUNC_INFO;
    if (isIconChanged || isNameChanged) {
        QMessageBox::StandardButton question = QMessageBox::question(
            this, "RadikPass", "Save changes?",
            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes); // Creates MessageBox with buttons
        if (question == QMessageBox::Yes) {
            saveChanges();
            event->accept();
        } else if (question == QMessageBox::No) {
            event->accept();
        } else if (question == QMessageBox::Cancel) {
            event->ignore();
        }
    }
    event->accept();
}

void TableEditor::on_buttonSave_clicked() {
    if (saveChanges())
        this->accept();
}


void TableEditor::on_buttonCancel_clicked() {
    this->accept();
}

