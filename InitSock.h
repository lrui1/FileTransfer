#pragma once
#include <WinSock2.h>

#pragma comment(lib, "WS2_32") // 链接到WS2_32.lib
#pragma warning(disable:4996) // 禁用过时警告

#define MAX_BUFFER_SIZE 1024
#define MAX_FILENAME_SIZE 256
#define MAX_INPUT_SIZE 25  // 用于存储用户输入的缓冲区大小

class CInitSock
{
public:
	CInitSock(BYTE minorVer = 2, BYTE majorVer = 2)
	{
		// 初始化 WS2_32.dll
		WSADATA wsaData;
		WORD sockVersion = MAKEWORD(minorVer, majorVer);
		if (::WSAStartup(sockVersion, &wsaData) != 0)
		{
			exit(0);
		}
	}
	~CInitSock()
	{
		::WSACleanup();
	}
};
