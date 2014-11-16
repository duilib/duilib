//#include "StdAfx.h"
#include "DBManager.h"



CDBManager::CDBManager(void)
: m_pDb(NULL)
{
	m_pDb = new CppSQLite3DB();
}

CDBManager::~CDBManager(void)
{
	Close();
	delete m_pDb;
}

BOOL CDBManager::Load( LPCTSTR lpszDbPath )
{
	m_strDbPath = lpszDbPath;

	do 
	{
		if ( m_pDb == NULL)
			break;

		try
		{
			m_pDb->open(lpszDbPath);
			if ( m_pDb->mpDB == NULL)
				break;

			if ( m_pDb->tableExists(_T("Config")) == FALSE)
			{
				/*
				return m_pDb->execDML(_T("CREATE TABLE [Config] ( \
					[Field] NVARCHAR(32)  UNIQUE NOT NULL PRIMARY KEY, \
					[Data] TEXT  NOT NULL \
					)")) == 1 ? TRUE : FALSE;
				*/
				int ret = 0;
				ret = m_pDb->execDML(_T("CREATE TABLE [Config] ( \
										[Field] NVARCHAR(32)  UNIQUE NOT NULL PRIMARY KEY, \
										[Data] TEXT NOT NULL\
 										)")) == 1 ? TRUE : FALSE;
				return ret;
			}
		}
		catch (CppSQLite3Exception e)
		{
			//g_logger.Info(_T("CppSQLite3Exception:%d %s"),e.errorCode(),e.errorMessage());
			return FALSE;
		}

		return TRUE;
	} while (FALSE);

	return FALSE;
}

BOOL CDBManager::Load( LPCTSTR lpszDbPath,LPCTSTR TableName,AttributeInfoVector &pAttributeInfo )
{
	m_strDbPath = lpszDbPath;

	do 
	{
		if ( m_pDb == NULL)
			break;

		try
		{
			m_pDb->open(lpszDbPath);
			if ( m_pDb->mpDB == NULL)
				break;

			if ( m_pDb->tableExists(TableName) == FALSE)
			{
				/*
				return m_pDb->execDML(_T("CREATE TABLE [Config] ( \
					[Field] NVARCHAR(32)  UNIQUE NOT NULL PRIMARY KEY, \
					[Data] TEXT  NOT NULL \
					)")) == 1 ? TRUE : FALSE;
				*/
				int ret = 0;
				CString Sql;
				Sql.Append(_T("CREATE TABLE ["));
				Sql.Append(TableName);
				Sql.Append(_T("]( "));
				for(unsigned i=0;i<pAttributeInfo.size();i++)
				{
					Sql.Append(_T("["));
					Sql.Append(pAttributeInfo[i].AttName);
					Sql.Append(_T("] "));
					Sql.Append(pAttributeInfo[i].AttType);
					Sql.Append(_T(" "));
					Sql.Append(pAttributeInfo[i].AttDesc);
					if((i+1)<pAttributeInfo.size())
					{
						Sql.Append(_T(", "));
					}	
				}
				Sql.Append(_T(")"));

				ret = m_pDb->execDML(Sql) == 1 ? TRUE : FALSE;
				return ret;
			}
		}
		catch (CppSQLite3Exception e)
		{
			//g_logger.Info(_T("CppSQLite3Exception:%d %s"),e.errorCode(),e.errorMessage());
			return FALSE;
		}

		return TRUE;
	} while (FALSE);

	return FALSE;
}


VOID CDBManager::Close()
{
	if ( m_pDb != NULL)
		m_pDb->close();
}

BOOL CDBManager::IsOpend()
{
	return m_pDb !=NULL && m_pDb->mpDB !=NULL;
}

BOOL CDBManager::Query( LPCTSTR lpszField,CString *Value )
{
	if ( IsOpend() == FALSE || Value == NULL)
		return FALSE;

	Value->Empty();
	if ( IsExistKey(lpszField) == FALSE)
		return FALSE;

	CString strSQL;
	strSQL.Format(_T("select Data from Config where Field='%s'"),lpszField);
	*Value = m_pDb->execScalarStr(strSQL);
	return TRUE;
}

BOOL CDBManager::QueryAnsiString( LPCTSTR lpszField,CStringA *Value )
{
	return FALSE;
}

BOOL CDBManager::QueryInt( LPCTSTR lpszField,int *Value )
{
	return FALSE;
}

BOOL CDBManager::QueryFloat( LPCTSTR lpszField,float *Value )
{
	return FALSE;
}

BOOL CDBManager::QueryDouble( LPCTSTR lpszField,double *Value )
{
	return FALSE;
}

int CDBManager::Delete( LPCTSTR lpszField )
{
	if ( IsExistKey(lpszField))
	{
		CString strSQL;
		strSQL.Format(_T("delete from Config where Field='%s'"),lpszField);
		return m_pDb->execDML(strSQL);
	}

	return 0;
}

BOOL CDBManager::Set( LPCTSTR lpszField,LPCTSTR Value )
{
	if ( IsOpend() == FALSE)
		return FALSE;

	CString strSQL;
	if ( IsExistKey(lpszField) )
		strSQL.Format(_T("update Config set Data='%s' where Field='%s'"),Value,lpszField);
	else
		strSQL.Format(_T("insert into Config values('%s','%s')"),lpszField,Value);

	return m_pDb->execDML(strSQL) == 1;
}

BOOL CDBManager::SetInt( LPCTSTR lpszField,int szValue )
{
	return FALSE;
}

BOOL CDBManager::SetFloat( LPCTSTR lpszField,float szValue )
{
	return FALSE;
}

BOOL CDBManager::SetDouble( LPCTSTR lpszField,double szValue )
{
	return FALSE;
}

BOOL CDBManager::IsExistKey( LPCTSTR lpszKey )
{
	CString strSQL;
	strSQL.Format(_T("select count(*) from Config where Field='%s'"),lpszKey);
	return m_pDb->execScalar(strSQL) == 1;
}
