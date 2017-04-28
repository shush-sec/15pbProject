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
	// 1. �źż��(��ʼ����̻���  WSAStartup)
	WSADATA wsd = {};
	if (WSAStartup(MAKEWORD(2, 2), &wsd)) {
		printf("��ʼ������ʧ�ܣ�");
		return 0;
	}
	// 1.1 �жϰ汾���Ƿ�ͨ��
	if (!(LOBYTE(wsd.wVersion) == 2 &&
		HIBYTE(wsd.wVersion) == 2)) {
		printf("�汾�ż�ⲻͨ����");
		WSACleanup();
		return 0;
	}
	return true;
}

//����socket��ʵ�����ͻ���socket
bool CTCPServer::start(char* strIP, unsigned short port)
{
	// 2. ���ֻ�(�����׽���  socket)
	SOCKET stServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (stServer == INVALID_SOCKET) {
		printf("�����׽���ʧ�ܣ�");
		WSACleanup();
		return false;
	}
	// 3. ���ֻ���(�󶨵�ַ �˿ں�  bind)
	SOCKADDR_IN addrServer = {};
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.S_un.S_addr = inet_addr(strIP);
	addrServer.sin_port = htons(port);
	int nRet = bind(stServer, (sockaddr*)&addrServer, sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR) {
		printf("��ʧ�ܣ�");
		return false;
	}
	// 4. ����(listen)
	nRet = listen(stServer, SOMAXCONN);
	if (nRet == SOCKET_ERROR) {
		printf("����ʧ�ܣ�");
		return false;
	}
	printf("���������ɹ���������:  %s:%d\n", strIP, port);


	while (true)
	{
		// 5. ��������(accept)
		SOCKADDR_IN addrClient = {};
		int nSize = sizeof(SOCKADDR_IN);
		//�ͻ���socket
		SOCKET stClient = accept(stServer, (sockaddr*)&addrClient, &nSize);
		if (stClient == INVALID_SOCKET) {
			printf("����ʧ�ܣ�");
			return false;
		}
		//���ݸ��̵߳�socket�ṹ
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
	char * msg = "��ӭ���������ң�";
	send(stClient, msg, strlen(msg) + 1, 0);
	//��½+ע��
	tcpServer.userLogin(stClient);
	//��ʼ���շ������� 
	while (true)
	{
		
		if (tcpServer.recvMessage(stClient) != SOCKET_ERROR)
		{
			//������������Ϣ
			tcpServer.sendMessage();
		}
	}
	return 0;
}
//����ͼƬ
void CTCPServer::getPic(char * buf, int nRet, SOCKET &stClient)
{

	// �����ļ���
	int nCount = 0;
	CHAR szName[50] = {};
	sprintf_s(szName, 50, "file_%d.jpg", nCount++);
	ofstream outFile(szName, ios_base::binary);
	outFile.write(buf, nRet);
	// ���洫������ͼƬ
	while (true)
	{
		char bufRecv[1024] = {};
		int nRet = recv(stClient, bufRecv, 1024, 0);
		//�����ݽ���
		/*for (int i = 0; i < strlen(bufRecv); i++)
		{
		bufRecv[i] = bufRecv[i] ^ KEY;
		}*/
		if (nRet == SOCKET_ERROR)
		{
			break;
		}
		if (nRet < 1024) {
			// ���ݶ�ȡ�������
			outFile.write(bufRecv, nRet);
			outFile.close();
			break;
		}
		else {
			// ������ȡ����
			outFile.write(bufRecv, nRet);
		}
	}

}
//ѭ������������Ϣ
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
//������������Ϣ
void CTCPServer::sendMessageRoom( MYMESSAGE myMsg)
{
	for (int j = 0; j < tcpServer.vClient.size(); j++)	//ѭ���ͻ��˶���
	{
		if (tcpServer.vClient[j].socket != myMsg.messageFrom)
		{
			char content[2048];
			sprintf_s(content, "%s˵:%s",myMsg.messageFromName,myMsg.messageContent );
			send(tcpServer.vClient[j].socket, content, strlen(content), 0);
		}
	}
}
//���͸�������Ϣ
void CTCPServer::sendMessageSingle(MYMESSAGE myMsg)
{
	for (int j = 0; j < tcpServer.vClient.size(); j++)	//ѭ���ͻ��˶���
	{
		if (!strcmp(tcpServer.vClient[j].name.c_str(), myMsg.messageToName))	//�ҵ��Է�
		{
			char content[2049];
			sprintf_s(content, "%s˵:%s", myMsg.messageFromName, myMsg.messageContent);
			send(myMsg.messageTo, content, strlen(content), 0);
		}
	}
}

//����������Ϣ
int CTCPServer::recvMessage(SOCKET &stClient)
{
	char buf[2048] = {};
	int nRet = recv(stClient, buf, 2048, 0);

	MYMESSAGE msg = { 0 };
	if (nRet > 0)
	{
		//�����ͻ��˷���������Ϣ
		
		char * result;
		char * content = NULL;

		if (memchr(buf, ':', 10) == NULL)		//Ⱥ����Ϣ
		{
			msg.messageType = sendMessage_room;
			strcpy_s(msg.messageContent, 2048, buf);
			strcpy_s(msg.messageFromName, 50,getUserNameBySocket(stClient).c_str());
		}
		else									//��Ⱥ����Ϣ
		{
			result = strtok_s(buf, ":", &content);
			if (!strcmp(result, "list"))			//���������û�
			{
				msg.messageType = getUserList;
				const char* name = getUserNameBySocket(stClient).c_str();
				strcpy_s(msg.messageFromName, 50, getUserNameBySocket(stClient).c_str());
			}
			else								//�û�����
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

		//����Ϣ�������vector<string> ��

		if (msg.messageType>0)
		{
			tcpServer.m_roomMessage.push_back(msg);
		}		
	}
	else 			//�ͻ��˶Ͽ�����
	{
		userLogout(stClient);
	}
	return nRet;
}
//�û��˳�
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
//��Զ��Э��
void CTCPServer::sendCMD(MYMESSAGE myMsg)
{
	for (int j = 0; j < tcpServer.vClient.size(); j++)	//ѭ���ͻ��˶���
	{
		if (!strcmp(tcpServer.vClient[j].name.c_str(), myMsg.messageToName))	//�ҵ��Է�
		{
			send(myMsg.messageTo, myMsg.messageContent, strlen(myMsg.messageContent), 0);
		}
	}
}

//�û���½
void CTCPServer::userLogin(SOCKET &stClient)
{

	char* strError;
	char* strCorrect;
	char* msg;
	char name[50] = {};
	char pwd[50] = {};

	while (true)
	{

		msg = "�������û�����(û���˺ţ�����reg: ��ʼע��)";
		send(stClient, msg, strlen(msg)+1, 0);
		recv(stClient, name, 50, 0);

		if (strcmp(name, "reg:") == 0)
		{
			userRegister(stClient);

		}
		else
		{
			msg = "���������룺";
			send(stClient, msg, strlen(msg) + 1, 0);

			recv(stClient, pwd, 50, 0);
			if (!dbHelper.login(name, pwd))
			{
				strError = "�û��������벻��ȷ��";
				send(stClient, strError, strlen(strError) + 1, 0);
			}
			else
			{
				strCorrect = "��½�ɹ��� (����list: �鿴�����û�)";
				send(stClient, strCorrect, strlen(strCorrect) + 1, 0);

				//�����û���Ϣ��vClient ����û���vector
				STCCLIENTNAME stcClientName;
				stcClientName.socket = stClient;
				stcClientName.name = name;
				tcpServer.vClient.push_back(stcClientName);
				return;
			}
			//���name��pwd
			memset(name, 0, 50);
			memset(pwd, 0, 50);

		}

	}
}

//�û�ע��
bool CTCPServer::userRegister(SOCKET &stClient)
{
	char* strFailed;
	char* strCorrect;
	char * msg = "��������Ҫע����û�����";
	send(stClient, msg, strlen(msg) + 1, 0);
	char name[4096] = {};
	recv(stClient, name, 4096, 0);
	msg = "���������룺";
	send(stClient, msg, strlen(msg) + 1, 0);
	char pwd[4096] = {};
	recv(stClient, pwd, 4096, 0);

	if (!dbHelper.addUser(name, pwd) ||!strcmp(name, "list") 
		|| !strcmp(name, "img") || !strcmp(name, "cmd"))
	{
		strFailed = "ע��ʧ�ܣ�";
		send(stClient, strFailed, strlen(strFailed) + 1, 0);
		return false;
	}
	else
	{
		strCorrect = "ע��ɹ���";
		send(stClient, strCorrect, strlen(strCorrect) + 1, 0);
		return true;
	}
} 

//��ʾ�����û�
void CTCPServer::selectUsers(MYMESSAGE myMsg)
{
	for (int i = 0; i < tcpServer.vClient.size(); i++)				//ѭ������û���
	{
		if (strcmp(tcpServer.vClient[i].name.c_str(), myMsg.messageFromName))		//����ʾ���������Լ�������
		{
			send(myMsg.messageFrom, tcpServer.vClient[i].name.c_str(), strlen(tcpServer.vClient[i].name.c_str()), 0);
		}
	}

	//ͨ����ѯ���ݿ�ķ���
	//vector<string> vClientName;
	//vClientName = dbHelper.selectUsers();
	//for (int i = 0; i < vClientName.size(); i++)				//ѭ������û���
	//{
	//	if (strcmp(vClientName[i].c_str(), myMsg.messageFromName))		//����ʾ���������Լ�������
	//	{
	//		send(myMsg.messageFrom, vClientName[i].c_str(), strlen(vClientName[i].c_str()), 0);
	//	}
	//}
}
//����socketid�ҵ��û���
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
//�����û����ҵ�socketid
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
