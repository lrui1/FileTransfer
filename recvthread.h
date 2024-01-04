#ifndef RECVTHREAD_H
#define RECVTHREAD_H

#include "InitSock.h"
#include <QThread>

class RecvThread : public QThread
{
    Q_OBJECT

public:
    explicit RecvThread(const QString& ipAddress, quint16 port, QObject *parent = nullptr);
    ~RecvThread();
    // 添加一个方法来停止线程
    void stopThread();

protected:
    void run() override;

signals:
    void updateInfo(const QString &info); // 日志信号
    void updateProgress(int percent); // 进度条信号

private:
    QString ipAddress; // ipv4
    quint16 port;
    qint64 totalBytesReceived;
    qint64 totalBytesToReceive;

    bool stopFlag = false;  // 标志线程是否应该停止
    void recvServer(); // 文件分享服务
    void recvFile(SOCKET clientSocket, const char* filename);  /* 从服务器接收文件 */
    int calProgress(); // 计算百分比
};

#endif // RECVTHREAD_H
