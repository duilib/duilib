#pragma once

class CGlobalVariable
{
public:
	CGlobalVariable(void);
	~CGlobalVariable(void);

public:
	static CString& GetCurPath();
	static CString& GetTemplatesDir();
	static CString& GetStylesDir();

public:
	static bool m_bIsProjectExist;
	static CString m_strProjectName;
	static CString m_strProjectPath;

private:
	static CString m_strCurPath;
	static CString m_strTemplatesDir;
	static CString m_strStylesDir;
};
