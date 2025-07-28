#include "DbOpener.h"
#include "ui_DbOpener.h"

DbOpener::DbOpener(QWidget *parent, QByteArray *result, const QString &path, Theme theme)
    : QDialog(parent)
    , ui(new Ui::DbOpener)
    , theme(theme)
{
    ui->setupUi(this);
    ui->button_ok->setDefault(true);

    // Connecting buttons to slots
    connect(ui->button_ok, SIGNAL(clicked(bool)), this, SLOT(clicked_button_ok()));
    connect(ui->button_cancel, SIGNAL(clicked(bool)), this, SLOT(clicked_button_cancel()));

    ui->line_password->setEchoMode(QLineEdit::EchoMode::Password);

    action_hidePassword = ui->line_password->addAction(IconLoader::getIcon(Icon::EyeClosed, theme), QLineEdit::TrailingPosition);
    connect(action_hidePassword, SIGNAL(triggered(bool)), this, SLOT(hidePassword()));


    ui->label_databasePath->setText(path);
    masterPassword = result;
    QSettings setting("AlexRadik", "RadikPass");
    ui->button_ok->setAutoDefault(true);
    this->setWindowTitle(tr("Open Database"));
}

DbOpener::~DbOpener() {
    delete ui;
}

void DbOpener::hidePassword() {
    if (ui->line_password->echoMode() == QLineEdit::EchoMode::Password) {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::Eye, theme));
        ui->line_password->setEchoMode(QLineEdit::Normal);
    } else {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::EyeClosed, theme));
        ui->line_password->setEchoMode(QLineEdit::Password);
    }
}



void DbOpener::clicked_button_ok() {
    if (ui->line_password->text().size() > 0) {
        *masterPassword = ui->line_password->text().toUtf8();
        this->close();
    } else {
        QMessageBox msg;
        msg.setText(tr("Password line must be not empty"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
}


void DbOpener::clicked_button_cancel() {
    settings.setValue("Last", "");
    this->destroy(true,true);
}


