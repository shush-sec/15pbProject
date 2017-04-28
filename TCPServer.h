#pragma once
//�ýṹ���������̺߳�������socket��addr
typedef struct _STCCLIENT
{
	SOCKET stClient;
	in_addr addr;
}STCCLIENT;
//
typedef struct _STCCLIENTNAME
{
	SOCKET socket;
	string name;		//�û���
}STCCLIENTNAME;


//��Ϣ�ṹ��
typedef struct _MYMESSAGE
{
	int messageType;		//��Ϣ���ͣ�
	SOCKET messageFrom;		//��Ϣ����	socketID
	SOCKET messageTo;
	char messageFromName[50];	//���ͷ��û���
	char messageToName[50];		//�Է��û���
	char messageContent[2048];	//��Ϣ����
}MYMESSAGE;

//��Ϣ����
enum MessageType
{
	sendRegister = 1,				//����ע����Ϣ
	sendLogin,
	sendMessage_room,				//�������ҷ�������
	sendPic_room,					//�������ҷ���ͼƬ
	sendMessage_singl,				//�����ѵ�����������
	sendPic_singl,					//�����ѵ�������ͼƬ
	sendMessage_CMD,						//����cmd
	getUserList,					//��ȡ���������û��б�
	getFirendList,					//��ȡ�����б�
	addFriend,						//��Ӻ���
	other,							//����������Ϣ
};
class CTCPServer
{
public:
	CTCPServer();
	virtual ~CTCPServer();
	bool init();
	bool start(char*  strIP, unsigned short port);
	bool exit();
	int recvMessage(SOCKET &stClient);
	int recvMessageSingle(SOCKET &stClient);
	void getPic(char * buf, int nRet, SOCKET &stClient);
	void sendMessage();
	void sendMessageRoom( MYMESSAGE myMsg);
	void sendMessageSingle(MYMESSAGE myMsg);
	void sendCMD(MYMESSAGE myMsg);
	void userLogin(SOCKET &stClient);
	void userLogout(SOCKET &stClient);
	bool userRegister(SOCKET &stClient);
	string getUserNameBySocket(SOCKET &stClient);
	SOCKET getSocketByUserName(char * name);
	//���������û�
	void selectUsers(MYMESSAGE myMsg);
public:
	in_addr addr;
	vector<MYMESSAGE> m_roomMessage;
	vector<STCCLIENTNAME> vClient;
};

