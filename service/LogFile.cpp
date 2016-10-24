// Released under Public Domain 2008 Joe Oszlanczi
#include "LogFile.h"
#include <direct.h>
#include <Windows.h>
#include <string>

using namespace std;

//------------------------------------------------------------------------
void CLogFile::OpenFile(const char* strFile, bool bAppend, long lTruncate)
{
	m_bOpened   = false;
	m_lTruncate = lTruncate;
	m_filename  = strFile;

	TCHAR	szFile[MAX_PATH+1];
	if ( strlen(strFile)>3 && strFile[1]!=':' )
	{
		::GetModuleFileName(NULL, szFile, MAX_PATH);
		long llength = strlen(szFile);
		TCHAR*	pcat = szFile+(llength - 1);
		while (llength--)
		{
			pcat--;
			if (*pcat == '\\')
				break;
		}
		
		if (*pcat == '\\')
		{
			pcat++;
			strcpy(pcat, strFile);
		}
		else
			strcpy(szFile, strFile);
	}
	else
		strcpy(szFile, strFile);

	if (!(m_pLogFile = fopen(szFile, bAppend ? "a" : "w")))
	{
	}

	InitializeCriticalSection(&m_cs);
	if ( m_pLogFile )
		fputs("\n=============== Start Loging================\n", m_pLogFile);
}

//------------------------------------------------------------------------

CLogFile::CLogFile()
{
	
}

//------------------------------------------------------------------------
CLogFile::~CLogFile()
{
	CloseFile();
}

//------------------------------------------------------------------------
void CLogFile::CloseFile()
{
	if (m_pLogFile)
	{
		fclose(m_pLogFile);
	}

	DeleteCriticalSection(&m_cs);
}

//------------------------------------------------------------------------
void CLogFile::ChangeFile(const char* strFile, bool bAppend, long lTruncate)
{
	if (strFile != m_filename)
	{
		CloseFile();
		OpenFile(strFile, bAppend, lTruncate);
	}
}

//------------------------------------------------------------------------
void CLogFile::Write(const char* pszFormat, ...)
{
	if (!m_pLogFile)
		return;

	EnterCriticalSection(&m_cs);
	TCHAR	szLog[256];
	va_list argList;
	va_start( argList, pszFormat );
	vsprintf( szLog, pszFormat, argList );
	va_end( argList );
	_SYSTEMTIME	time;
	::GetLocalTime(&time);
	TCHAR	szLine[256];

	sprintf(szLine, "%04d/%02d/%02d %02d:%02d:%02d:%03d \t%s\n", 
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
		szLog);

	fputs(szLine, m_pLogFile);
	fflush(m_pLogFile);
	LeaveCriticalSection(&m_cs);
}
