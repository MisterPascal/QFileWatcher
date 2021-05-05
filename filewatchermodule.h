#ifndef FILEWATCHERMODULE_H
#define FILEWATCHERMODULE_H

#include <QObject>

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

private:
    QString mId;
    QString mWatchPath;
    QString mMoveToPath;
    QFileSystemWatcher *mWatcher;
    QString mSelectedPrinterName;

    void persistSettings();

signals:
    void watchPathChanged(QString);
    void moveToPathChanged(QString);
    void selectedPrinterChanged(QString);
    void watchPathExists(bool exists);

};

#endif // FILEWATCHERMODULE_H
