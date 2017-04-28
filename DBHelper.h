#pragma once
#include "mysql.h"
#include <vector>
#include <string>
using namespace std;
typedef struct _RESULT
{
	vector<string>			vecColName;			//字段名称
	vector<vector<string>>  vecData;		    //该字段对应的数据
}RESULT,*P_RESULT;
class CDBHelper
{
public:
	CDBHelper();
	~CDBHelper();
	
	//连接数据库
	void conn(const char *host, const char *user, const char *passwd,
		const char *db, unsigned int port);
	//用户登陆
	bool login(string name, string pwd);
	//用户注册
	bool addUser(string name, string pwd);
	//更新用户状态 登陆或登出
	bool updateActive(string name,bool );
	//搜索在线用户
	vector<string> selectUsers();
	//执行sql
	void query(char * query);
	//打印字段
	void printInfo();
	//打印数据集
	void printRows();
private:
	MYSQL * mysql;
	MYSQL_RES * resSql;	//数据结果集
	int m_column;		//数据结果集行数
};

