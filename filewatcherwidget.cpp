#include "filewatcherwidget.h"
#include "ui_filewatcherwidget.h"

#include <QPrinterInfo>
#include <QStandardPaths>
#include <QFileDialog>
#include <QDebug>

FileWatcherWidget::FileWatcherWidget(FileWatcherModule *m, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileWatcherWidget),
    mModule(m)
{
    ui->setupUi(this);
    ui->label_errorMsg->setVisible(false);

    ui->comboBox_printer->blockSignals(true);
    for (QPrinterInfo inf : QPrinterInfo::availablePrinters()) {
        ui->comboBox_printer->addItem(inf.description(), QVariant(inf.printerName()));
    }
    ui->comboBox_printer->blockSignals(false);

    ui->label_path->setText(mModule->watchPath());
    connect(mModule, &FileWatcherModule::watchPathChanged, ui->label_path, &QLabel::setText);

    ui->label_moveToPath->setText(mModule->moveToPath());
    connect(mModule, &FileWatcherModule::moveToPathChanged, ui->label_moveToPath, &QLabel::setText);

    ui->comboBox_printer->setCurrentText(mModule->selectedPrinterName());
    connect(mModule, &FileWatcherModule::selectedPrinterChanged, ui->comboBox_printer, &QComboBox::setCurrentText);

    connect(mModule, &FileWatcherModule::watchPathExists, this, [=](bool exists){
        if(!exists){
            ui->label_errorMsg->setText(QStringLiteral("Fehler: Überwachungsordner nicht gefunden!"));
            ui->label_errorMsg->setVisible(true);
        }
        else{
            ui->label_errorMsg->clear();
            ui->label_errorMsg->setVisible(false);
        }
    });
}

FileWatcherWidget::~FileWatcherWidget(){
    delete ui;
}

void FileWatcherWidget::on_pushButton_changePath_clicked(){
    mModule->setWatchPath(QFileDialog::getExistingDirectory(this, QStringLiteral("Überwachungsverzeichnis wählen"),
                                                            QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                            QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks));
}

void FileWatcherWidget::on_lineEdit_path_textEdited(const QString &arg1){
    mModule->setWatchPath(arg1);
}

void FileWatcherWidget::on_comboBox_printer_currentIndexChanged(int index){
    mModule->setSelectedPrinter(ui->comboBox_printer->itemData(index).toString());
}

void FileWatcherWidget::on_pushButton_moveToPath_clicked(){
    mModule->setMoveToPath(QFileDialog::getExistingDirectory(this, QStringLiteral("Verzeichnis wählen"),
                                                            QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                            QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks));

}
