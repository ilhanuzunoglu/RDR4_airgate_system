#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QDir> // Required for path operations

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // ===================================================================
    // Load QSS Style File from an External Path - START
    // ===================================================================

    // Read the path from the "STYLE_PATH" environment variable
    QByteArray stylePathEnv = qgetenv("STYLE_PATH");

    if (stylePathEnv.isEmpty()) {
        qWarning() << "WARNING: The STYLE_PATH environment variable is not set. Styles will not be applied.";
    } else {
        // Construct the full path to the style sheet file
        // Example: /root/ilhan + / + style.qss
        QString styleFilePath = QString::fromLatin1(stylePathEnv) + QDir::separator() + "style.qss";
        styleFilePath = QDir::toNativeSeparators(styleFilePath);

        // Open the file from the filesystem path (note: no ":/" prefix)
        QFile styleFile(styleFilePath);

        if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
            QString styleSheet = QLatin1String(styleFile.readAll());
            a.setStyleSheet(styleSheet);
            qDebug() << "Successfully loaded style from:" << styleFilePath;
        } else {
            qWarning() << "ERROR: Could not open the style sheet file at:" << styleFilePath;
            qWarning() << "Check if the file exists and you have read permissions.";
        }
        styleFile.close();
    }
    // ===================================================================
    // Load QSS Style File from an External Path - END
    // ===================================================================

    MainWindow w;
     // Uygulama başlatılırken sistem saatini internetten güncelle
    w.show();

    return a.exec();
}