#include "filewatchermodule.h"



#include <QFileSystemWatcher>
#include <QDir>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrinterInfo>
#include <QProcess>
#include <QThread>
#include <QCoreApplication>
#include <QSettings>
#include <QTime>
#include <QTimer>

#include <QDebug>

#define MAXPRINTTRIES 10

FileWatcherModule::FileWatcherModule(const QString id, QObject *parent) : QObject(parent), mId(id){

    mWatcher = new QFileSystemWatcher(this);

    connect(this, &FileWatcherModule::watchPathChanged, this, [=]{
        mWatcher->removePaths(mWatcher->files());
        mWatcher->addPath(mWatchPath);
        qDebug() << "watch path" << mWatchPath;
    });

    connect(mWatcher, &QFileSystemWatcher::directoryChanged, this, &FileWatcherModule::handleChangedInPath);

}

QString FileWatcherModule::watchPath() const{
    return mWatchPath;
}

void FileWatcherModule::setWatchPath(const QString &watchPath){
    if(mWatchPath == watchPath) return;
    mWatchPath = watchPath;
    persistSettings();
    emit watchPathChanged(mWatchPath);
}

QString FileWatcherModule::selectedPrinterName() const{
    return mSelectedPrinterName;
}

void FileWatcherModule::setSelectedPrinter(const QString &value){
    if(mSelectedPrinterName == value) return;
    mSelectedPrinterName = value;
    emit selectedPrinterChanged(mSelectedPrinterName);
    persistSettings();
}

QString FileWatcherModule::moveToPath() const{
    return mMoveToPath;
}

void FileWatcherModule::setMoveToPath(const QString &moveToPath){
    if(mMoveToPath == moveToPath) return;
    mMoveToPath = moveToPath;
    persistSettings();
    emit moveToPathChanged(mMoveToPath);
}

void FileWatcherModule::deleteWatcher(){
    emit deleted();

    //delete settings
    QSettings settings(qApp->applicationName(), "Watchers");
    settings.remove(mId);

    this->deleteLater();
}

void FileWatcherModule::persistSettings(){
    QSettings settings(qApp->applicationName(), "Watchers");

    QString content;
    content.append(mWatchPath);
    content.append("|");
    content.append(mSelectedPrinterName);
    content.append("|");
    content.append(mMoveToPath);

    settings.setValue(mId, content);
}

void FileWatcherModule::handleChangedInPath(const QString &filePath){
    QDir directory(filePath);

    //if dir was deleted...
    if(!directory.exists()){
        qDebug() << directory << "was deleted...";
        emit watchPathExists(false);
    }

    QStringList files = directory.entryList(QStringList() << "*.pdf", QDir::Files); //only names of the files eg "test.pdf"
    if(!files.isEmpty()){

        //there is the possibility that the signal to changed files is emitted just one time after many file changed - so we need to check every file in this slot

        //check which file is not printed yet and add it to our printlist with printcounter = 0
        for (const QString &f : files) {
            bool fileIsKnown = false;
            for (QPair<QString, int> content : mPrintableFileList) {
                if(content.first == filePath + "/" + f){
                    fileIsKnown = true;
                    break;
                }
            }
            if(!fileIsKnown){
                mPrintableFileList.append(QPair<QString, int>(filePath + "/" + f, 0));
            }
        }

        //print every file in printList with counter = 0 (never tried to printed before)

        for (QPair<QString, int> content : mPrintableFileList) {
            if(content.second == 0){
                printFile(content.first);
            }
        }
    }
}

void FileWatcherModule::printFile(QString filePath){
    if(!mCurrentFileInPrint.isEmpty()){
        qDebug() << "Printer is currently blocked by printing: " + mCurrentFileInPrint + " wait 2 seconds and try again";
        QTimer::singleShot(2000, [=]{
            printFile(filePath);
        });
        return;
    }

    QFile f(filePath);

    //remove from list and retur if deleted while wait
    if(!f.exists()){
        for (int i = 0; i < mPrintableFileList.count(); ++i) {
            if(mPrintableFileList.at(i).first == filePath){
                mPrintableFileList.removeAt(i);
                break;
            }
        }
        return;
    }

    qDebug() << __FUNCTION__ << filePath << "on printer" << mSelectedPrinterName;

    //count up the print tries or return if limit reached
    for (int i = 0; i < mPrintableFileList.count(); ++i) {
        if(mPrintableFileList.at(i).first == filePath){
            if(mPrintableFileList.at(i).second == MAXPRINTTRIES)
                return;

            mPrintableFileList.replace(i, QPair<QString, int>(filePath, (mPrintableFileList.at(i).second + 1)));
            break;
        }
    }

    //if file not readable wait for 2 seconds and try to print it again - dont forget a counter to ensure we dont get in a loop: break after 10 tries
    if(f.isOpen()){
        QTimer::singleShot(2000, [=]{
            printFile(filePath);
        });
        return;
    }

    filePath.replace("/", "\\"); //we need to replace slashes cause pdf printer need backslashes

    auto process = new QProcess;

    connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [=](int, QProcess::ExitStatus exitStatus){

        if(exitStatus == QProcess::NormalExit){
            mCurrentFileInPrint.clear();

            QFileInfo info(filePath);

            qDebug() << info.fileName() << " printed!";

            QFile file(filePath);
            if(file.exists()){
                QDir d(mMoveToPath);
                if(!d.exists()){
                    d.mkdir(mMoveToPath);
                    qDebug() << "goal dir " << mMoveToPath << " created";
                }

                const QString newPath = mMoveToPath + "/" + info.fileName();
                if(file.copy(newPath))
                    file.remove();
                qDebug() << info.fileName() << " moved to " << mMoveToPath;
            }

            //remove from print list
            for (int i = 0; i < mPrintableFileList.count(); ++i) {
                QString pathSlashFix = filePath;
                pathSlashFix = pathSlashFix.replace("\\", "/");
                if(mPrintableFileList.at(i).first == pathSlashFix){
                    mPrintableFileList.removeAt(i);
                    break;
                }
            }
        }

        process->deleteLater();
    });

    QString s = qApp->property("exePath").toString() + " \"" + filePath + "\" \"" + mSelectedPrinterName + "\"";
    qDebug() << "call print queue" << s;
    mCurrentFileInPrint = filePath;
    process->start(s);
}
