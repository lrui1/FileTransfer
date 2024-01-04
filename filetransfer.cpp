#include "filetransfer.h"
#include "ui_filetransfer.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QThread>

FileTransfer::FileTransfer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileTransfer)
{
    ui->setupUi(this);
    // connect(shareThread, &ShareThread::updateInfo, this, &FileTransfer::updateShareInfoSlot); // 不能再这里绑定，因为线程对象还没创建，空指针报错
}

FileTransfer::~FileTransfer()
{
    delete ui;
}

void FileTransfer::on_connetButton_clicked()
{
    QString ipAddress = ui->ipAddress->text();
    quint16 port = ui->remotePort->value();
    ui->progressBar->setValue(0); //进度条归0
    recvThread = new RecvThread(ipAddress, port, this);
    connect(recvThread, &RecvThread::updateInfo, this, &FileTransfer::updateRecvInfoSlot);
    connect(recvThread, &RecvThread::updateProgress, this, &FileTransfer::updateProgressBar); //连接信号和槽
    connect(recvThread, &RecvThread::finished, recvThread, &QObject::deleteLater); // 自动释放线程
    recvThread->start();
}

void FileTransfer::on_closeButton_clicked()
{
    if(recvThread)
        recvThread->stopThread();
}

void FileTransfer::on_selectFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择文件"), QDir::homePath(), tr("All Files (*);;Text Files (*.txt);;Images (*.png *.xpm *.jpg)"));
    ui->filePathLineEdit->setText(filePath);
}

void FileTransfer::on_startService_clicked()
{
    QString filePath = ui->filePathLineEdit->text();
    quint16 port = ui->remotePort->value();
    shareThread = new ShareThread(filePath, port, this);
    connect(shareThread, &ShareThread::updateInfo, this, &FileTransfer::updateShareInfoSlot); // 信号与槽绑定
    connect(shareThread, &ShareThread::finished, shareThread, &QObject::deleteLater); // 自动释放线程
    shareThread->start();
}


void FileTransfer::on_shutService_clicked()
{
    if (shareThread)
        shareThread->stopThread();
}

void FileTransfer::updateShareInfoSlot(const QString &info)
{
    // Handle updates from the thread, e.g., update the UI
    ui->shareInfo->append(info);
}

void FileTransfer::updateRecvInfoSlot(const QString &info)
{
    ui->recvInfo->append(info);
}

void FileTransfer::updateProgressBar(int percent)
{
    // 更新进度条
    ui->progressBar->setValue(percent);
}


void FileTransfer::on_clearRecvInfoButton_clicked()
{
    ui->recvInfo->setText("");
}


void FileTransfer::on_clearShareInfoButton_clicked()
{
    ui->shareInfo->setText("");
}

