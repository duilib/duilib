#ifndef __DBManager_H__
#define __DBManager_H__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	DBManager.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2013-12-24 11:57:29
// 最后修改	:	2013-12-24 11:57:29
// 说明			:	SQLite本地数据库基类，提供基础打开、关闭、CRUD服务
数据库按照 Key-》Value 表结构存储
所有Value按UTF8字符串方式写入

以下方法是基础接口
Set()
Query()
Delete()
其他查询设置接口为提供数据类型转换的重载

//*////////////////////////////////////////////////////////////
#pragma once
#include "CppSQLite3U.h"
#include <vector>

typedef struct _stAttributeInfo
{
	CString AttName;
	CString AttType;
	CString AttDesc;
}AttributeInfo;

typedef std::vector<AttributeInfo> AttributeInfoVector;


class CDBManager
{
public:
	CDBManager(void);
	~CDBManager(void);

	BOOL	Load(LPCTSTR lpszDbPath);
	BOOL	Load(LPCTSTR lpszDbPath,LPCTSTR TableName,AttributeInfoVector &pAttributeInfo);
	VOID	Close();
	BOOL	IsOpend();

	//////////////////////////////////////////////////////////////////////////
	// Query
	BOOL Query(LPCTSTR lpszField,CString *Value);
	BOOL QueryAnsiString(LPCTSTR lpszField,CStringA *Value);

	BOOL QueryInt(LPCTSTR lpszField,int *Value);
	BOOL QueryFloat(LPCTSTR lpszField,float *Value);
	BOOL QueryDouble(LPCTSTR lpszField,double *Value);
	//BOOL QueryBinary(LPCTSTR lpszField,CMemBuffer * Value);

	//////////////////////////////////////////////////////////////////////////
	// Delete
	int Delete(LPCTSTR lpszField);
	
	//////////////////////////////////////////////////////////////////////////
	// Create & Update
	BOOL Set(LPCTSTR lpszField,LPCTSTR Value);
	BOOL SetInt(LPCTSTR lpszField,int szValue);
	BOOL SetFloat(LPCTSTR lpszField,float szValue);
	BOOL SetDouble(LPCTSTR lpszField,double szValue);
	//BOOL SetBinary(LPCTSTR lpszField,LPVOID pData,DWORD dwLength);

    BOOL IsExistKey(LPCTSTR lpszKey);
private:
	CppSQLite3DB*	m_pDb;
	CString m_strDbPath;
};
#endif // __DBManager_H__
