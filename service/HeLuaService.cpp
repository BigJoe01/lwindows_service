// Released under Public Domain 2008 Joe Oszlanczi
#include "HeLuaService.h"
#include <Windows.h>
#include <iostream>
#include <exception>
#include <assert.h>

//--------------------------------------------------
static int heSleep(lua_State *L)
{
	int t;
	t = luaL_checkint(L,1);
	if (t < 0) t = 0;
	Sleep((DWORD)t);
	return 0;
}

//--------------------------------------------------
static int heLogEvent(lua_State *L)
{
  assert( lua_gettop(L) > 0);
  Logger.Write(lua_tostring(L,1));
  return 0;
}

static int heLogWinEvent(lua_State *L)
{
	assert( lua_gettop(L) > 1);
	BaseService->LogEvent(lua_tostring(L,1), (TService::evLogType)lua_tointeger(L,2) );
	return 0;
}


//--------------------------------------------------
static int heGetTerminated(lua_State *L)
{   
	lua_pushboolean(L,BaseService->Terminated());
	return 1;
}

//--------------------------------------------------
static int heTerminate(lua_State *L)
{
	BaseService->Terminate();
	return 0;
}

//--------------------------------------------------
static int heDirectory(lua_State *L)
{
	lua_pushstring(L,BaseService->m_sServicePath.c_str());
	return 1;
}

//--------------------------------------------------
static int heProcessId(lua_State *L)
{
	lua_pushinteger(L,BaseService->ProcessId() );
	return 1;
}

//--------------------------------------------------
static int heParameters(lua_State *L)
{
	lua_pushstring( L,BaseService->Parameters.c_str() );
	return 1;
}

//--------------------------------------------------
static int heServiceName(lua_State *L)
{
	lua_pushstring(L,BaseService->ServiceName.c_str());
	return 1;
}

//--------------------------------------------------
static int heDisplayName(lua_State *L)
{
	lua_pushstring(L,BaseService->DisplayName.c_str());
	return 1;
}

//--------------------------------------------------
static int heOS(lua_State *L)
{
	lua_pushstring(L,"Windows");
	return 1;
}

//--------------------------------------------------
static int hePath(lua_State *L)
{
	lua_pushstring(L,BaseService->m_sFilePath.c_str());
	return 1;
}

//--------------------------------------------------
static int heWorkingDir(lua_State *L)
{
	lua_pushstring(L,BaseService->m_sWorkingDir.c_str());
	return 1;
}


//--------------------------------------------------
static const struct luaL_Reg service_functions[] = {
	{"sleep", heSleep },
	{"isterminated", heGetTerminated },
	{"terminate", heTerminate },
	{"currentdir", hePath},
	{"service_name", heServiceName},
	{"service_displayname", heDisplayName},
	{"log_event", heLogEvent},
	{"log_winevent", heLogWinEvent },
	{"filename", heDirectory},
	{"processid", heProcessId },
	{"parameters", heParameters },
	{"working_dir", heWorkingDir },
	{"os", heOS},
	{NULL, NULL},
};

//--------------------------------------------------
bool    HeLuaService::LuaInit()
{
     bool ares = true;
	 Logger.Write("Init lua state");

	 m_pLuaState = luaL_newstate();
     if (m_pLuaState==NULL) 
	 {
		 Logger.Write("Invalid lua state");
		 return false;
	 }
	 
	 luaL_openlibs(m_pLuaState);
	 LuaInitGlobals(m_pLuaState);

	 return ares;
}

//--------------------------------------------------
void     HeLuaService::LuaInitGlobals(lua_State *L)
{
	Logger.Write("Initializate Service Global functions");
	luaL_register(L, "service", service_functions);	
}

//--------------------------------------------------
bool HeLuaService::LuaLoad()
{
	bool ares = true;
	Logger.Write("Load lua file %s",m_sLuaFile.c_str());

	int ls = luaL_loadfile(m_pLuaState, std::string( m_sWorkingDir + m_sLuaFile ).c_str() );
	if ( ls == 0 ) 
	{
		Logger.Write("Lua file successfully loaded");
	}
	else
	{
		std::string err =  lua_tostring(m_pLuaState, -1);
		lua_pop(m_pLuaState,1);
		
		if ( ls==LUA_ERRSYNTAX )
		{
			Logger.Write("Lua Syntax error check file"); 
		}
		else if (ls==LUA_ERRMEM) {
			Logger.Write("Lua memory allocation error"); 
		}
		else
		{
			Logger.Write("Lua file load error %s",err.c_str()); 
		}
		ares = false;
	}
		
	return ares;
}

//--------------------------------------------------
bool HeLuaService::LuaRun()
{
   Logger.Write("Running lua application");
   bool ares = true;
   int status = lua_pcall(m_pLuaState,0,LUA_MULTRET,0);
   if ( status != 0 )
   { 
	   std::string error = lua_tostring(m_pLuaState,-1);
	   Logger.Write("Lua run error : %s",error.c_str());
	   luaL_error(m_pLuaState,"%s\n",error.c_str());
	   ares = false;
   }
   else
   {
	 Logger.Write("Lua run no error ");
   }
  return ares;
}

//--------------------------------------------------
bool HeLuaService::LuaClose()
{
	Logger.Write("Close state");
	bool bRes = false;
	if (m_bLuaInited)
	{
		lua_close(m_pLuaState);
		m_pLuaState = NULL;
		m_bLuaInited = false;
		bRes = true;
	}
  return bRes;
}

//--------------------------------------------------
HeLuaService::HeLuaService(void) 
{
	m_bLuaInited = false;	  
    BaseService  = this;
	char        imagePath[MAX_PATH+1];	 
	GetModuleFileNameA(NULL,imagePath,255);
	m_sServicePath	= imagePath;


	std::string fnn   = imagePath;
	std::string fnlog = fnn + ".log";
	std::string fnini = fnn + ".ini";
	Logger.OpenFile(fnlog.c_str());

	CIniReader IniReader2((char*)fnini.c_str());

	m_sWorkingDir   = IniReader2.ReadString("lua","path","");
	m_sLuaFile		= IniReader2.ReadString("lua","application","");
	ServiceName		= IniReader2.ReadString("service","name","");
	DisplayName     = IniReader2.ReadString("service","displayname","");
	Parameters      = IniReader2.ReadString("service","parameters","");


	if (m_sLuaFile.empty() || ServiceName.empty() ||  DisplayName.empty() )
	{
			Logger.Write("Display name invalid in ini file","");
	}
	else
	{
			Logger.Write("Ini file readed","");
			Logger.Write("Service Name : %s",ServiceName.c_str());
			Logger.Write("Display Name : %s",DisplayName.c_str());
			Logger.Write("Lua application : %s",m_sLuaFile.c_str());
			Logger.Write("Lua parameters  : %s",Parameters.c_str());
	}

	char *cp;
	cp = strrchr(imagePath, '\\');
	if (cp) {
		cp[1] = '\0';
		m_sFilePath = imagePath;
	}

	if ( Terminated() )
	{
	  Logger.Write("Terminated true");
	} 
	else
	{
		Logger.Write("Terminated false");
	}
	 
}

//--------------------------------------------------
int HeLuaService::LuaPanic (lua_State *L)
{
	Logger.Write("Lua panic - exception,  unprotected error in call to Lua API");
	return 0;
}

//--------------------------------------------------
HeLuaService::~HeLuaService(void) 
{  
	Logger.CloseFile();
	m_bLuaInited = false;
}

//--------------------------------------------------
bool HeLuaService::Init(void)
 {
	if ( SetCurrentDirectory( m_sWorkingDir.c_str() ) )
	{
		bool asucc = true;
		asucc = LuaInit() && LuaLoad(); 
		m_bLuaInited = asucc;
		return asucc;
	}
	return false;
 }

//--------------------------------------------------
 void HeLuaService::Cleanup(void)
 {

try 
 {
 	 Logger.Write("Stop Lua Service");
	 LuaClose();
	 Logger.Write("Destroy Lua Service");
  } 
  catch (exception e ) {
	  Logger.Write("Cleanup exception %s",e.what());
  }
	Logger.Write("Cleanup End");
	Logger.CloseFile();
 }

 //--------------------------------------------------
 void HeLuaService::ServiceProc(void)
 { 
	 try
	 {
		if (m_bLuaInited)
			LuaRun();
	 }
	 catch (std::bad_alloc*  e)
	 {
	 	Logger.Write("Service Memory exception : %s",e->what());
	 }
	 catch (std::exception* e)
	 {
		 Logger.Write("Service exception : %s",e->what());

	 }
 }