#ifndef FILEWATCHERMODULE_H
#define FILEWATCHERMODULE_H

#include <QObject>
#include <QPair>

class QFileSystemWatcher;
class QPrinterInfo;

class FileWatcherModule : public QObject
{
    Q_OBJECT
public:
    explicit FileWatcherModule(const QString id, QObject *parent = nullptr);

    QString watchPath() const;
    void setWatchPath(const QString &watchPath);

    QString selectedPrinterName() const;
    void setSelectedPrinter(const QString &value);

    QString moveToPath() const;
    void setMoveToPath(const QString &moveToPath);

    void deleteWatcher();

private:
    QString mId;
    QString mWatchPath;
    QString mMoveToPath;
    QFileSystemWatcher *mWatcher;
    QString mSelectedPrinterName;
    QList<QPair<QString, int>> mPrintableFileList; //first: absolute file path - second: print tries to stop after X tries
    QString mCurrentFileInPrint;

    void persistSettings();

private slots:
    void handleChangedInPath(const QString &filePath);
    void printFile(QString filePath);

signals:
    void watchPathChanged(QString);
    void moveToPathChanged(QString);
    void selectedPrinterChanged(QString);
    void watchPathExists(bool exists);
    void deleted();

};

#endif // FILEWATCHERMODULE_H
