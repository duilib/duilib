#ifndef __DBManager_H__
#define __DBManager_H__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	DBManager.h
// ������		: 	daviyang35@QQ.com
// ����ʱ��	:	2013-12-24 11:57:29
// ����޸�	:	2013-12-24 11:57:29
// ˵��			:	SQLite�������ݿ���࣬�ṩ�����򿪡��رա�CRUD����
���ݿⰴ�� Key-��Value ��ṹ�洢
����Value��UTF8�ַ�����ʽд��

���·����ǻ����ӿ�
Set()
Query()
Delete()
������ѯ���ýӿ�Ϊ�ṩ��������ת��������

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
