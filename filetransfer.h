#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QWidget>
#include "shareThread.h"
#include "recvthread.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class FileTransfer;
}
QT_END_NAMESPACE

class FileTransfer : public QWidget
{
    Q_OBJECT

public:
    FileTransfer(QWidget *parent = nullptr);
    ~FileTransfer();

private slots:
    void on_selectFileButton_clicked();
    void on_connetButton_clicked();

    void on_closeButton_clicked();

    void on_startService_clicked();

    void on_shutService_clicked();

    void updateShareInfoSlot(const QString &info); // 更新分享服务信息槽

    void updateRecvInfoSlot(const QString &info); // 更新接收文件服务信息槽

private:
    Ui::FileTransfer *ui;
    ShareThread *shareThread;
    RecvThread * recvThread;
};
#endif // FILETRANSFER_H
