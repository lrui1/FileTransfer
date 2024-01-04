#include "recvthread.h"

RecvThread::RecvThread(const QString& ipAddress, quint16 port, QObject *parent)
    :ipAddress(ipAddress), port(port), QThread(parent)
{
    totalBytesReceived = 0;
    totalBytesToReceive = -1;
}

RecvThread::~RecvThread()
{

}

void RecvThread::stopThread()
{
    stopFlag = true;
    emit updateInfo("等待10s后关闭服务");
}

void RecvThread::run()
{
    emit updateInfo("接收文件服务开启!");
    recvServer();
    emit updateInfo("接收文件服务关闭!");
}

void RecvThread::recvServer()
{
    // 获取C风格ip
    std::string std_ipAddress = ipAddress.toStdString();
    const char * c_ipAddress = std_ipAddress.c_str();

    // 创建客户端套接字
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        emit updateInfo("套接字创建失败");
        return;
    }

    // 设置服务器地址信息
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(c_ipAddress);

    // 连接到服务器
    if (::connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        emit updateInfo("连接服务器失败");
        closesocket(clientSocket);
        return;
    }

    emit updateInfo("连接"+ipAddress+"成功!");

    // 设置等待时间为5秒
    timeval timeout;
    timeout.tv_sec = 10;  // 秒
    timeout.tv_usec = 0; // 微秒

    // 使用fd_set设置
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(clientSocket, &readSet);

    // 执行任务，确保在循环中检查 stopFlag
    while (!stopFlag)
    {
        FD_SET tmpReadSet = readSet;
        int res = select(0, &tmpReadSet, NULL, NULL, &timeout);
        if (res == SOCKET_ERROR)
        {
            emit updateInfo("select() failed: "+QString::number(WSAGetLastError()));
            return;
        }
        else if(res == 0)
        {
            emit updateInfo("等待时间超过10s, 超出等待时间限制");
            return;
        }

        // 从服务器获取文件名、大小
        char fileInput[MAX_BUFFER_SIZE];
        int nRecv = ::recv(clientSocket, fileInput, sizeof(fileInput), 0);
        if (nRecv > 0)
        {
            fileInput[nRecv] = '\0';
        }
        QString input(fileInput);
        QStringList parts = input.split('?');
        if(parts.size() < 2)
        {
            emit updateInfo("无法找到文件名和大小");
            break;
        }
        QString qFileName = parts[0];
        QString qFileSize = parts[1];
        // 转换C风格
        std::string std_FileName = qFileName.toStdString();
        std::string std_FileSize = qFileSize.toStdString();
        const char* c_filename = std_FileName.c_str();
        const char* c_filesize = std_FileSize.c_str();
        totalBytesToReceive = std::atoi(c_filesize);
        emit updateInfo("文件名为"+QString(c_filename)+", 大小为"+QString(c_filesize));
        emit updateInfo("正在接收······");
        recvFile(clientSocket, c_filename);
        emit updateInfo("接收完毕······\n");
        closesocket(clientSocket);
        stopThread(); // 停止线程
    }
}

void RecvThread::recvFile(SOCKET clientSocket, const char* filename)
{
    FILE* file = fopen(filename, "wb");
    if (!file) {
        emit updateInfo("无法创建文件: "+QString(filename));
        return;
    }

    char buffer[MAX_BUFFER_SIZE];
    int bytesReceived;

    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        // 写入文件，重新分配空间
        fwrite(buffer, 1, bytesReceived, file);
        memset(buffer, 0, MAX_BUFFER_SIZE);

        // 计算百分比，更新信号
        totalBytesReceived += bytesReceived;
        int percent = calProgress();
        emit updateProgress(percent);

    }
    fclose(file);
}

int RecvThread::calProgress()
{
    if (totalBytesToReceive > 0) {
        return static_cast<int>((totalBytesReceived * 100) / totalBytesToReceive);
    } else {
        return 0; // 避免除以零错误
    }
}
