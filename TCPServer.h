#pragma once
//该结构体用于向线程函数传递socket和addr
typedef struct _STCCLIENT
{
	SOCKET stClient;
	in_addr addr;
}STCCLIENT;
//
typedef struct _STCCLIENTNAME
{
	SOCKET socket;
	string name;		//用户名
}STCCLIENTNAME;


//消息结构体
typedef struct _MYMESSAGE
{
	int messageType;		//消息类型，
	SOCKET messageFrom;		//消息作者	socketID
	SOCKET messageTo;
	char messageFromName[50];	//发送方用户名
	char messageToName[50];		//对方用户名
	char messageContent[2048];	//消息内容
}MYMESSAGE;

//消息类型
enum MessageType
{
	sendRegister = 1,				//发送注册消息
	sendLogin,
	sendMessage_room,				//给聊天室发送文字
	sendPic_room,					//给聊天室发送图片
	sendMessage_singl,				//给好友单独发送文字
	sendPic_singl,					//给好友单独发送图片
	sendMessage_CMD,						//发送cmd
	getUserList,					//获取所有在线用户列表
	getFirendList,					//获取好友列表
	addFriend,						//添加好友
	other,							//其他类型信息
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
	//查找在线用户
	void selectUsers(MYMESSAGE myMsg);
public:
	in_addr addr;
	vector<MYMESSAGE> m_roomMessage;
	vector<STCCLIENTNAME> vClient;
};

