#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static QTextEdit * mDebugTextEdit;

private slots:
    void on_pushButton_addWatcher_clicked();
    void on_pushButton_choosePDPrinterPath_clicked();
    void on_checkBox_autostart_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
