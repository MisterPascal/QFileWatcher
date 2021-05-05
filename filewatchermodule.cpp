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

#include <QDebug>

FileWatcherModule::FileWatcherModule(const QString id, QObject *parent) : QObject(parent), mId(id){

    mWatcher = new QFileSystemWatcher(this);

    connect(this, &FileWatcherModule::watchPathChanged, this, [=]{
        mWatcher->removePaths(mWatcher->files());
        mWatcher->addPath(mWatchPath);
        qDebug() << "watch path" << mWatchPath;
    });

    connect(mWatcher, &QFileSystemWatcher::directoryChanged, this, [=](QString path){
       qDebug() << "directory changed" <<  path;

       QDir directory(path);

       //if dir was deleted...
       if(!directory.exists()){
           qDebug() << directory << "was deleted...";
           emit watchPathExists(false);
       }

       QStringList files = directory.entryList(QStringList() << "*.pdf", QDir::Files);
       if(!files.isEmpty()){

           qDebug() << "print" << files.first();
           qDebug() << "on printer" << mSelectedPrinterName;
           path.replace("/", "\\"); //we need to replace slashes cause pdf printer need backslashes

           auto thread = new QThread;
           auto process = new QProcess;

           connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
               [=](int, QProcess::ExitStatus exitStatus){

               if(exitStatus == QProcess::NormalExit){
                   QFile file(path + "/" + files.first());
                   if(file.exists()){
                       QDir d(mMoveToPath);
                       if(!d.exists())
                           d.mkdir(mMoveToPath);

                       const QString newPath = mMoveToPath + "/" + files.first();
                       if(file.copy(newPath))
                           file.remove();
                   }
               }

               process->deleteLater();
               process->thread()->quit();
           });

           QString s = qApp->property("exePath").toString() + " \"" + path + "\\" + files.first() + "\" \"" + mSelectedPrinterName + "\"";
           qDebug() << s;
           process->start(s);
           process->moveToThread(thread);

           // Move the thread **handle** to the main thread
           thread->moveToThread(qApp->thread());
           connect(thread, &QThread::finished, thread, &QObject::deleteLater);
           thread->start();

       }
    });
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
