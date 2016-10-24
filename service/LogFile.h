// Released under Public Domain 2008 Joe Oszlanczi
#ifndef _C_LOGFILE_
#define _C_LOGFILE_

#include <Windows.h>
#include <string.h>
#include <io.h>
#include <iostream>

#include <string.h>

using namespace std;

class CLogFile
{

public:
	CLogFile();
	~CLogFile();

	FILE*				m_pLogFile;
	long				m_lTruncate;
	CRITICAL_SECTION	m_cs;
	string              m_filename;
	bool                m_bOpened;

	void OpenFile(const char* strFile, bool bAppend = TRUE, long lTruncate = 4096);
	void ChangeFile(const char* strFile, bool bAppend = TRUE, long lTruncate = 4096);
	void CloseFile();

	void Write(const char* pszFormat, ...);

};

#endif
