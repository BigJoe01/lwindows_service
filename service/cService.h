// Released under Public Domain 2008 Joe Oszlanczi
#ifndef TSERVICE_BASE
#define TSERVICE_BASE

#define REG_CONFIG   "SYSTEM\\CurrentControlSet\\Services\\%s\\ServiceConfig"
#define REG_EVENTLOG "SYSTEM\\CurrentControlSet\\Services\\EventLog\\System"

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <string>

class TService
{

public:
	TService();
	~TService();

	std::string ServiceName;
	std::string DisplayName;
	std::string Parameters;
	enum evLogType
	{
		evError   = EVENTLOG_ERROR_TYPE,
		evWarning = EVENTLOG_WARNING_TYPE,
		evInfo    = EVENTLOG_INFORMATION_TYPE
	};

   bool          Execute                 (void);
   bool          ConsoleMode             (void);

   bool          Start                   (void);
   bool          Stop                    (void);
   bool          Install                 (void);
   bool          Remove                  (void);
   bool          Query					 (void);
   virtual bool  Help                    (DWORD context = 0);

   bool          Terminated              (void) {return m_Terminated;}
   void          Terminate               (void) {m_Terminated = true;}

   DWORD         ProcessId               (void) { return m_ProcessId; }
   const char   *LastError               (void) const  {return m_ErrorString;}
   void          PrintLastError          (const char *Caption = NULL);
   void          LogEvent                (const char* e,evLogType t = evInfo,WORD cat = 0);
 protected:

   enum cfValType
   {
      cfBinary  = REG_BINARY,
      cfDword   = REG_DWORD,
      cfString  = REG_SZ
   };

   bool SetConfigValue (char* key,BYTE *val,DWORD nval,cfValType t = cfString);
   bool GetConfigValue (char* key,BYTE *buf,DWORD *nbuff,cfValType *t);

   virtual void           ServiceProc    (void) = 0;

   virtual bool           Init           (void)         {return true;}
   virtual void           Cleanup        (void)         {return;}
   virtual void           LogoffEvent    (void)         {return;}
   virtual void           ShutdownEvent  (void)         {Terminate();}

  private:

   typedef SC_HANDLE tSvcHandle;

   SERVICE_STATUS_HANDLE  m_StatusHandle;
   DWORD                  m_StatusCode;
   DWORD                  m_ProcessId;
   HANDLE                 m_EventLog;
   bool                   m_Terminated;
   char                   m_ErrorString[512];

   bool                   shopen         (tSvcHandle &hService);
   void                   shclose        (tSvcHandle hService);
   bool                   succeeded      (BOOL	ReturnValue);

   void                   SetStatus      (DWORD status);
   static void  WINAPI    ServiceMain    (DWORD argc,char* argv[]);
   static void  WINAPI    ServiceHandler (DWORD ServiceControl);
   static BOOL  WINAPI    ConsoleHandler (DWORD dwCtrlType);
};

#endif
