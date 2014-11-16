////////////////////////////////////////////////////////////////////////////////
// CppSQLite3U is a C++ unicode wrapper around the SQLite3 embedded database library.
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
////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	CppSQLite3U.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2013-8-2 21:34:05
// 最后修改	:	2013-8-2 21:34:05
// 说明			:	SQLite Unicode Warpper类
//*////////////////////////////////////////////////////////////
#if !defined(AFX_CPPSQLITE3U_H__1B1BE273_2D1E_439C_946F_3CBD1C0EFD2F__INCLUDED_)
#define AFX_CPPSQLITE3U_H__1B1BE273_2D1E_439C_946F_3CBD1C0EFD2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "sqlite3.h"
#include <WTypes.h>
#include <tchar.h>
#include <atlstr.h>
/////////////////////////////////////////////////////////////////////////////

#define SQL_MAXSIZE    2048

#define CPPSQLITE_ERROR 1000
static const bool DONT_DELETE_MSG=false;


CString DoubleQuotes(CString in);

class CppSQLite3Query;
class CppSQLite3Statement;

class CppSQLite3Exception
{
public:

    CppSQLite3Exception(const int nErrCode,
					    LPTSTR  szErrMess,
					    bool bDeleteMsg=true);

    CppSQLite3Exception(const CppSQLite3Exception&  e);

    virtual ~CppSQLite3Exception();

    const int errorCode() { return mnErrCode; }

    LPCTSTR errorMessage() { return mpszErrMess; }

    static LPCTSTR  errorCodeAsString(int nErrCode);

private:

    int mnErrCode;
    LPTSTR  mpszErrMess;
};


class CppSQLite3DB 
{
// Construction
public:
	CppSQLite3DB();

// Operations
public:

    virtual ~CppSQLite3DB();

    void open(LPCTSTR szFile);

    void close();
    bool tableExists(LPCTSTR szTable);
    int execDML(LPCTSTR szSQL);

    CppSQLite3Query execQuery(LPCTSTR szSQL);

    int execScalar(LPCTSTR szSQL);
	CString execScalarStr(LPCTSTR szSQL);

    CppSQLite3Statement compileStatement(LPCTSTR szSQL);

    sqlite_int64 lastRowId();

    void interrupt() { sqlite3_interrupt(mpDB); }

    void setBusyTimeout(int nMillisecs);

	bool loadOrSaveDb(LPCTSTR lpszDbFilePath, bool bSave=true);

    static const char* SQLiteVersion() { return SQLITE_VERSION; }

private:

    CppSQLite3DB(const CppSQLite3DB& db);
    CppSQLite3DB& operator=(const CppSQLite3DB& db);

    sqlite3_stmt* compile(LPCTSTR szSQL);

    void checkDB();
public:
    sqlite3* mpDB;
    int mnBusyTimeoutMs;
};
/////////////////////////////////////////////////////////////////////////////

class CppSQLite3Statement
{
public:

	CppSQLite3Statement();

	CppSQLite3Statement(const CppSQLite3Statement& rStatement);

	CppSQLite3Statement(sqlite3* pDB, sqlite3_stmt* pVM);

	virtual ~CppSQLite3Statement();

	CppSQLite3Statement& operator=(const CppSQLite3Statement& rStatement);

	int execDML();

	CppSQLite3Query execQuery();

	void bind(int nParam, LPCTSTR szValue);
	void bind(int nParam, LPCTSTR szValue,int nLen);
	void bind(int nParam, const int nValue);
	void bind(int nParam, const double dwValue);
	void bind(int nParam, const unsigned char* blobValue, int nLen);
	void bindNull(int nParam);

	void reset();

	void finalize();

private:

	void checkDB();
	void checkVM();

	sqlite3* mpDB;
	sqlite3_stmt* mpVM;
};
/////////////////////  CppSQLite3Query  //////////////////////////////////////////////////
class CppSQLite3Query
{
public:

	CppSQLite3Query();

	CppSQLite3Query(const CppSQLite3Query& rQuery);

	CppSQLite3Query(sqlite3* pDB,
		sqlite3_stmt* pVM,
		bool bEof,
		bool bOwnVM=true);

	CppSQLite3Query& operator=(const CppSQLite3Query& rQuery);

	virtual ~CppSQLite3Query();

	int numFields();

	int fieldIndex(LPCTSTR szField);
	LPCTSTR fieldName(int nCol);

	LPCTSTR fieldDeclType(int nCol);
	int fieldDataType(int nCol);

	LPCTSTR fieldValue(int nField);
	LPCTSTR fieldValue(LPCTSTR szField);

	int getIntField(int nField, int nNullValue=0);
	int getIntField(LPCTSTR szField, int nNullValue=0);

	double getFloatField(int nField, double fNullValue=0.0);
	double getFloatField(LPCTSTR szField, double fNullValue=0.0);

	LPCTSTR getStringField(int nField, LPCTSTR szNullValue=_T(""));
	LPCTSTR getStringField(LPCTSTR szField, LPCTSTR szNullValue=_T(""));

	const unsigned char* getBlobField(int nField, int& nLen);
	const unsigned char* getBlobField(LPCTSTR szField, int& nLen);

	bool fieldIsNull(int nField);
	bool fieldIsNull(LPCTSTR szField);

	bool eof();
	void nextRow();
	void finalize();

private:

	void checkVM();

	sqlite3* mpDB;
	sqlite3_stmt* mpVM;
	bool mbEof;
	int mnCols;
	bool mbOwnVM;
};

/////////////////////////////////////////////////////////////////////////////
//
// TCHAR based sqlite3 function names for Unicode/MCBS builds.
//
#if defined(_UNICODE) || defined(UNICODE)
#pragma message("Unicode Selected")
#define _sqlite3_aggregate_context      sqlite3_aggregate_context
#define _sqlite3_aggregate_count        sqlite3_aggregate_count
#define _sqlite3_bind_blob              sqlite3_bind_blob
#define _sqlite3_bind_double            sqlite3_bind_double
#define _sqlite3_bind_int               sqlite3_bind_int
#define _sqlite3_bind_int64             sqlite3_bind_int64
#define _sqlite3_bind_null              sqlite3_bind_null
#define _sqlite3_bind_parameter_count   sqlite3_bind_parameter_count
#define _sqlite3_bind_parameter_index   sqlite3_bind_parameter_index
#define _sqlite3_bind_parameter_name    sqlite3_bind_parameter_name
#define _sqlite3_bind_text              sqlite3_bind_text16
#define _sqlite3_bind_text16            sqlite3_bind_text16
#define _sqlite3_busy_handler           sqlite3_busy_handler
#define _sqlite3_busy_timeout           sqlite3_busy_timeout
#define _sqlite3_changes                sqlite3_changes
#define _sqlite3_close                  sqlite3_close
#define _sqlite3_collation_needed       sqlite3_collation_needed16
#define _sqlite3_collation_needed16     sqlite3_collation_needed16
#define _sqlite3_column_blob            sqlite3_column_blob
#define _sqlite3_column_bytes           sqlite3_column_bytes16
#define _sqlite3_column_bytes16         sqlite3_column_bytes16
#define _sqlite3_column_count           sqlite3_column_count
#define _sqlite3_column_decltype        sqlite3_column_decltype16
#define _sqlite3_column_decltype16      sqlite3_column_decltype16
#define _sqlite3_column_double          sqlite3_column_double
#define _sqlite3_column_int             sqlite3_column_int
#define _sqlite3_column_int64           sqlite3_column_int64
#define _sqlite3_column_name            sqlite3_column_name16
#define _sqlite3_column_name16          sqlite3_column_name16
#define _sqlite3_column_text            sqlite3_column_text16
#define _sqlite3_column_text16          sqlite3_column_text16
#define _sqlite3_column_type            sqlite3_column_type
#define _sqlite3_commit_hook            sqlite3_commit_hook
#define _sqlite3_complete               sqlite3_complete16
#define _sqlite3_complete16             sqlite3_complete16
#define _sqlite3_create_collation       sqlite3_create_collation16
#define _sqlite3_create_collation16     sqlite3_create_collation16
#define _sqlite3_create_function        sqlite3_create_function16
#define _sqlite3_create_function16      sqlite3_create_function16
#define _sqlite3_data_count             sqlite3_data_count
#define _sqlite3_errcode                sqlite3_errcode
#define _sqlite3_errmsg                 sqlite3_errmsg16
#define _sqlite3_errmsg16               sqlite3_errmsg16
#define _sqlite3_exec                   sqlite3_exec
#define _sqlite3_finalize               sqlite3_finalize
#define _sqlite3_free                   sqlite3_free
#define _sqlite3_free_table             sqlite3_free_table
#define _sqlite3_get_table              sqlite3_get_table
#define _sqlite3_interrupt              sqlite3_interrupt
#define _sqlite3_last_insert_rowid      sqlite3_last_insert_rowid
#define _sqlite3_libversion             sqlite3_libversion
#define _sqlite3_mprintf                sqlite3_mprintf
#define _sqlite3_open                   sqlite3_open16
#define _sqlite3_open16                 sqlite3_open16
#define _sqlite3_prepare                sqlite3_prepare16
#define _sqlite3_prepare16              sqlite3_prepare16
#define _sqlite3_progress_handler       sqlite3_progress_handler
#define _sqlite3_reset                  sqlite3_reset
#define _sqlite3_result_blob            sqlite3_result_blob
#define _sqlite3_result_double          sqlite3_result_double
#define _sqlite3_result_error           sqlite3_result_error16
#define _sqlite3_result_error16         sqlite3_result_error16
#define _sqlite3_result_int             sqlite3_result_int
#define _sqlite3_result_int64           sqlite3_result_int64
#define _sqlite3_result_null            sqlite3_result_null
#define _sqlite3_result_text            sqlite3_result_text16
#define _sqlite3_result_text16          sqlite3_result_text16
#define _sqlite3_result_text16be        sqlite3_result_text16be
#define _sqlite3_result_text16le        sqlite3_result_text16le
#define _sqlite3_result_value           sqlite3_result_value
#define _sqlite3_set_authorizer         sqlite3_set_authorizer
#define _sqlite3_step                   sqlite3_step
#define _sqlite3_total_changes          sqlite3_total_changes
#define _sqlite3_trace                  sqlite3_trace
#define _sqlite3_user_data              sqlite3_user_data
#define _sqlite3_value_blob             sqlite3_value_blob
#define _sqlite3_value_bytes            sqlite3_value_bytes16
#define _sqlite3_value_bytes16          sqlite3_value_bytes16
#define _sqlite3_value_double           sqlite3_value_double
#define _sqlite3_value_int              sqlite3_value_int
#define _sqlite3_value_int64            sqlite3_value_int64
#define _sqlite3_value_text             sqlite3_value_text16
#define _sqlite3_value_text16           sqlite3_value_text16
#define _sqlite3_value_text16be         sqlite3_value_text16be
#define _sqlite3_value_text16le         sqlite3_value_text16le
#define _sqlite3_value_type             sqlite3_value_type
#define _sqlite3_vmprintf               sqlite3_vmprintf
#else
#pragma message("MCBS Selected")
#define _sqlite3_aggregate_context      sqlite3_aggregate_context
#define _sqlite3_aggregate_count        sqlite3_aggregate_count
#define _sqlite3_bind_blob              sqlite3_bind_blob
#define _sqlite3_bind_double            sqlite3_bind_double
#define _sqlite3_bind_int               sqlite3_bind_int
#define _sqlite3_bind_int64             sqlite3_bind_int64
#define _sqlite3_bind_null              sqlite3_bind_null
#define _sqlite3_bind_parameter_count   sqlite3_bind_parameter_count
#define _sqlite3_bind_parameter_index   sqlite3_bind_parameter_index
#define _sqlite3_bind_parameter_name    sqlite3_bind_parameter_name
#define _sqlite3_bind_text              sqlite3_bind_text
#define _sqlite3_bind_text16            sqlite3_bind_text16
#define _sqlite3_busy_handler           sqlite3_busy_handler
#define _sqlite3_busy_timeout           sqlite3_busy_timeout
#define _sqlite3_changes                sqlite3_changes
#define _sqlite3_close                  sqlite3_close
#define _sqlite3_collation_needed       sqlite3_collation_needed
#define _sqlite3_collation_needed16     sqlite3_collation_needed16
#define _sqlite3_column_blob            sqlite3_column_blob
#define _sqlite3_column_bytes           sqlite3_column_bytes
#define _sqlite3_column_bytes16         sqlite3_column_bytes16
#define _sqlite3_column_count           sqlite3_column_count
#define _sqlite3_column_decltype        sqlite3_column_decltype
#define _sqlite3_column_decltype16      sqlite3_column_decltype16
#define _sqlite3_column_double          sqlite3_column_double
#define _sqlite3_column_int             sqlite3_column_int
#define _sqlite3_column_int64           sqlite3_column_int64
#define _sqlite3_column_name            sqlite3_column_name
#define _sqlite3_column_name16          sqlite3_column_name16
#define _sqlite3_column_text            sqlite3_column_text
#define _sqlite3_column_text16          sqlite3_column_text16
#define _sqlite3_column_type            sqlite3_column_type
#define _sqlite3_commit_hook            sqlite3_commit_hook
#define _sqlite3_complete               sqlite3_complete
#define _sqlite3_complete16             sqlite3_complete16
#define _sqlite3_create_collation       sqlite3_create_collation
#define _sqlite3_create_collation16     sqlite3_create_collation16
#define _sqlite3_create_function        sqlite3_create_function
#define _sqlite3_create_function16      sqlite3_create_function16
#define _sqlite3_data_count             sqlite3_data_count
#define _sqlite3_errcode                sqlite3_errcode
#define _sqlite3_errmsg                 sqlite3_errmsg
#define _sqlite3_errmsg16               sqlite3_errmsg16
#define _sqlite3_exec                   sqlite3_exec
#define _sqlite3_finalize               sqlite3_finalize
#define _sqlite3_free                   sqlite3_free
#define _sqlite3_free_table             sqlite3_free_table
#define _sqlite3_get_table              sqlite3_get_table
#define _sqlite3_interrupt              sqlite3_interrupt
#define _sqlite3_last_insert_rowid      sqlite3_last_insert_rowid
#define _sqlite3_libversion             sqlite3_libversion
#define _sqlite3_mprintf                sqlite3_mprintf
#define _sqlite3_open                   sqlite3_open
#define _sqlite3_open16                 sqlite3_open16
#define _sqlite3_prepare                sqlite3_prepare
#define _sqlite3_prepare16              sqlite3_prepare16
#define _sqlite3_progress_handler       sqlite3_progress_handler
#define _sqlite3_reset                  sqlite3_reset
#define _sqlite3_result_blob            sqlite3_result_blob
#define _sqlite3_result_double          sqlite3_result_double
#define _sqlite3_result_error           sqlite3_result_error
#define _sqlite3_result_error16         sqlite3_result_error16
#define _sqlite3_result_int             sqlite3_result_int
#define _sqlite3_result_int64           sqlite3_result_int64
#define _sqlite3_result_null            sqlite3_result_null
#define _sqlite3_result_text            sqlite3_result_text
#define _sqlite3_result_text16          sqlite3_result_text16
#define _sqlite3_result_text16be        sqlite3_result_text16be
#define _sqlite3_result_text16le        sqlite3_result_text16le
#define _sqlite3_result_value           sqlite3_result_value
#define _sqlite3_set_authorizer         sqlite3_set_authorizer
#define _sqlite3_step                   sqlite3_step
#define _sqlite3_total_changes          sqlite3_total_changes
#define _sqlite3_trace                  sqlite3_trace
#define _sqlite3_user_data              sqlite3_user_data
#define _sqlite3_value_blob             sqlite3_value_blob
#define _sqlite3_value_bytes            sqlite3_value_bytes
#define _sqlite3_value_bytes16          sqlite3_value_bytes16
#define _sqlite3_value_double           sqlite3_value_double
#define _sqlite3_value_int              sqlite3_value_int
#define _sqlite3_value_int64            sqlite3_value_int64
#define _sqlite3_value_text             sqlite3_value_text
#define _sqlite3_value_text16           sqlite3_value_text16
#define _sqlite3_value_text16be         sqlite3_value_text16be
#define _sqlite3_value_text16le         sqlite3_value_text16le
#define _sqlite3_value_type             sqlite3_value_type
#define _sqlite3_vmprintf               sqlite3_vmprintf
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPPSQLITE3U_H__1B1BE273_2D1E_439C_946F_3CBD1C0EFD2F__INCLUDED_)
