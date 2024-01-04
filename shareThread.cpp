#include "sharethread.h"

#include <QFileInfo>

ShareThread::ShareThread(const QString& filePath, quint16 port, QObject *parent)
    : QThread(parent), filePath(filePath), port(port)
{
    // Initialize any members if needed
}

ShareThread::~ShareThread()
{
    // Cleanup if needed
}

void ShareThread::stopThread()
{
    stopFlag = true;
    emit updateInfo("等待10s后关闭服务");
}

void ShareThread::run()
{
    // Make sure to emit signals if you want to update the GUI from the thread
    emit updateInfo("分享文件服务启动！");
    shareServer();
    emit updateInfo("分享文件服务关闭！");
}

void ShareThread::shareServer()
{
    // 使用 QFileInfo 获取文件名
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();

    // 获取C风格
    std::string std_fileName = fileName.toStdString();
    const char* c_filename = std_fileName.c_str();
    std::string std_filePath = filePath.toStdString();
    const char* c_filePath = std_filePath.c_str();

    // 创建监听套接字
    SOCKET serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SOCKET_ERROR == serverSocket)
    {
        emit updateInfo("Socket创建错误");
        return;
    }

    // 初始化一个地址结构
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // 绑定socket和本机地址
    if (SOCKET_ERROR == ::bind(serverSocket, (sockaddr*)&server_addr, sizeof(server_addr)))
    {
        emit updateInfo("服务绑定地址错误 "+QString::number(WSAGetLastError()));
        return;
    }

    // 监听
    if (SOCKET_ERROR == ::listen(serverSocket, 10))
    {
        emit updateInfo("服务监听错误 "+QString::number(WSAGetLastError()));
        return;
    }

    emit updateInfo("创建socket成功，在127.0.0.1:"+QString::number(port)+"上监听");
    emit updateInfo("发送文件为"+filePath);

    // 设置等待时间为5秒
    timeval timeout;
    timeout.tv_sec = 10;  // 秒
    timeout.tv_usec = 0; // 微秒

    // 使用fd_set设置
    fd_set readSet, writeSet;
    FD_ZERO(&readSet);
    FD_SET(serverSocket, &readSet);
    FD_ZERO(&writeSet);

    // 执行任务，确保在循环中检查 stopFlag
    while (!stopFlag)
    {
        // 复制fd_set，因为select会修改它
        fd_set tmpReadSet = readSet;
        fd_set tmpWriteSet = writeSet;

        // 运行select函数，选择响应套接字
        int res = select(0, &tmpReadSet, &tmpWriteSet, NULL, &timeout);
        if (res == SOCKET_ERROR)
        {
            emit updateInfo("select() failed: "+QString::number(WSAGetLastError()));
            break;
        }
        else if(res == 0)
        {
            emit updateInfo("等待时间超过10s, 超出等待时间限制");
            break;
        }

        if (FD_ISSET(serverSocket, &tmpReadSet))
        {
            // 接受新的连接
            sockaddr_in clientAddr;
            int clientAddrLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
            if (clientSocket != INVALID_SOCKET) {
                emit updateInfo(QString(inet_ntoa(clientAddr.sin_addr))+"客户端连接");
                FD_SET(clientSocket, &writeSet);
            }
        }

        // 发送文件
        for (int i = 0; i < tmpWriteSet.fd_count; i++)
        {
            // qDebug() << "文件名" << fileName << " " << fileName.toUtf8().length();
            // 发送文件名给用户
            if (::send(tmpWriteSet.fd_array[i], c_filename, strlen(c_filename), 0) == SOCKET_ERROR) {
                emit updateInfo("文件名发送失败");
                continue;
            }
            // 获取对应的地址信息
            sockaddr_in clientAddr;
            int clientAddrLen = sizeof(clientAddr);
            if (getsockname(tmpWriteSet.fd_array[i], (sockaddr*)&clientAddr, &clientAddrLen) == SOCKET_ERROR) {
                emit updateInfo("获取连接地址失败");
                continue;
            }
            emit updateInfo("正在向 "+QString(inet_ntoa(clientAddr.sin_addr))+" 发送 "+filePath+" 文件······");
            sendFile(tmpWriteSet.fd_array[i], c_filePath);
            emit updateInfo("发送完毕!");

            // 关闭套接字 将该套接字从FD_SET集合中移除
            closesocket(tmpWriteSet.fd_array[i]);
            FD_CLR(tmpWriteSet.fd_array[i], &writeSet);
        }
    }

    // 将剩余的套接字全部close掉
    closesocket(serverSocket);
    for(int i = 0; i < writeSet.fd_count; i++)
    {
        closesocket(writeSet.fd_array[i]);
    }
}

void ShareThread::sendFile(SOCKET clientSocket, const char* c_filePath)
{
    FILE* file = fopen(c_filePath, "rb");
    if (!file) {
        emit updateInfo("无法打开文件 : "+QString(c_filePath));
        return;
    }

    char buffer[MAX_BUFFER_SIZE];  /* 设置发送的缓冲区大小 */
    int bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (::send(clientSocket, buffer, bytesRead, 0) == SOCKET_ERROR) {
            emit updateInfo("发送文件数据失败");
            break;
        }
        memset(buffer, 0, MAX_BUFFER_SIZE);
    }

    fclose(file);
}
