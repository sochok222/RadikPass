#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QSettings>
#include "opendatabase.h"

bool isFileExists(const QString path)
{
    QFile file(path);
    if(file.exists())
        return true;
    return false;
}

/// Settings
// columnId
// columnTitle
// columnUserName
// columnPassword
// columnUrl
// columnNotes
// Last - last used database

int main(int argc, char *argv[])
{
    qDebug() << Q_FUNC_INFO;
    QApplication a(argc, argv);

    QSettings settings = QSettings("AlexRadik", "RadiPass");

    QByteArray key;
    if((!settings.value("Last").isNull() && settings.value("Last").toString() != ""))
    {
        qDebug() << "Got previously used database" << settings.value("Last").toString();
        if(isFileExists(settings.value("Last").toString()))
        {
            qDebug() << "And it exists";
            OpenDatabase *openDb = new OpenDatabase(0, &key);
            openDb->exec();
        }else {
            qDebug() << "It doesn't exists";
            QMessageBox msg;
            msg.setIcon(QMessageBox::Information);
            msg.setText("Found last used database: " + settings.value("Last").toString() + "\nBut it doesn't extists");
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
            settings.setValue("Last", "");
        }
    }else
    {
        qDebug() << "Can't find last used database";
    }

    MainWindow w(0, key);
    w.show();
    return a.exec();
}
