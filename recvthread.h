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
    void updateInfo(const QString &info);

private:
    QString ipAddress; // ipv4
    quint16 port;
    bool stopFlag = false;  // 标志线程是否应该停止
    void recvServer(); // 文件分享服务
    void recvFile(SOCKET clientSocket, const char* filename);  /* 从服务器接收文件 */
};

#endif // RECVTHREAD_H
