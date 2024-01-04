#ifndef SHARETHREAD_H
#define SHARETHREAD_H

#include "InitSock.h"
#include <QThread>

class ShareThread : public QThread
{
    Q_OBJECT

public:
    explicit ShareThread(const QString& filePath, quint16 port, QObject *parent = nullptr);
    ~ShareThread();
    // 添加一个方法来停止线程
    void stopThread();

protected:
    void run() override;

signals:
    void updateInfo(const QString &info);

private:
    // Add any additional members or functions if needed
    QString filePath;
    quint16 port;
    bool stopFlag = false;  // 标志线程是否应该停止
    void shareServer(); // 文件分享服务
    void sendFile(SOCKET clientSocket, const char* filePath);
};


#endif // SHARETHREAD_H
