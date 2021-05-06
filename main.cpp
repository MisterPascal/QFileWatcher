#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDateTime>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg){
    if(MainWindow::mDebugTextEdit == nullptr){
        QByteArray localMsg = msg.toLocal8Bit();
        switch (type) {
        case QtDebugMsg:
            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            abort();
        }
    }
    else{
        switch (type) {
        case QtDebugMsg:
        case QtWarningMsg:
        case QtCriticalMsg:
            // redundant check, could be removed, or the
            // upper if statement could be removed
            if(MainWindow::mDebugTextEdit != nullptr)
                MainWindow::mDebugTextEdit->append(QDateTime::currentDateTime().toString("dd.MM hh:mm:ss ")  + msg);
            break;
        case QtFatalMsg:
            abort();
        }
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QApplication a(argc, argv);
    a.setApplicationName("QFileWatcher");
    a.setApplicationVersion("0.3");

    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale::system(), "qt", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath))){
        a.installTranslator(&qtTranslator);
    }

    QTranslator qtBaseTranslator;
    if (qtBaseTranslator.load("qtbase_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath))){
        a.installTranslator(&qtBaseTranslator);
    }

    MainWindow w;
    w.show();
    return a.exec();
}
