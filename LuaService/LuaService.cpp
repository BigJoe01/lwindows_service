#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <aclapi.h>
#include <stdio.h>
#include "HeLuaService.h"
#pragma comment(lib, "advapi32.lib")
TCHAR szCommand[10];


VOID __stdcall DisplayUsage(void)
{
	printf("Lua Service usage [command]"":\n");
	printf("\t  /start\n");
	printf("\t  /stop\n");
	printf("\t  /remove\n");
	printf("\t  /help or /?\n");
	printf("\t  /install\n");
	printf("\t  /query\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
	HeLuaService   luaservice;
	BaseService = &luaservice;

	if( argc > 1 )
	{
		StringCchCopy(szCommand, 10, argv[1]);

		if ( lstrcmpi( szCommand, TEXT("/start")) == 0 )
		{
			if ( luaservice.Start() )
				printf("Service started \n\n","");
			else
				printf("Service start failed \n\n","");
		}
		else if ( lstrcmpi( szCommand, TEXT("/stop")) == 0 )
		{
			if ( luaservice.Stop() )
				printf("Service stoped \n\n","");
			else
				printf("Service stop failed \n\n","");
		}
		else if ( lstrcmpi( szCommand, TEXT("/remove")) == 0 ){
			if (luaservice.Remove())
				printf("Service removed \n\n","");
			else
				printf("Service remove failed \n\n","");
		}
		else if ( lstrcmpi( szCommand, TEXT("/help") ) == 0 || lstrcmpi( szCommand, TEXT("/?") ) == 0 )
		{
			DisplayUsage();
		} 
		else if( lstrcmpi( argv[1], TEXT("/install")) == 0 )
		{
			if ( luaservice.Install() )
				printf("Service installed \n\n","");
			else
				printf("Service install failed \n\n","");
		}
		else if( lstrcmpi( argv[1], TEXT("/query")) == 0 )
			luaservice.Query();

	}
	else
		luaservice.Execute();

}