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

void setStandardCfgColumns()
{
    QSettings settings("AlexRadik", "RadiPass"); // Loading current settings to QSettings, this will write settings to registry

    // QStringList to store two values in one QSettings field
    QStringList columnTitle = QStringList() << "" << "";
    QStringList columnUsername = QStringList() << "" << "";
    QStringList columnPassword = QStringList() << "" << "";
    QStringList columnURL = QStringList() << "" << "";
    QStringList columnNotes = QStringList() << "" << "";
    QStringList columnCreationTime = QStringList() << "" << "";
    QStringList columnLastChanged = QStringList() << "" << "";

    // Set standard values
    columnTitle[0] = "shown"; columnTitle[1] = "unmasked";
    columnUsername[0] = "shown"; columnUsername[1] = "unmasked";
    columnPassword[0] = "shown"; columnPassword[1] = "masked";
    columnURL[0] = "shown"; columnURL[1] = "unmasked";
    columnNotes[0] = "shown"; columnNotes[1] = "unmasked";
    columnCreationTime[0] = "shown"; columnCreationTime[1] = "unmasked";
    columnLastChanged[0] = "shown"; columnLastChanged[1] = "unmasked";

    // Applying settings
    settings.setValue("columnTitle", columnTitle);
    settings.setValue("columnUsername", columnUsername);
    settings.setValue("columnPassword", columnPassword);
    settings.setValue("columnURL", columnURL);
    settings.setValue("columnNotes", columnNotes);
    settings.setValue("columnCreationTime", columnCreationTime);
    settings.setValue("columnLastChanged", columnLastChanged);
}


int main(int argc, char *argv[])
{
    qDebug() << Q_FUNC_INFO;
    setStandardCfgColumns();
    QApplication a(argc, argv);
    QTranslator *translator = new QTranslator;
    QSettings settings = QSettings("AlexRadik", "RadiPass");
    qApp->setWindowIcon(QIcon(":/programImages/resources/program_images/windowIcon.svg")); // Set icon for all windows

    // Checking if the fields that used to configure columns exists.

    QVector<QString> cfgColumnsFields = {"columnTitle", "columnUsername", "columnPassword", "columnURL", "columnNotes", "columnCreationTime", "columnLastChanged"};
    for(QString &field : cfgColumnsFields)
    {
        if (settings.value(field).toStringList().size() != 2) // Set standard values if user launches program for first time.
        {
            setStandardCfgColumns();
            break;
        }
    }

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
        }else if(settings.value("Language") == "ge")
        {
            // load the new translator
            QString path = QApplication::applicationDirPath();
            path.append("/Translations/");
            if(translator->load(":/translations/resources/translations/uk.qm")) //Here Path and Filename has to be entered because the system didn't find the QM Files else
                qApp->installTranslator(translator);
            else
            {
                qDebug() << "Can't load german translation";
                QMessageBox msg;
                msg.setText("Can't load german translation");
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
        if(isFileExists(settings.value("Last").toString()))
        {
            OpenDatabase *openDb = new OpenDatabase(0, &key, settings.value("Last").toString());
            openDb->exec();
        }
    }else
    {
        qDebug() << "Can't find last used database";
    }

    MainWindow w(0, key, translator, theme);
    w.show();
    return a.exec();
}
