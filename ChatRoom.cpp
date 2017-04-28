// ChatRoom.cpp : 定义控制台应用程序的入口点。
//
#pragma once
#include "stdafx.h"
#include "MySocket.h"
#include "TCPServer.h"
#include "DBHelper.h"
int _tmain(int argc, _TCHAR* argv[])
{


	dbHelper.conn("127.0.0.1", "root", "15pbhackav", "chatroom", 3306);
	CTCPServer mySocket;
	mySocket.init();
	///获取ip地址
	char hostname[256];
	gethostname(hostname, 256);
	HOSTENT* hosts = gethostbyname(hostname);
	struct in_addr addr;
	memcpy(&addr, hosts->h_addr_list[0], sizeof(struct in_addr));

	mySocket.start(inet_ntoa(addr), 6666);
	
	system("pause");
	return 0;
}

