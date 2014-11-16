////////////////////////////////////////////////////////////////////////////////
// CppSQLite3U is a C++ unicode wrapper around the SQLite3 embedded database library.
//
// Copyright (c) 2006  Tyushkov Nikolay.  All Rights Reserved. http://softvoile.com
//
//
// Based on beautiful wrapper written by Rob Groves  
// (https://secure.codeproject.com/database/CppSQLite.asp). 
// Very good wrapper, but without unicode support unfortunately. 
// So, I have reconstructed it for unicode.
//
//  CppSQLite3 wrapper:
//  Copyright (c) 2004 Rob Groves. All Rights Reserved. rob.groves@btinternet.com
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose, without fee, and without a written
// agreement, is hereby granted, provided that the above copyright notice, 
// this paragraph and the following two paragraphs appear in all copies, 
// modifications, and distributions.
//
// IN NO EVENT SHALL THE AUTHOR BE LIABLE TO ANY PARTY FOR DIRECT,
// INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST
// PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
// EVEN IF THE AUTHOR HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// THE AUTHOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF
// ANY, PROVIDED HEREUNDER IS PROVIDED "AS IS". THE AUTHOR HAS NO OBLIGATION
// TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
// 
// If you want to get some documentation look at 
// https://secure.codeproject.com/database/CppSQLite.asp
// Note, not all features from CppSQLite3 were implemented in CppSQLite3U
//
// V1.0		11/06/2006	- Initial Public Version
//
//  Noteses : 
//   I have tested this wrapper only in unicode version, so I have no idea 
//   about its work in ANSI configuration, I think it doesn't work without modification;)
//
//  Home page : http://softvoile.com/development/CppSQLite3U/
//  Please send all bug report and comment to mail2@softvoile.com
// 
// 
////////////////////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <atlstr.h>
#include <tchar.h>
#include "CppSQLite3U.h"

/////////////////////////////////////////////////////////////////////////////
// CppSQLite3Exception

CppSQLite3Exception::CppSQLite3Exception(const int nErrCode,
									LPTSTR szErrMess,
									bool bDeleteMsg/*=true*/) :
									mnErrCode(nErrCode)
{
	mpszErrMess=new TCHAR[ szErrMess ? _tcslen(szErrMess)+50 : 50];
	_stprintf(mpszErrMess, _T("%s[%d]: %s"),
								errorCodeAsString(nErrCode),
								nErrCode,
								szErrMess ? szErrMess : _T(""));

	if (bDeleteMsg && szErrMess)
	{
		_sqlite3_free((char*)szErrMess);
	}
}


CppSQLite3Exception::CppSQLite3Exception(const CppSQLite3Exception&  e) :
									mnErrCode(e.mnErrCode)
{
	mpszErrMess = 0;
	if (e.mpszErrMess)
	{
		mpszErrMess=new TCHAR[_tcslen(e.mpszErrMess)+10];
		_stprintf(mpszErrMess, _T("%s"), e.mpszErrMess);
	}
}


LPCTSTR CppSQLite3Exception::errorCodeAsString(int nErrCode)
{
	switch (nErrCode)
	{
		case SQLITE_OK          : return _T("SQLITE_OK");
		case SQLITE_ERROR       : return _T("SQLITE_ERROR");
		case SQLITE_INTERNAL    : return _T("SQLITE_INTERNAL");
		case SQLITE_PERM        : return _T("SQLITE_PERM");
		case SQLITE_ABORT       : return _T("SQLITE_ABORT");
		case SQLITE_BUSY        : return _T("SQLITE_BUSY");
		case SQLITE_LOCKED      : return _T("SQLITE_LOCKED");
		case SQLITE_NOMEM       : return _T("SQLITE_NOMEM");
		case SQLITE_READONLY    : return _T("SQLITE_READONLY");
		case SQLITE_INTERRUPT   : return _T("SQLITE_INTERRUPT");
		case SQLITE_IOERR       : return _T("SQLITE_IOERR");
		case SQLITE_CORRUPT     : return _T("SQLITE_CORRUPT");
		case SQLITE_NOTFOUND    : return _T("SQLITE_NOTFOUND");
		case SQLITE_FULL        : return _T("SQLITE_FULL");
		case SQLITE_CANTOPEN    : return _T("SQLITE_CANTOPEN");
		case SQLITE_PROTOCOL    : return _T("SQLITE_PROTOCOL");
		case SQLITE_EMPTY       : return _T("SQLITE_EMPTY");
		case SQLITE_SCHEMA      : return _T("SQLITE_SCHEMA");
		case SQLITE_TOOBIG      : return _T("SQLITE_TOOBIG");
		case SQLITE_CONSTRAINT  : return _T("SQLITE_CONSTRAINT");
		case SQLITE_MISMATCH    : return _T("SQLITE_MISMATCH");
		case SQLITE_MISUSE      : return _T("SQLITE_MISUSE");
		case SQLITE_NOLFS       : return _T("SQLITE_NOLFS");
		case SQLITE_AUTH        : return _T("SQLITE_AUTH");
		case SQLITE_FORMAT      : return _T("SQLITE_FORMAT");
		case SQLITE_RANGE       : return _T("SQLITE_RANGE");
		case SQLITE_ROW         : return _T("SQLITE_ROW");
		case SQLITE_DONE        : return _T("SQLITE_DONE");
		case CPPSQLITE_ERROR    : return _T("CPPSQLITE_ERROR");
		default: return _T("UNKNOWN_ERROR");
	}
}


CppSQLite3Exception::~CppSQLite3Exception()
{
	if (mpszErrMess)
	{
 		delete [] mpszErrMess;
		mpszErrMess = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CppSQLite3DB

CppSQLite3DB::CppSQLite3DB()
{
	mpDB = 0;
	mnBusyTimeoutMs = 60000; // 60 seconds
}

CppSQLite3DB::CppSQLite3DB(const CppSQLite3DB& db)
{
	mpDB = db.mpDB;
	mnBusyTimeoutMs = 60000; // 60 seconds
}


CppSQLite3DB::~CppSQLite3DB()
{
	close();
}

////////////////////////////////////////////////////////////////////////////////

CppSQLite3DB& CppSQLite3DB::operator=(const CppSQLite3DB& db)
{
	mpDB = db.mpDB;
	mnBusyTimeoutMs = 60000; // 60 seconds
	return *this;
}

void CppSQLite3DB::open(LPCTSTR szFile)
{
    int nRet;

#if defined(_UNICODE) || defined(UNICODE)

	nRet = sqlite3_open16(szFile, &mpDB); // not tested under window 98 

#else // For Ansi Version
//*************-  Added by Begemot  szFile must be in unicode- 23/03/06 11:04 - ****
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx ((OSVERSIONINFO *) &osvi);
   
	if ( osvi.dwMajorVersion == 5) 
    {
          WCHAR pMultiByteStr[MAX_PATH+1];
          MultiByteToWideChar( CP_ACP, 0, szFile,
		                    _tcslen(szFile)+1, pMultiByteStr,   
			                sizeof(pMultiByteStr)/sizeof(pMultiByteStr[0]) );
          nRet = sqlite3_open16(pMultiByteStr, &mpDB);
    }
    else
          nRet = sqlite3_open(szFile,&mpDB);
#endif
//*************************
	if (nRet != SQLITE_OK)
	{
		LPCTSTR szError = (LPCTSTR) _sqlite3_errmsg(mpDB);
		throw CppSQLite3Exception(nRet, (LPTSTR)szError, DONT_DELETE_MSG);
	}
	setBusyTimeout(mnBusyTimeoutMs);
}

void CppSQLite3DB::close()
{
	if (mpDB)
	{
		int nRet = _sqlite3_close(mpDB);

		if (nRet != SQLITE_OK)
		{
			LPCTSTR szError = (LPCTSTR)_sqlite3_errmsg(mpDB);
			throw CppSQLite3Exception(nRet, (LPTSTR)szError, DONT_DELETE_MSG);
		}

		mpDB = 0;
	}
}


CppSQLite3Statement CppSQLite3DB::compileStatement(LPCTSTR szSQL)
{
	checkDB();

	sqlite3_stmt* pVM = compile(szSQL);

	return CppSQLite3Statement(mpDB, pVM);
}


bool CppSQLite3DB::tableExists(LPCTSTR szTable)
{
	TCHAR szSQL[128];
	_stprintf(szSQL, _T("select count(*) from sqlite_master where type='table' and name='%s'"),	szTable);
	int nRet = execScalar(szSQL);
	return (nRet > 0);
}


int CppSQLite3DB::execDML(LPCTSTR szSQL)
{
	int nRet;
	sqlite3_stmt* pVM; 
	checkDB();

	do{ 
		pVM = compile(szSQL);

		nRet = _sqlite3_step(pVM);
	
		if (nRet == SQLITE_ERROR)
		{
			LPCTSTR szError = (LPCTSTR) _sqlite3_errmsg(mpDB);
			throw CppSQLite3Exception(nRet, (LPTSTR)szError, DONT_DELETE_MSG);
		}
		nRet = _sqlite3_finalize(pVM);
	} 
	while( nRet == SQLITE_SCHEMA );
	return SQLITE_ERROR;
}

CppSQLite3Query CppSQLite3DB::execQuery(LPCTSTR szSQL)
{
	checkDB();
	int nRet;
	sqlite3_stmt* pVM; 
	
	do{ 
		pVM = compile(szSQL);

		nRet = _sqlite3_step(pVM);

		if (nRet == SQLITE_DONE)
		{	// no rows
			return CppSQLite3Query(mpDB, pVM, true/*eof*/);
		}
		else if (nRet == SQLITE_ROW)
		{	// at least 1 row
			return CppSQLite3Query(mpDB, pVM, false/*eof*/);
		}
		nRet = _sqlite3_finalize(pVM);
	} 
	while( nRet == SQLITE_SCHEMA ); // Edit By Begemot 08/16/06 12:44:35 -   read SQLite FAQ 
	
	LPCTSTR szError = (LPCTSTR) _sqlite3_errmsg(mpDB);
	throw CppSQLite3Exception(nRet, (LPTSTR)szError, DONT_DELETE_MSG);
}


int CppSQLite3DB::execScalar(LPCTSTR szSQL)
{
	CppSQLite3Query q = execQuery(szSQL);

	if (q.eof() || q.numFields() < 1)
		throw CppSQLite3Exception(CPPSQLITE_ERROR, _T("Invalid scalar query"),	DONT_DELETE_MSG);

	return _tstoi(q.fieldValue(0));
}

// Added By Begemot, exact as execScalar but return CString  08/06/06 16:30:37
CString CppSQLite3DB::execScalarStr(LPCTSTR szSQL)
{
	CppSQLite3Query q = execQuery(szSQL);

	if (q.eof() || q.numFields() < 1)
		throw CppSQLite3Exception(CPPSQLITE_ERROR, _T("Invalid scalar query"),	DONT_DELETE_MSG);
	
	return (CString)q.getStringField(0);
}

sqlite_int64 CppSQLite3DB::lastRowId()
{
	return sqlite3_last_insert_rowid(mpDB);
}


void CppSQLite3DB::setBusyTimeout(int nMillisecs)
{
	mnBusyTimeoutMs = nMillisecs;
	sqlite3_busy_timeout(mpDB, mnBusyTimeoutMs);
}


void CppSQLite3DB::checkDB()
{
	if (!mpDB)
		throw CppSQLite3Exception(CPPSQLITE_ERROR,_T("Database not open"), DONT_DELETE_MSG);
	
}


sqlite3_stmt* CppSQLite3DB::compile(LPCTSTR szSQL)
{	
	checkDB();
	sqlite3_stmt* pVM;

	int nRet = _sqlite3_prepare(mpDB, szSQL, -1, &pVM, NULL);

	if (nRet != SQLITE_OK)
	{
		pVM=NULL;
		LPCTSTR szError = (LPCTSTR) _sqlite3_errmsg(mpDB);
		throw CppSQLite3Exception(nRet, (LPTSTR)szError, DONT_DELETE_MSG);
	}
	return pVM;
}

bool CppSQLite3DB::loadOrSaveDb( LPCTSTR lpszDbFilePath, bool bSave/*=true*/ )
{
	int rc;
	sqlite3 *pFile;
	sqlite3_backup *pBackup;
	sqlite3 *pTo;
	sqlite3 *pFrom;
	rc = sqlite3_open16(lpszDbFilePath, &pFile);
	if(rc == SQLITE_OK && mpDB!=NULL)
	{
		pFrom =	(bSave ? mpDB : pFile );
		pTo =		(bSave ? pFile : mpDB);
		pBackup = sqlite3_backup_init(pTo,"main",pFrom,"main");
		if(pBackup)
		{
			(void)sqlite3_backup_step(pBackup,-1);
			(void)sqlite3_backup_finish(pBackup);
		}
		rc = sqlite3_errcode(pTo);
	}
	(void)sqlite3_close(pFile);
	return rc == 0;
}


//////////////////////// CppSQLite3Statement  ///////////////////////////////////////////
CppSQLite3Statement::CppSQLite3Statement()
{
	mpDB = 0;
	mpVM = 0;
}

CppSQLite3Statement::CppSQLite3Statement(const CppSQLite3Statement& rStatement)
{
	mpDB = rStatement.mpDB;
	mpVM = rStatement.mpVM;
	// Only one object can own VM
	const_cast<CppSQLite3Statement&>(rStatement).mpVM = 0;
}

CppSQLite3Statement::CppSQLite3Statement(sqlite3* pDB, sqlite3_stmt* pVM)
{
	mpDB = pDB;
	mpVM = pVM;
}

CppSQLite3Statement::~CppSQLite3Statement()
{
	try
	{
		finalize();
	}
	catch (...)	{}
}

CppSQLite3Statement& CppSQLite3Statement::operator=(const CppSQLite3Statement& rStatement)
{
	mpDB = rStatement.mpDB;
	mpVM = rStatement.mpVM;
	// Only one object can own VM
	const_cast<CppSQLite3Statement&>(rStatement).mpVM = 0;
	return *this;
}

int CppSQLite3Statement::execDML()
{
	checkDB();
	checkVM();

	int nRet = sqlite3_step(mpVM);

	if (nRet == SQLITE_DONE)
	{
		int nRowsChanged = sqlite3_changes(mpDB);

		nRet = sqlite3_reset(mpVM);

		if (nRet != SQLITE_OK)
		{
			LPCTSTR szError = (LPCTSTR) _sqlite3_errmsg(mpDB);
			throw CppSQLite3Exception(nRet, (LPTSTR)szError, DONT_DELETE_MSG);
		}
		return nRowsChanged;
	}
	else
	{
		nRet = sqlite3_reset(mpVM);
		LPCTSTR szError = (LPCTSTR) _sqlite3_errmsg(mpDB);
		throw CppSQLite3Exception(nRet, (LPTSTR)szError, DONT_DELETE_MSG);
	}
}


void CppSQLite3Statement::bind(int nParam, LPCTSTR szValue)
{
	checkVM();
	int nRes = _sqlite3_bind_text(mpVM, nParam, szValue, -1, SQLITE_TRANSIENT);
	if (nRes != SQLITE_OK)
		throw CppSQLite3Exception(nRes,_T("Error binding string param"), DONT_DELETE_MSG);
}


void CppSQLite3Statement::bind( int nParam, LPCTSTR szValue,int nLen )
{
	checkVM();
	int nRes = _sqlite3_bind_text(mpVM, nParam, szValue, nLen*2, SQLITE_TRANSIENT);
	if (nRes != SQLITE_OK)
		throw CppSQLite3Exception(nRes,_T("Error binding string param"), DONT_DELETE_MSG);
}


void CppSQLite3Statement::bind(int nParam, const int nValue)
{
	checkVM();
	int nRes = sqlite3_bind_int(mpVM, nParam, nValue);
	if (nRes != SQLITE_OK)
		throw CppSQLite3Exception(nRes,_T("Error binding int param"), DONT_DELETE_MSG);
}


void CppSQLite3Statement::bind(int nParam, const double dValue)
{
	checkVM();
	int nRes = sqlite3_bind_double(mpVM, nParam, dValue);
	if (nRes != SQLITE_OK)
		throw CppSQLite3Exception(nRes,	_T("Error binding double param"), DONT_DELETE_MSG);
}


void CppSQLite3Statement::bind(int nParam, const unsigned char* blobValue, int nLen)
{
	checkVM();
	int nRes = sqlite3_bind_blob(mpVM, nParam,(const void*)blobValue, nLen, SQLITE_TRANSIENT);
	if (nRes != SQLITE_OK)
		throw CppSQLite3Exception(nRes,_T("Error binding blob param"),DONT_DELETE_MSG);
}


void CppSQLite3Statement::bindNull(int nParam)
{
	checkVM();
	int nRes = sqlite3_bind_null(mpVM, nParam);

	if (nRes != SQLITE_OK)
  		throw CppSQLite3Exception(nRes,_T("Error binding NULL param"),DONT_DELETE_MSG);
}


void CppSQLite3Statement::reset()
{
	if (mpVM)
	{
		int nRet = sqlite3_reset(mpVM);

		if (nRet != SQLITE_OK)
		{
			LPCTSTR szError = (LPCTSTR) _sqlite3_errmsg(mpDB);
			throw CppSQLite3Exception(nRet, (LPTSTR)szError, DONT_DELETE_MSG);
		}
	}
}


void CppSQLite3Statement::finalize()
{
	if (mpVM)
	{
		int nRet = sqlite3_finalize(mpVM);
		mpVM = 0;

		if (nRet != SQLITE_OK)
		{
			LPCTSTR szError = (LPCTSTR) _sqlite3_errmsg(mpDB);
			throw CppSQLite3Exception(nRet, (LPTSTR)szError, DONT_DELETE_MSG);
		}
	}
}


void CppSQLite3Statement::checkDB()
{
	if (mpDB == 0) throw CppSQLite3Exception(CPPSQLITE_ERROR,_T("Database not open"), DONT_DELETE_MSG);
}

void CppSQLite3Statement::checkVM()
{
	if (mpVM == 0)
		throw CppSQLite3Exception(CPPSQLITE_ERROR,_T("Null Virtual Machine pointer"), DONT_DELETE_MSG);
}


/////////////////////  CppSQLite3Query  //////////////////////////////////////////////////
CppSQLite3Query::CppSQLite3Query()
{
	mpVM = 0;
	mbEof = true;
	mnCols = 0;
	mbOwnVM = false;
}


CppSQLite3Query::CppSQLite3Query(const CppSQLite3Query& rQuery)
{
	mpVM = rQuery.mpVM;
	// Only one object can own the VM
	const_cast<CppSQLite3Query&>(rQuery).mpVM = 0;
	mbEof = rQuery.mbEof;
	mnCols = rQuery.mnCols;
	mbOwnVM = rQuery.mbOwnVM;
}


CppSQLite3Query::CppSQLite3Query(sqlite3* pDB, sqlite3_stmt* pVM,
								 bool bEof,	 bool bOwnVM/*=true*/)
{
	mpDB = pDB;
	mpVM = pVM;
	mbEof = bEof;
	mnCols = _sqlite3_column_count(mpVM);
	mbOwnVM = bOwnVM;
}

CppSQLite3Query::~CppSQLite3Query()
{
	try
	{
		finalize();
	}
	catch (...) {}
}


CppSQLite3Query& CppSQLite3Query::operator=(const CppSQLite3Query& rQuery)
{
	try
	{
		finalize();
	}
	catch (...)	{ }

	mpVM = rQuery.mpVM;
	// Only one object can own the VM
	const_cast<CppSQLite3Query&>(rQuery).mpVM = 0;
	mbEof = rQuery.mbEof;
	mnCols = rQuery.mnCols;
	mbOwnVM = rQuery.mbOwnVM;
	return *this;
}


int CppSQLite3Query::numFields()
{
	checkVM();
	return mnCols;
}


LPCTSTR CppSQLite3Query::fieldValue(int nField)
{
	checkVM();

	if (nField < 0 || nField > mnCols-1)
		throw CppSQLite3Exception(CPPSQLITE_ERROR,_T("Invalid field index requested"),DONT_DELETE_MSG);

	return (LPCTSTR)_sqlite3_column_text(mpVM, nField);
}


LPCTSTR CppSQLite3Query::fieldValue(LPCTSTR szField)
{
	int nField = fieldIndex(szField);
	return (LPCTSTR)_sqlite3_column_text(mpVM, nField);
}


int CppSQLite3Query::getIntField(int nField, int nNullValue/*=0*/)
{
	if (fieldDataType(nField) == SQLITE_NULL)
	{
		return nNullValue;
	}
	else
	{
		return _sqlite3_column_int(mpVM, nField);
	}
}


int CppSQLite3Query::getIntField(LPCTSTR szField, int nNullValue/*=0*/)
{
	int nField = fieldIndex(szField);
	return getIntField(nField, nNullValue);
}


double CppSQLite3Query::getFloatField(int nField, double fNullValue/*=0.0*/)
{
	if (fieldDataType(nField) == SQLITE_NULL)
	{
		return fNullValue;
	}
	else
	{
		return _sqlite3_column_double(mpVM, nField);
	}
}


double CppSQLite3Query::getFloatField(LPCTSTR szField, double fNullValue/*=0.0*/)
{
	int nField = fieldIndex(szField);
	return getFloatField(nField, fNullValue);
}


LPCTSTR CppSQLite3Query::getStringField(int nField, LPCTSTR szNullValue/*=""*/)
{
	if (fieldDataType(nField) == SQLITE_NULL)
	{
		return szNullValue;
	}
	else
	{
		return (LPCTSTR)_sqlite3_column_text(mpVM, nField);
	}
}


LPCTSTR CppSQLite3Query::getStringField(LPCTSTR szField, LPCTSTR szNullValue/*=""*/)
{
	int nField = fieldIndex(szField);
	return getStringField(nField, szNullValue);
}


const unsigned char* CppSQLite3Query::getBlobField(int nField, int& nLen)
{
	checkVM();

	if (nField < 0 || nField > mnCols-1)
		throw CppSQLite3Exception(CPPSQLITE_ERROR,_T("Invalid field index requested"),DONT_DELETE_MSG);

	nLen = _sqlite3_column_bytes(mpVM, nField);
	return (const unsigned char*)sqlite3_column_blob(mpVM, nField);
}


const unsigned char* CppSQLite3Query::getBlobField(LPCTSTR szField, int& nLen)
{
	int nField = fieldIndex(szField);
	return getBlobField(nField, nLen);
}


bool CppSQLite3Query::fieldIsNull(int nField)
{
	return (fieldDataType(nField) == SQLITE_NULL);
}


bool CppSQLite3Query::fieldIsNull(LPCTSTR szField)
{
	int nField = fieldIndex(szField);
	return (fieldDataType(nField) == SQLITE_NULL);
}


int CppSQLite3Query::fieldIndex(LPCTSTR szField)
{
	checkVM();

	if (szField)
	{
		for (int nField = 0; nField < mnCols; nField++)
		{
			LPCTSTR szTemp = (LPCTSTR)_sqlite3_column_name(mpVM, nField);

			if (_tcscmp(szField, szTemp) == 0)
			{
				return nField;
			}
		}
	}
	throw CppSQLite3Exception(CPPSQLITE_ERROR,_T("Invalid field name requested"),DONT_DELETE_MSG);
}


LPCTSTR CppSQLite3Query::fieldName(int nCol)
{
	checkVM();

	if (nCol < 0 || nCol > mnCols-1)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,_T("Invalid field index requested"),DONT_DELETE_MSG);
	}
	return (LPCTSTR)_sqlite3_column_name(mpVM, nCol);
}


LPCTSTR CppSQLite3Query::fieldDeclType(int nCol)
{
	checkVM();

	if (nCol < 0 || nCol > mnCols-1)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,_T("Invalid field index requested"),DONT_DELETE_MSG);
	}
	return (LPCTSTR)_sqlite3_column_decltype(mpVM, nCol);
}


int CppSQLite3Query::fieldDataType(int nCol)
{
	checkVM();

	if (nCol < 0 || nCol > mnCols-1)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,_T("Invalid field index requested"), DONT_DELETE_MSG);
	}
	return _sqlite3_column_type(mpVM, nCol);
}


bool CppSQLite3Query::eof()
{
	checkVM();
	return mbEof;
}


void CppSQLite3Query::nextRow()
{
	checkVM();

	int nRet = _sqlite3_step(mpVM);

	if (nRet == SQLITE_DONE)
	{
		// no rows
		mbEof = true;
	}
	else if (nRet == SQLITE_ROW)
	{
		// more rows, nothing to do
	}
	else
	{
		nRet = _sqlite3_finalize(mpVM);
		mpVM = 0;
		LPCTSTR szError = (LPCTSTR)_sqlite3_errmsg(mpDB);
		throw CppSQLite3Exception(nRet,	(LPTSTR)szError, DONT_DELETE_MSG);
	}
}


void CppSQLite3Query::finalize()
{
	if (mpVM && mbOwnVM)
	{
		int nRet = _sqlite3_finalize(mpVM);
		mpVM = 0;
		if (nRet != SQLITE_OK)
		{
			LPCTSTR szError = (LPCTSTR)_sqlite3_errmsg(mpDB);
			throw CppSQLite3Exception(nRet, (LPTSTR)szError, DONT_DELETE_MSG);
		}
	}
}

void CppSQLite3Query::checkVM()
{
	if (mpVM == 0)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,_T("Null Virtual Machine pointer"),DONT_DELETE_MSG);
	}
}


////////////////////////////////////////////////////////////////////////////////
//**************************
//*************-  Added By Begemot - 28/02/06 20:25 - ****
CString DoubleQuotes(CString in)
{
	in.Replace(_T("\'"),_T("\'\'"));
	return in;
}
