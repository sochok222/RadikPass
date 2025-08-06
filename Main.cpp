#include "MainWindow.h"
#include <QApplication>
#include <QStyleHints>
#include <QFile>
#include <iostream>
#include <QSettings>
#include "DbOpener.h"
#include "IconLoader.h"

bool isFileExists(const QString path) {
    QFile file(path);
    if (file.exists())
        return true;
    return false;
}

void logMessageHandler(const QtMsgType type, const QMessageLogContext& context, const QString& msg);
void resetColumnsSettings();

int main(int argc, char *argv[]) {
    qDebug() << Q_FUNC_INFO;

    QApplication a(argc, argv);
    QTranslator *translator = new QTranslator;
    QSettings settings = QSettings("AlexRadik", "RadikPass");
    qApp->setWindowIcon(QIcon(":/programImages/resources/program_images/windowIcon.svg")); // Set icon for all windows


    QVector<QString> cfgColumnsFields = {"columnTitle", "columnUsername", "columnPassword", "columnURL", "columnNotes", "columnCreationTime", "columnLastChanged"};
    for (QString &field : cfgColumnsFields) {
        if (settings.value(field).toStringList().size() != 2) { // Set standard values if user launches program for first time.
            resetColumnsSettings();
            break;
        }
    }


    // Obtaining last used language from QSettings
    if (!settings.value("Language").isNull() && settings.value("Language") != ""){
        if (settings.value("Language") == "uk") {
            QString path = QApplication::applicationDirPath();
            path.append("/Translations/");
            if (translator->load(":/translations/resources/translations/uk.qm")) //Here Path and Filename has to be entered because the system didn't find the QM Files else
                qApp->installTranslator(translator);
            else {
                qDebug() << "Can't load ukrainian translation";
                QMessageBox msg;
                msg.setText("Can't load ukrainian translation");
                msg.setStandardButtons(QMessageBox::Ok);
                msg.exec();
                return 1;
            }
        } else if (settings.value("Language") == "ge") {
            QString path = QApplication::applicationDirPath();
            path.append("/Translations/");
            if (translator->load(":/translations/resources/translations/uk.qm")) //Here Path and Filename has to be entered because the system didn't find the QM Files else
                qApp->installTranslator(translator);
            else {
                qDebug() << "Can't load german translation";
                QMessageBox msg;
                msg.setText("Can't load german translation");
                msg.setStandardButtons(QMessageBox::Ok);
                msg.exec();
                return 1;
            }
        }
    } else settings.setValue("Language", "en");



    // Obtaining last used color theme
    Theme theme;
    if (settings.value("theme") == "system" || (settings.value("theme").isNull() || settings.value("theme") == "")) {
        if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
            QFile  styleFile(":/themes/resources/themes/dark.qss");
            styleFile.open(QFile::ReadOnly);
            QString  style(styleFile.readAll());
            styleFile.close();
            qApp->setStyleSheet(style);

            theme = Theme::Dark;
        } else {
            QFile  styleFile(":/themes/resources/themes/light.qss");
            styleFile.open(QFile::ReadOnly);
            QString  style(styleFile.readAll());
            styleFile.close();
            qApp->setStyleSheet(style);

            theme = Theme::Light;
        }
    } else {
        if (settings.value("theme") == "dark") {
            QFile  styleFile(":/themes/resources/themes/dark.qss");
            styleFile.open(QFile::ReadOnly);
            QString  style(styleFile.readAll());
            styleFile.close();
            qApp->setStyleSheet(style);

            theme = Theme::Dark;
        } else {
            QFile  styleFile(":/themes/resources/themes/light.qss");
            styleFile.open(QFile::ReadOnly);
            QString  style(styleFile.readAll());
            styleFile.close();
            qApp->setStyleSheet(style);

            theme = Theme::Light;
        }
    }

    MainWindow w(0, theme, translator);
    w.show();
    return a.exec();
}

void logMessageHandler(const QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    std::cout << msg.toStdString() << std::endl;
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/log.txt";
    QDir dir(path);
    if (!dir.exists(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)))
        dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    QFile logFile(path);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        logFile.write(qUtf8Printable(qFormatLogMessage(type, context, msg) + "\n"));
    }
}

void resetColumnsSettings(){
    QSettings settings("AlexRadik", "RadikPass"); // Loading current settings to QSettings, this will write settings to registry

    // Installing log messge handler
    qInstallMessageHandler(logMessageHandler);
    // Install custom log message pattern
    qSetMessagePattern("%{time yyyy-MM-dd hh:mm} [%{type}]: %{message}");

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
