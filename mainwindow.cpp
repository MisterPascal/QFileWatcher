#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "filewatchermodule.h"
#include "filewatcherwidget.h"

#include <QCoreApplication>
#include <QUuid>
#include <QSettings>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>

QTextEdit * MainWindow::mDebugTextEdit = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mDebugTextEdit = new QTextEdit();
    ui->verticalLayout_debug->addWidget(mDebugTextEdit);

    this->setWindowTitle(qApp->applicationName());
    ui->tabWidget->setCurrentIndex(0);

    //restore watchers from settings


    QSettings settingsWatchers(qApp->applicationName(), "Watchers");
    for(const QString &key : settingsWatchers.allKeys()){
        qDebug() << "found key" << key;
        FileWatcherModule *m = new FileWatcherModule(key, this);

        QStringList params = settingsWatchers.value(key).toString().split("|");

        m->setWatchPath(params.at(0));
        m->setSelectedPrinter(params.at(1));
        m->setMoveToPath(params.at(2));

        ui->verticalLayout_watchers->addWidget(new FileWatcherWidget(m));
    }

    QSettings settings(qApp->applicationName(), "Settings");
    qApp->setProperty("exePath", settings.value("PDFtoPrinterPath").toString());
    ui->label_pdfPrinterExePath->setText(qApp->property("exePath").toString());

    QSettings settingsAutoStart(QStringLiteral("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), QSettings::NativeFormat);
    if(!settingsAutoStart.value(qApp->applicationName()).isNull()){
        ui->checkBox_autostart->setChecked(true);
    }

    ui->label_info->setText(ui->label_info->text() + "\n Version " + qApp->applicationVersion());
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_pushButton_addWatcher_clicked(){
    ui->verticalLayout_watchers->addWidget(new FileWatcherWidget(new FileWatcherModule(QUuid::createUuid().toString(QUuid::WithoutBraces)), this));
}

void MainWindow::on_pushButton_choosePDPrinterPath_clicked(){
    QString exePath = QFileDialog::getOpenFileName(this, QStringLiteral("PDFtoPrinter wählen"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), tr("(*.exe)"));

    if(exePath.isEmpty()) return;

    qApp->setProperty("exePath", exePath);
    ui->label_pdfPrinterExePath->setText(exePath);

    QSettings settings(qApp->applicationName(), "Settings");
    settings.setValue("PDFtoPrinterPath", exePath);
}

void MainWindow::on_checkBox_autostart_stateChanged(int arg1){
    QSettings settings(QStringLiteral("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), QSettings::NativeFormat);

    if (arg1) {
        QString value = QCoreApplication::applicationFilePath(); //get absolute path of running exe
        QString apostroph = QString();

        value.replace(QStringLiteral("/"),QStringLiteral("\\"));
        value = apostroph + value + apostroph + QStringLiteral(" --argument");

        //write value to the register
        settings.setValue(qApp->applicationName(), value);
    }
    else {
        settings.remove(qApp->applicationName());
    }
}
