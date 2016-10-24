// Released under Public Domain 2008 Joe Oszlanczi
#ifndef _HELUASERVICE_H__
#define _HELUASERVICE_H__


#include "cService.h"
#include "LogFile.h"
#include <string>
#include "IniReader.h"

extern "C"{

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luaconf.h"
}

#pragma warning(disable: 4995)

class  HeLuaService;
static CLogFile       Logger;
static HeLuaService*  BaseService;

class HeLuaService : public TService
{
public:

	HeLuaService(void) ;
	~HeLuaService(void);

	bool           m_bLuaInited;
	lua_State *    m_pLuaState;
	std::string    m_sServicePath;
	std::string    m_sLuaFile;
	std::string    m_sFilePath;
	std::string    m_sWorkingDir;

	bool    LuaInit();
	bool    LuaClose();
	bool    LuaLoad();
	bool    LuaRun();
	void    LuaInitGlobals(lua_State *L);

static	int LuaPanic (lua_State *L);
protected:
	
	void				   ServiceProc    (void) ;
	virtual bool           Init           (void)         ;
	virtual void           Cleanup        (void)         ;
	virtual void           LogoffEvent    (void)         {Terminate();}
	virtual void           ShutdownEvent  (void)         {Terminate();}
		

  };

#endif