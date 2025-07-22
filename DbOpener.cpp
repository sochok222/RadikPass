#include "DbOpener.h"
#include "ui_DbOpener.h"

DbOpener::DbOpener(QWidget *parent, QByteArray *result, const QString &path, Theme theme)
    : QDialog(parent)
    , ui(new Ui::DbOpener)
    , theme(theme)
{
    ui->setupUi(this);
    ui->okButton->setDefault(true);

    ui->passwordLine->setEchoMode(QLineEdit::EchoMode::Password);

    action_hidePassword = ui->passwordLine->addAction(IconLoader::getIcon(Icon::EyeClosed, theme), QLineEdit::TrailingPosition);
    connect(action_hidePassword, SIGNAL(triggered(bool)), this, SLOT(hidePassword()));


    ui->databasePathLabel->setText(path);
    masterPassword = result;
    QSettings setting("AlexRadik", "RadikPass");
    ui->okButton->setAutoDefault(true);
    this->setWindowTitle(tr("Open Database"));
}

DbOpener::~DbOpener() {
    delete ui;
}

void DbOpener::hidePassword() {
    if (ui->passwordLine->echoMode() == QLineEdit::EchoMode::Password) {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::Eye, theme));
        ui->passwordLine->setEchoMode(QLineEdit::Normal);
    } else {
        action_hidePassword->setIcon(IconLoader::getIcon(Icon::EyeClosed, theme));
        ui->passwordLine->setEchoMode(QLineEdit::Password);
    }
}



void DbOpener::on_okButton_clicked() {
    if (ui->passwordLine->text().size() > 0) {
        *masterPassword = ui->passwordLine->text().toUtf8();
        this->close();
    } else {
        QMessageBox msg;
        msg.setText(tr("Password line must be not empty"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
}


void DbOpener::on_cancelButton_clicked() {
    settings.setValue("Last", "");
    this->destroy(true,true);
}


