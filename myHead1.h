#pragma once
#include "InitSock.h"

#define MAX_BUFFER_SIZE 1024
#define MAX_FILENAME_SIZE 256
#define MAX_INPUT_SIZE 25  // 用于存储用户输入的缓冲区大小

void start();  /* 菜单 */
void shareServer(); /* 分享文件 */
void recvClient();  /* 接收文件 */
void sendFile(SOCKET clientSocket, const char* filename);  /* 发送文件到clientSocket */
void recvFile(SOCKET clientSocket, const char* filename);  /* 从服务器接收文件 */