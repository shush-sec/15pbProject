#pragma once
#include "mysql.h"
#include <vector>
#include <string>
using namespace std;
typedef struct _RESULT
{
	vector<string>			vecColName;			//�ֶ�����
	vector<vector<string>>  vecData;		    //���ֶζ�Ӧ������
}RESULT,*P_RESULT;
class CDBHelper
{
public:
	CDBHelper();
	~CDBHelper();
	
	//�������ݿ�
	void conn(const char *host, const char *user, const char *passwd,
		const char *db, unsigned int port);
	//�û���½
	bool login(string name, string pwd);
	//�û�ע��
	bool addUser(string name, string pwd);
	//�����û�״̬ ��½��ǳ�
	bool updateActive(string name,bool );
	//���������û�
	vector<string> selectUsers();
	//ִ��sql
	void query(char * query);
	//��ӡ�ֶ�
	void printInfo();
	//��ӡ���ݼ�
	void printRows();
private:
	MYSQL * mysql;
	MYSQL_RES * resSql;	//���ݽ����
	int m_column;		//���ݽ��������
};

