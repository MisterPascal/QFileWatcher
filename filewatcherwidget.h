#ifndef FILEWATCHERWIDGET_H
#define FILEWATCHERWIDGET_H

#include <QWidget>

#include "filewatchermodule.h"

namespace Ui {
class FileWatcherWidget;
}

class FileWatcherWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileWatcherWidget(FileWatcherModule *m, QWidget *parent = nullptr);
    ~FileWatcherWidget();

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private slots:
    void on_pushButton_changePath_clicked();
    void on_comboBox_printer_currentIndexChanged(int index);
    void on_pushButton_moveToPath_clicked();
    void on_pushButton_delete_clicked();

private:
    Ui::FileWatcherWidget *ui;
    FileWatcherModule *mModule;
};

#endif // FILEWATCHERWIDGET_H
