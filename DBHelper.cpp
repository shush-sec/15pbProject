
#include "stdafx.h"
#include "mysql.h"
#include "DBHelper.h"
#include "iostream"

#pragma comment(lib, "libmysql.lib")

CDBHelper::CDBHelper()
{
	if ((mysql = mysql_init(NULL)) == NULL)
	{
		printf("failed in mysql_init!");
	}
}
CDBHelper::~CDBHelper()
{
	if (mysql)
		mysql_close(mysql);
}
//------------------------------------------------------------------------------  

void CDBHelper::conn(const char *host, const char *user, const char *passwd,
	const char *db, unsigned int port)
{
	if (mysql_real_connect(mysql, host, user, passwd, db,
		port,NULL,0) == NULL)
		printf("failed in mysql_connect!");

}

// 
void CDBHelper::query(char *query)
{
	// mysql_query Returns:  
	// Zero if the query was successful. Non-zero if an error occurred.   
	if (mysql_query(mysql, query))
		printf("failed in mysql_query!");

	resSql = mysql_store_result(mysql); // 保存结果集  
}

bool CDBHelper::addUser(string name, string pwd)
{
	char bufSql[100] = {};
	MYSQL_RES *result;
	sprintf_s(bufSql, "insert into userinfo(username,userpassword) values('%s','%s')", name.c_str(), pwd.c_str());

	if (mysql_query(mysql, bufSql))
	{
		return false;
	}
	else
	{
		if ((ULONG)mysql_affected_rows(mysql)>0) //mysql_affected_rows 获取影响的行数
		{
			return true;
		}
	}

	return false;
}
bool CDBHelper::login(string name, string pwd)
{
	if (!strcmp(name.c_str(), "") && !strcmp(pwd.c_str(), ""))
	{
		return true;
	}
	char bufSql[100] = {};
	MYSQL_RES *result;
	sprintf_s(bufSql, "select * from userinfo where username='%s' and userpassword='%s'", name.c_str(), pwd.c_str());
	
	if (mysql_query(mysql, bufSql))
	{
		return false;

	}
	else
	{
		result = mysql_store_result(mysql);
		if (result->row_count>0)  // there are rows
		{
			//更新用户状态 isActive ->1;
			updateActive(name,true);
			return true;
		}
	}
	
	return false;
}
vector<string> CDBHelper::selectUsers()
{
	char bufSql[100] = {};
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	vector<string> usersName;
	sprintf_s(bufSql, "select username from userinfo where isactive = 1");

	if (mysql_query(mysql, bufSql))
	{
		return usersName;
	}
	else
	{
		result = mysql_store_result(mysql);
		if (result)
		{
			while (sql_row = mysql_fetch_row(result))//获取具体的数据
			{
				usersName.push_back(sql_row[0]);
			}
		}
	}
	return usersName;
}
bool CDBHelper::updateActive(string name,bool isActive)
{
	char bufSqlActive[100] = {};
	if (isActive)
	{
		sprintf_s(bufSqlActive, "update userinfo set isactive = 1 where username = '%s'", name.c_str());
		mysql_query(mysql, bufSqlActive);
	}
	else
	{
		sprintf_s(bufSqlActive, "update userinfo set isactive = 0 where username = '%s'", name.c_str());
		mysql_query(mysql, bufSqlActive);
	}
	return true;
}
void CDBHelper::printInfo()
{

	m_column = 0;
	MYSQL_FIELD* sqlfield;
	while (sqlfield = mysql_fetch_field(resSql)) //遍历字段  
	{
		cout << '[' << sqlfield->name << ']';
		m_column++;
	}
	cout << endl;
}


void CDBHelper::printRows()
{
	MYSQL_ROW sqlrow;
	while (sqlrow = mysql_fetch_row(resSql)) // 遍历结果集  
	{
		for ( int i = 0; i < 2; i++) // 遍历每列  
		{
			if (i == 0)
			{
				cout << "<"<< sqlrow[i] << ">  ";
			}
			else
			{
				cout << sqlrow[i];
			}
		}
		cout << endl;
	}
}