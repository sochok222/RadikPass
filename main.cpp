#include "mainwindow.h"
#include <QApplication>
#include <QStyleHints>
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
    QTranslator *translator = new QTranslator;
    QSettings settings = QSettings("AlexRadik", "RadiPass");
    qApp->setWindowIcon(QIcon(":/programImages/resources/program_images/windowIcon.svg")); // Set icon for all windows

    if(!settings.value("Language").isNull() && settings.value("Language") != "")
    {
        if(settings.value("Language") == "uk")
        {
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
        }
    }else settings.setValue("Language", "en"); // Default language is english


    QString theme;
    if(settings.value("theme") == "system" || (settings.value("theme").isNull() || settings.value("theme") == ""))
    {
        if(QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark)
        {
            QFile  styleFile(":/themes/resources/themes/dark.qss");
            styleFile.open(QFile::ReadOnly);

            theme = "dark";

            QString  style(styleFile.readAll());
            styleFile.close();

            qApp->setStyleSheet(style);
        }else {
            QFile  styleFile(":/themes/resources/themes/light.qss");
            styleFile.open(QFile::ReadOnly);

            theme = "light";

            QString  style(styleFile.readAll());
            styleFile.close();

            qApp->setStyleSheet(style);
        }
    }else
    {
        if(settings.value("theme") == "dark")
        {
            QFile  styleFile(":/themes/resources/themes/dark.qss");
            styleFile.open(QFile::ReadOnly);

            theme = "dark";

            QString  style(styleFile.readAll());
            styleFile.close();

            qApp->setStyleSheet(style);
        }else {
            QFile  styleFile(":/themes/resources/themes/light.qss");
            styleFile.open(QFile::ReadOnly);

            theme = "light";

            QString  style(styleFile.readAll());
            styleFile.close();

            qApp->setStyleSheet(style);
        }
    }


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

    MainWindow w(0, key, translator, theme);
    w.show();
    return a.exec();
}
