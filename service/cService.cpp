// Released under Public Domain 2008 Joe Oszlanczi
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include "cService.h"
#include <string>

namespace ModuleTService 
{
   TService* Service;
};

//-----------------------------------------------------------------------------
#pragma warning (disable : 100)

void WINAPI TService::ServiceMain (DWORD argc, char* argv[])
{
using namespace ModuleTService;

Service->m_StatusHandle = RegisterServiceCtrlHandlerA(Service->ServiceName.c_str(),ServiceHandler);

if (Service->succeeded (Service->m_StatusHandle != NULL)) {
   Service->SetStatus (SERVICE_START_PENDING);

   if (Service->Init()) {
      Service->SetStatus (SERVICE_RUNNING);
      Service->ServiceProc();
      Service->SetStatus (SERVICE_STOP_PENDING);
      Service->Cleanup();
      Service->SetStatus (SERVICE_STOPPED);
      }
   else Service->SetStatus (SERVICE_STOPPED);
   }
else Service->SetStatus (SERVICE_STOPPED);
}
#pragma warning (default : 100)

//-----------------------------------------------------------------------------
void WINAPI TService::ServiceHandler (DWORD control)
{
using namespace ModuleTService;

switch (control) {
   case SERVICE_CONTROL_INTERROGATE:
      Service->SetStatus (Service->m_StatusCode);
      break;

   case SERVICE_CONTROL_STOP:
   case SERVICE_CONTROL_SHUTDOWN:
      Service->SetStatus (SERVICE_STOP_PENDING);
      Service->m_Terminated = true;
      break;
   }
}

//-----------------------------------------------------------------------------
bool TService::Query(void)
{
	std::string temps;
	SC_HANDLE schSCManager;
    SC_HANDLE schService;
    LPQUERY_SERVICE_CONFIG lpsc; 
    LPSERVICE_DESCRIPTION lpsd;
	DWORD dwProcessid = 0;
    DWORD dwBytesNeeded, cbBufSize, dwError; 

    // Get a handle to the SCM database. 
 
    schSCManager = OpenSCManagerA( 
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 
 
    if (NULL == schSCManager) 
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return false;
    }

    schService = OpenService( 
        schSCManager,          // SCM database 
		ServiceName.c_str(),             // name of service 
        SERVICE_QUERY_CONFIG); // need query config access 
 
    if (schService == NULL)
    { 
        printf("OpenService failed (%d)\n", GetLastError()); 
        CloseServiceHandle(schSCManager);
        return false;
    }
	
	SERVICE_STATUS_PROCESS ServiceStatus;
	DWORD BytesNeeded = 0;
	BOOL  bResult     = false;
	if ( !QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ServiceStatus, sizeof(SERVICE_STATUS_PROCESS), (LPDWORD)&BytesNeeded ) )
	{
		dwError = GetLastError();
		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

		std::string message(messageBuffer, size);

		//Free the buffer.
		LocalFree(messageBuffer);
		_tprintf(TEXT("QueryServiceStatusEx failed (%d) %s \n"), dwError, message.c_str() );

	}
	else
	{
		dwProcessid = ServiceStatus.dwProcessId;
	}
 
    if( !QueryServiceConfig( schService, NULL, 0, &dwBytesNeeded ))
    {
        dwError = GetLastError();
        if( ERROR_INSUFFICIENT_BUFFER == dwError )
        {
            cbBufSize = dwBytesNeeded;
            lpsc = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LMEM_FIXED, cbBufSize);
        }
        else
        {
            printf("QueryServiceConfig failed (%d)", dwError);
            goto cleanup; 
        }
    }
  
    if( !QueryServiceConfig( schService, lpsc, cbBufSize, &dwBytesNeeded) ) 
    {
        printf("QueryServiceConfig failed (%d)", GetLastError());
        goto cleanup;
    }

    if( !QueryServiceConfig2( schService, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &dwBytesNeeded))
    {
        dwError = GetLastError();
        if( ERROR_INSUFFICIENT_BUFFER == dwError )
        {
            cbBufSize = dwBytesNeeded;
            lpsd = (LPSERVICE_DESCRIPTION) LocalAlloc(LMEM_FIXED, cbBufSize);
        }
        else
        {
            printf("QueryServiceConfig2 failed (%d)", dwError);
            goto cleanup; 
        }
    }
	
    if (! QueryServiceConfig2( schService, SERVICE_CONFIG_DESCRIPTION, (LPBYTE) lpsd, cbBufSize,&dwBytesNeeded) ) 
    {
        printf("QueryServiceConfig2 failed (%d)", GetLastError());
        goto cleanup;
    }
 
    // Print the configuration information.
	_tprintf(TEXT("%s configuration: \n"), DisplayName.c_str());
	switch(lpsc->dwServiceType) {
	case SERVICE_ADAPTER:
		 temps = "SERVICE_ADAPTER";
		break;
	case SERVICE_FILE_SYSTEM_DRIVER:
		temps = "SERVICE_FILE_SYSTEM_DRIVER";
		break;
	case SERVICE_KERNEL_DRIVER:
		temps = "SERVICE_KERNEL_DRIVER";
		break;
	case SERVICE_WIN32_OWN_PROCESS:
		temps = "SERVICE_WIN32_OWN_PROCESS";
		break;
	case SERVICE_WIN32_SHARE_PROCESS:
		temps = "SERVICE_WIN32_SHARE_PROCESS";
		break;
	case SERVICE_RECOGNIZER_DRIVER:
		temps = "SERVICE_RECOGNIZER_DRIVER";
		break;
	case SERVICE_INTERACTIVE_PROCESS:
		temps = "SERVICE_INTERACTIVE_PROCESS";
		break;
	case (0x110) :
		temps = "SERVICE_INTERACTIVE_PROCESS AND SERVICE_WIN32_OWN_PROCESS";
		break;

	}
	
	_tprintf(TEXT("  Type: 0x%x %s\n"), lpsc->dwServiceType,  temps.c_str());
    
	
	switch(lpsc->dwStartType) {
	case SERVICE_AUTO_START:
		 temps = "SERVICE_AUTO_START";
		break;
	case SERVICE_BOOT_START:
		temps = "SERVICE_BOOT_START";
		break;
	case SERVICE_DEMAND_START:
		temps = "SERVICE_DEMAND_START";
		break;
	case SERVICE_DISABLED:
		temps = "SERVICE_DISABLED";
		break;
	case SERVICE_SYSTEM_START:
		temps = "SERVICE_SYSTEM_START";
		break;
	}
	
	
	_tprintf(TEXT("  Start Type:%s\n"),temps.c_str() );
    
	
	switch(lpsc->dwErrorControl) {
	case SERVICE_ERROR_CRITICAL:
		 temps = "SERVICE_ERROR_CRITICAL";
		break;
	case SERVICE_ERROR_IGNORE:
		temps = "SERVICE_ERROR_IGNORE";
		break;
	case SERVICE_ERROR_NORMAL:
		temps = "SERVICE_ERROR_NORMAL";
		break;
	case SERVICE_ERROR_SEVERE:
		temps = "SERVICE_ERROR_SEVERE";
		break;
	}
	
	_tprintf(TEXT("  Error Control: 0x%x  %s \n"), lpsc->dwErrorControl,temps.c_str() );
    _tprintf(TEXT("  Binary path: %s\n"), lpsc->lpBinaryPathName);
    _tprintf(TEXT("  Account: %s\n"), lpsc->lpServiceStartName);
	_tprintf(TEXT("  ProcessId: %d\n"), dwProcessid );

    if (lpsd->lpDescription != NULL && lstrcmp(lpsd->lpDescription, TEXT("")) != 0)
        _tprintf(TEXT("  Description: %s\n"), lpsd->lpDescription);
    if (lpsc->lpLoadOrderGroup != NULL && lstrcmp(lpsc->lpLoadOrderGroup, TEXT("")) != 0)
        _tprintf(TEXT("  Load order group: %s\n"), lpsc->lpLoadOrderGroup);
    if (lpsc->dwTagId != 0)
        _tprintf(TEXT("  Tag ID: %d\n"), lpsc->dwTagId);
    if (lpsc->lpDependencies != NULL && lstrcmp(lpsc->lpDependencies, TEXT("")) != 0)
        _tprintf(TEXT("  Dependencies: %s\n"), lpsc->lpDependencies);
 
    LocalFree(lpsc); 
    LocalFree(lpsd);

cleanup:
    CloseServiceHandle(schService); 
    CloseServiceHandle(schSCManager);
	return true;
}


//-----------------------------------------------------------------------------
BOOL WINAPI TService::ConsoleHandler (DWORD dwCtrlType) {
using namespace ModuleTService;

if (dwCtrlType == CTRL_LOGOFF_EVENT)
   Service->LogoffEvent ();
else Service->ShutdownEvent ();
return TRUE;
}

//-----------------------------------------------------------------------------

TService::TService() : m_StatusHandle(NULL),m_StatusCode(SERVICE_STOPPED),m_Terminated(false)
{
	m_ProcessId = 0;
	ModuleTService::Service = this;
	ZeroMemory (m_ErrorString,sizeof(m_ErrorString));
	succeeded(SetConsoleCtrlHandler (ConsoleHandler,TRUE));
}

//-----------------------------------------------------------------------------

TService::~TService()
{
if (m_EventLog)
   DeregisterEventSource (m_EventLog);
}

//-----------------------------------------------------------------------------
bool TService::shopen (tSvcHandle &hService)
{
	bool 		bSuccess = false;
	tSvcHandle	hSCM;
	            m_ProcessId = 0;
	if ((hSCM = OpenSCManagerA(NULL,NULL,SC_MANAGER_ALL_ACCESS)) != NULL)
	{
		hService = OpenServiceA(hSCM,ServiceName.c_str(),SERVICE_ALL_ACCESS);
		bSuccess = succeeded (hService != NULL);

		SERVICE_STATUS_PROCESS ServiceStatus;
		DWORD BytesNeeded = 0;
		if ( bSuccess && QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (BYTE*)&ServiceStatus, sizeof ServiceStatus, &BytesNeeded ) != NULL )
		{
			m_ProcessId = ServiceStatus.dwProcessId;
		}

		CloseServiceHandle (hSCM);
   }
	else 
		succeeded (false);
	
	return bSuccess;
}

//-----------------------------------------------------------------------------
void TService::shclose (tSvcHandle hService)
{
	CloseServiceHandle (hService);
}

//-----------------------------------------------------------------------------
void TService::SetStatus (DWORD status)
{
	m_StatusCode  = status;

	SERVICE_STATUS ss;
	ss.dwServiceType              = SERVICE_WIN32_OWN_PROCESS;
	ss.dwCurrentState             = m_StatusCode;
	ss.dwControlsAccepted	      = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN;
	ss.dwWin32ExitCode            = NOERROR;
	ss.dwServiceSpecificExitCode  = NOERROR;
	ss.dwCheckPoint               = 0;
	ss.dwWaitHint                 = 3000;

	if (! succeeded (SetServiceStatus (m_StatusHandle,&ss)))
	   LogEvent (m_ErrorString,evWarning);
}

//-----------------------------------------------------------------------------
bool TService::succeeded (BOOL bSuccess )
{
	DWORD SysError = bSuccess ? ERROR_SUCCESS : GetLastError();
	FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM,NULL,SysError, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), m_ErrorString,sizeof(m_ErrorString),NULL);
	return SysError == ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
bool TService::Execute ()
{
	m_EventLog = RegisterEventSourceA (NULL,ServiceName.c_str());

	SERVICE_TABLE_ENTRYA entries[2];
	entries[0].lpServiceName = (char*) ServiceName.c_str();
	entries[0].lpServiceProc = ServiceMain;
	entries[1].lpServiceName = NULL;
	entries[1].lpServiceProc = NULL;

	bool  bSuccess = succeeded (StartServiceCtrlDispatcherA(entries));
	if (! bSuccess )
		LogEvent (m_ErrorString,evError);

	return bSuccess;
}

//-----------------------------------------------------------------------------
bool TService::ConsoleMode (void)
{
	m_EventLog = RegisterEventSourceA(NULL,ServiceName.c_str());

	bool bSuccess = false;
	if (Init())	{
		ServiceProc();
		Cleanup ();
		bSuccess = true;
	}
	return bSuccess;
}

//-----------------------------------------------------------------------------
bool TService::Start()
{
	bool         bSuccess = false;
	tSvcHandle   hService;

	if (shopen (hService))
	{
		bSuccess = succeeded (StartService (hService,0,NULL));
		shclose (hService);
	}
	return bSuccess;
}

//-----------------------------------------------------------------------------
bool TService::Stop()
{
	bool         bSuccess = false;
	tSvcHandle   hService;

	if (shopen (hService)) {
	   SERVICE_STATUS   state;
	   bSuccess = succeeded (ControlService  (hService,SERVICE_CONTROL_STOP,&state));
	   shclose (hService);
	   }
	return bSuccess;
}

//-----------------------------------------------------------------------------
bool TService::Remove()
{
	Stop();

	bool         bSuccess = false;
	tSvcHandle   hService;

	if (shopen (hService)) {
		bSuccess = succeeded (DeleteService (hService));
		shclose (hService);

		HKEY hKey;
		if (RegOpenKeyExA (HKEY_LOCAL_MACHINE,REG_EVENTLOG,0,KEY_ALL_ACCESS,&hKey)
			== ERROR_SUCCESS) {
				RegDeleteKeyA (hKey,ServiceName.c_str());
				RegCloseKey (hKey);
		}
	}
	return bSuccess;
}

//-----------------------------------------------------------------------------
bool TService::Install()
{
	bool        bSuccess = false;
	SC_HANDLE   hService,hSCManager;
	char        imagePath[MAX_PATH];

	GetModuleFileNameA(NULL,imagePath,MAX_PATH);
	if (succeeded ((hSCManager = OpenSCManagerA(NULL,NULL,SC_MANAGER_ALL_ACCESS)) != NULL))
	{
		if (succeeded ((hService = ::CreateServiceA (hSCManager, ServiceName.c_str(),DisplayName.c_str(), SERVICE_ALL_ACCESS,
							  SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS, SERVICE_DEMAND_START,SERVICE_ERROR_NORMAL, imagePath,NULL,NULL,NULL,NULL,NULL)) != NULL))
		{
		  CloseServiceHandle (hService);
		  bSuccess = true;
		}
	   CloseServiceHandle (hSCManager);
	   }

	if (bSuccess)
	{
	   UINT f = EVENTLOG_ERROR_TYPE|EVENTLOG_WARNING_TYPE|EVENTLOG_INFORMATION_TYPE;

	   char   szKey [MAX_PATH];
	   wsprintfA (szKey,"%s\\%s",REG_EVENTLOG,ServiceName.c_str());

	   HKEY hKey;
	   if (RegCreateKeyA (HKEY_LOCAL_MACHINE,szKey,&hKey) == ERROR_SUCCESS)
	   {
		  char   mod[MAX_PATH];
		  DWORD  len = GetModuleFileNameA (NULL,mod,MAX_PATH);

		  RegSetValueExA(hKey,"TypesSupported",0,REG_DWORD,(BYTE*) &f,sizeof(DWORD));
		  RegSetValueExA(hKey,"EventMessageFile",0,REG_SZ,(BYTE*) mod,len+1);
		  RegCloseKey (hKey);
		}
	}
	else
	{
		printf("Install failed (%d)\n", GetLastError());
	}
	return bSuccess;
}

//-----------------------------------------------------------------------------
bool TService::Help (DWORD context)
{
	char file [MAX_PATH +6],*p;

	GetModuleFileNameA (NULL,file,MAX_PATH);
	if ((p = strrchr (file,'.')) != NULL)
	strcpy (p,".hlp");
	return WinHelpA (NULL,file,context ? HELP_CONTEXT : HELP_CONTENTS,context) != 0;
}

// -----------------------------------------------------------------------------

void TService::PrintLastError (const char *cap)
{
	DWORD n;

	if (cap != NULL && *cap) {
	   WriteConsole (GetStdHandle(STD_ERROR_HANDLE),"\r\n",2,&n,NULL);
	   WriteConsole (GetStdHandle(STD_ERROR_HANDLE),cap,(DWORD)strlen(cap),&n,NULL);
	   }

	char oem[sizeof(m_ErrorString)];
	ZeroMemory (oem,sizeof(oem));

	CharToOemBuffA (m_ErrorString,oem,(DWORD) strlen(m_ErrorString));
	WriteConsole (GetStdHandle(STD_ERROR_HANDLE),"\r\n",2,&n,NULL);
	WriteConsole (GetStdHandle(STD_ERROR_HANDLE),oem,(DWORD) strlen(oem),&n,NULL);
}

//-----------------------------------------------------------------------------
void TService::LogEvent (const char* str,evLogType type,WORD Category)
{
	if (m_EventLog != NULL)
	{
		const char* msgs[1];
		msgs[0] = str;
		ReportEventA (m_EventLog,(WORD) type,Category,0,NULL,1,0,msgs,NULL);
	}
}

//-----------------------------------------------------------------------------
bool TService::SetConfigValue (char* key,BYTE *b,DWORD n,cfValType t)
{
	char	RegPath[255];
	HKEY 	hk;
	DWORD disp;
	bool 	bSuccess = false;

	wsprintfA (RegPath,REG_CONFIG,ServiceName.c_str());
	if (succeeded (RegCreateKeyExA (HKEY_LOCAL_MACHINE,RegPath,0,NULL,
						 REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hk,&disp) == ERROR_SUCCESS))
	{
	   ZeroMemory (b,n);
	   bSuccess = succeeded (RegSetValueExA (hk,key,0,(DWORD)t,b,n) == ERROR_SUCCESS);
	   RegCloseKey (hk);
	}
	return bSuccess;
}

//-----------------------------------------------------------------------------
bool TService::GetConfigValue (char* key,BYTE *b,DWORD *n,cfValType *t)
{
	char	RegPath[255];
	HKEY 	hk;
	DWORD disp;
	bool 	bSuccess = false;

	wsprintfA (RegPath,REG_CONFIG,ServiceName.c_str());
	if (succeeded (RegCreateKeyExA (HKEY_LOCAL_MACHINE,RegPath,0,NULL, REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hk,&disp) == ERROR_SUCCESS))
	{
	   ZeroMemory (b,*n);
	   bSuccess = succeeded (RegQueryValueExA (hk,key,0,(DWORD*)t,b,n) == ERROR_SUCCESS );
	   RegCloseKey (hk);
	}
	return bSuccess;
}