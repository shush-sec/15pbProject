#include "stdafx.h"
#include "TCPServer.h"
#pragma comment(lib,"ws2_32.lib")
#include <fstream>

using namespace std;
#define KEY 0x666

DWORD WINAPI clientThread(_In_ LPVOID lpParameter);
CTCPServer tcpServer;

CTCPServer::CTCPServer()
{
}
CTCPServer::~CTCPServer()
{
}

bool CTCPServer::init()
{
	// 1. 信号检测(初始化编程环境  WSAStartup)
	WSADATA wsd = {};
	if (WSAStartup(MAKEWORD(2, 2), &wsd)) {
		printf("初始化环境失败！");
		return 0;
	}
	// 1.1 判断版本号是否通过
	if (!(LOBYTE(wsd.wVersion) == 2 &&
		HIBYTE(wsd.wVersion) == 2)) {
		printf("版本号检测不通过！");
		WSACleanup();
		return 0;
	}
	return true;
}

//监听socket并实例化客户端socket
bool CTCPServer::start(char* strIP, unsigned short port)
{
	// 2. 买手机(创建套接字  socket)
	SOCKET stServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (stServer == INVALID_SOCKET) {
		printf("创建套接字失败！");
		WSACleanup();
		return false;
	}
	// 3. 绑定手机号(绑定地址 端口号  bind)
	SOCKADDR_IN addrServer = {};
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.S_un.S_addr = inet_addr(strIP);
	addrServer.sin_port = htons(port);
	int nRet = bind(stServer, (sockaddr*)&addrServer, sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR) {
		printf("绑定失败！");
		return false;
	}
	// 4. 监听(listen)
	nRet = listen(stServer, SOMAXCONN);
	if (nRet == SOCKET_ERROR) {
		printf("监听失败！");
		return false;
	}
	printf("服务启动成功，服务器:  %s:%d\n", strIP, port);


	while (true)
	{
		// 5. 接收连接(accept)
		SOCKADDR_IN addrClient = {};
		int nSize = sizeof(SOCKADDR_IN);
		//客户端socket
		SOCKET stClient = accept(stServer, (sockaddr*)&addrClient, &nSize);
		if (stClient == INVALID_SOCKET) {
			printf("连接失败！");
			return false;
		}
		//传递给线程的socket结构
		STCCLIENT stcClient;
		stcClient.stClient = stClient;
		stcClient.addr = addr;

		HANDLE hThread = CreateThread(NULL, 0, clientThread, &stcClient, 0, 0);
	}


	return 0;
}
bool CTCPServer::exit()
{
	return false;
}

DWORD WINAPI clientThread(_In_ LPVOID lpParameter)
{
	STCCLIENT * pstcClient = (STCCLIENT*)lpParameter;
	SOCKET stClient = pstcClient->stClient;
	char * msg = "欢迎来到聊天室！";
	send(stClient, msg, strlen(msg) + 1, 0);
	//登陆+注册
	tcpServer.userLogin(stClient);
	//开始接收发送数据 
	while (true)
	{
		
		if (tcpServer.recvMessage(stClient) != SOCKET_ERROR)
		{
			//发送聊天室消息
			tcpServer.sendMessage();
		}
	}
	return 0;
}
//接收图片
void CTCPServer::getPic(char * buf, int nRet, SOCKET &stClient)
{

	// 构造文件名
	int nCount = 0;
	CHAR szName[50] = {};
	sprintf_s(szName, 50, "file_%d.jpg", nCount++);
	ofstream outFile(szName, ios_base::binary);
	outFile.write(buf, nRet);
	// 保存传过来的图片
	while (true)
	{
		char bufRecv[1024] = {};
		int nRet = recv(stClient, bufRecv, 1024, 0);
		//将数据解密
		/*for (int i = 0; i < strlen(bufRecv); i++)
		{
		bufRecv[i] = bufRecv[i] ^ KEY;
		}*/
		if (nRet == SOCKET_ERROR)
		{
			break;
		}
		if (nRet < 1024) {
			// 数据读取到最后了
			outFile.write(bufRecv, nRet);
			outFile.close();
			break;
		}
		else {
			// 正常读取数据
			outFile.write(bufRecv, nRet);
		}
	}

}
//循环发送聊天信息
void CTCPServer::sendMessage()
{
	vector<MYMESSAGE>::iterator strIterator;
	strIterator = tcpServer.m_roomMessage.begin();
	for (int i = 0; i < tcpServer.m_roomMessage.size(); i++)
	{
		switch (tcpServer.m_roomMessage[i].messageType)
		{
		case sendMessage_room:
			sendMessageRoom( tcpServer.m_roomMessage[i]);
			break;
		case sendMessage_CMD:
			sendCMD(tcpServer.m_roomMessage[i]);
			break;
		case sendMessage_singl:
			sendMessageSingle(tcpServer.m_roomMessage[i]);
			break;
		case sendPic_singl:
			break;
		case getUserList:
			selectUsers(tcpServer.m_roomMessage[i]);
			break;
		case other:
			break;
		default:
			break;
		}
		
	}
	if (tcpServer.m_roomMessage.size()> 0)
	{
		strIterator = tcpServer.m_roomMessage.erase(strIterator);
	}	

}
//发送聊天室消息
void CTCPServer::sendMessageRoom( MYMESSAGE myMsg)
{
	for (int j = 0; j < tcpServer.vClient.size(); j++)	//循环客户端队列
	{
		if (tcpServer.vClient[j].socket != myMsg.messageFrom)
		{
			char content[2048];
			sprintf_s(content, "%s说:%s",myMsg.messageFromName,myMsg.messageContent );
			send(tcpServer.vClient[j].socket, content, strlen(content), 0);
		}
	}
}
//发送给好友消息
void CTCPServer::sendMessageSingle(MYMESSAGE myMsg)
{
	for (int j = 0; j < tcpServer.vClient.size(); j++)	//循环客户端队列
	{
		if (!strcmp(tcpServer.vClient[j].name.c_str(), myMsg.messageToName))	//找到对方
		{
			char content[2049];
			sprintf_s(content, "%s说:%s", myMsg.messageFromName, myMsg.messageContent);
			send(myMsg.messageTo, content, strlen(content), 0);
		}
	}
}

//接收聊天信息
int CTCPServer::recvMessage(SOCKET &stClient)
{
	char buf[2048] = {};
	int nRet = recv(stClient, buf, 2048, 0);

	MYMESSAGE msg = { 0 };
	if (nRet > 0)
	{
		//解析客户端发过来的消息
		
		char * result;
		char * content = NULL;

		if (memchr(buf, ':', 10) == NULL)		//群聊消息
		{
			msg.messageType = sendMessage_room;
			strcpy_s(msg.messageContent, 2048, buf);
			strcpy_s(msg.messageFromName, 50,getUserNameBySocket(stClient).c_str());
		}
		else									//非群聊消息
		{
			result = strtok_s(buf, ":", &content);
			if (!strcmp(result, "list"))			//搜索在线用户
			{
				msg.messageType = getUserList;
				const char* name = getUserNameBySocket(stClient).c_str();
				strcpy_s(msg.messageFromName, 50, getUserNameBySocket(stClient).c_str());
			}
			else								//用户单聊
			{
				strcpy_s(msg.messageToName, 50, result);
				msg.messageTo = getSocketByUserName(result);
				strcpy_s(msg.messageFromName, 50, getUserNameBySocket(stClient).c_str());
				if (strstr(content, "cmd:") != NULL)
				{
					msg.messageType = sendMessage_CMD;
			
				}
				else
				{
					msg.messageType = sendMessage_singl;
					
				}
				strcpy_s(msg.messageContent, 2048, content);
				
			}
		}
		msg.messageFrom = stClient;

		//将消息打包放入vector<string> 中

		if (msg.messageType>0)
		{
			tcpServer.m_roomMessage.push_back(msg);
		}		
	}
	else 			//客户端断开连接
	{
		userLogout(stClient);
	}
	return nRet;
}
//用户退出
void CTCPServer::userLogout(SOCKET &stClient)
{
	vector<STCCLIENTNAME>::iterator clientIterator;
	clientIterator = tcpServer.vClient.begin();

	while (clientIterator != tcpServer.vClient.end())
	{
		try
		{
			if (clientIterator->socket == stClient)
			{
			
				clientIterator = tcpServer.vClient.erase(clientIterator);

			}
			else
			{
				++clientIterator;
			}
		}
		catch (exception &e)
		{
			printf(e.what());
		}
	}
}
//简单远程协助
void CTCPServer::sendCMD(MYMESSAGE myMsg)
{
	for (int j = 0; j < tcpServer.vClient.size(); j++)	//循环客户端队列
	{
		if (!strcmp(tcpServer.vClient[j].name.c_str(), myMsg.messageToName))	//找到对方
		{
			send(myMsg.messageTo, myMsg.messageContent, strlen(myMsg.messageContent), 0);
		}
	}
}

//用户登陆
void CTCPServer::userLogin(SOCKET &stClient)
{

	char* strError;
	char* strCorrect;
	char* msg;
	char name[50] = {};
	char pwd[50] = {};

	while (true)
	{

		msg = "请输入用户名：(没有账号？输入reg: 开始注册)";
		send(stClient, msg, strlen(msg)+1, 0);
		recv(stClient, name, 50, 0);

		if (strcmp(name, "reg:") == 0)
		{
			userRegister(stClient);

		}
		else
		{
			msg = "请输入密码：";
			send(stClient, msg, strlen(msg) + 1, 0);

			recv(stClient, pwd, 50, 0);
			if (!dbHelper.login(name, pwd))
			{
				strError = "用户名或密码不正确！";
				send(stClient, strError, strlen(strError) + 1, 0);
			}
			else
			{
				strCorrect = "登陆成功！ (输入list: 查看在线用户)";
				send(stClient, strCorrect, strlen(strCorrect) + 1, 0);

				//保存用户信息到vClient 存放用户的vector
				STCCLIENTNAME stcClientName;
				stcClientName.socket = stClient;
				stcClientName.name = name;
				tcpServer.vClient.push_back(stcClientName);
				return;
			}
			//清空name和pwd
			memset(name, 0, 50);
			memset(pwd, 0, 50);

		}

	}
}

//用户注册
bool CTCPServer::userRegister(SOCKET &stClient)
{
	char* strFailed;
	char* strCorrect;
	char * msg = "请输入想要注册的用户名：";
	send(stClient, msg, strlen(msg) + 1, 0);
	char name[4096] = {};
	recv(stClient, name, 4096, 0);
	msg = "请输入密码：";
	send(stClient, msg, strlen(msg) + 1, 0);
	char pwd[4096] = {};
	recv(stClient, pwd, 4096, 0);

	if (!dbHelper.addUser(name, pwd) ||!strcmp(name, "list") 
		|| !strcmp(name, "img") || !strcmp(name, "cmd"))
	{
		strFailed = "注册失败！";
		send(stClient, strFailed, strlen(strFailed) + 1, 0);
		return false;
	}
	else
	{
		strCorrect = "注册成功！";
		send(stClient, strCorrect, strlen(strCorrect) + 1, 0);
		return true;
	}
} 

//显示在线用户
void CTCPServer::selectUsers(MYMESSAGE myMsg)
{
	for (int i = 0; i < tcpServer.vClient.size(); i++)				//循环输出用户名
	{
		if (strcmp(tcpServer.vClient[i].name.c_str(), myMsg.messageFromName))		//不显示给请求者自己的名字
		{
			send(myMsg.messageFrom, tcpServer.vClient[i].name.c_str(), strlen(tcpServer.vClient[i].name.c_str()), 0);
		}
	}

	//通过查询数据库的方法
	//vector<string> vClientName;
	//vClientName = dbHelper.selectUsers();
	//for (int i = 0; i < vClientName.size(); i++)				//循环输出用户名
	//{
	//	if (strcmp(vClientName[i].c_str(), myMsg.messageFromName))		//不显示给请求者自己的名字
	//	{
	//		send(myMsg.messageFrom, vClientName[i].c_str(), strlen(vClientName[i].c_str()), 0);
	//	}
	//}
}
//根据socketid找到用户名
string  CTCPServer::getUserNameBySocket(SOCKET &stClient)
{
	for (int i = 0; i < tcpServer.vClient.size(); i++)
	{
		if (tcpServer.vClient[i].socket == stClient)
		{
			return tcpServer.vClient[i].name;
		}
	}
}
//根据用户名找到socketid
SOCKET CTCPServer::getSocketByUserName(char * name)
{
	for (int i = 0; i < tcpServer.vClient.size(); i++)
	{
		if (!strcmp(tcpServer.vClient[i].name.c_str(), name))
		{
			return tcpServer.vClient[i].socket;
		}
	}
}
