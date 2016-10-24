// ServiceFrameWork.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <aclapi.h>
#include <stdio.h>
#include "rserver.h"

#pragma comment(lib, "advapi32.lib")


TCHAR szCommand[10];

VOID __stdcall DisplayUsage(void);


int _tmain(int argc, _TCHAR* argv[])
{
	
if( argc > 1 ) {
    StringCchCopy(szCommand, 10, argv[1]);
   
	if (lstrcmpi( szCommand, TEXT("start")) == 0 ){
	  rserver RakServer;
	  if (RakServer.Start())
	  {
		  printf("Service started \n\n","");
	  }
	  else
	  {
	      printf("Service start failed \n\n","");
      }
	}
	else if (lstrcmpi( szCommand, TEXT("stop")) == 0 ){
	
	 rserver RakServer;
	 if (RakServer.Stop())
	  {
		  printf("Server stoped \n\n","");
	  }
	  else
	  {
	      printf("Service stop failed \n\n","");
      }
	
	}
	else if (lstrcmpi( szCommand, TEXT("remove")) == 0 ){
	  rserver RakServer;
	  if (RakServer.Remove())
	  {
		  printf("Service removed \n\n","");
	  }
	  else
	  {
	      printf("Server remove failed \n\n","");
      }
	}
	else if (lstrcmpi( szCommand, TEXT("help")) == 0 )
    {
        _tprintf(TEXT("Unknown command (%s)\n\n"), szCommand);
        DisplayUsage();
    } 
	else if( lstrcmpi( argv[1], TEXT("install")) == 0 )
    {
      rserver RakServer;
	  if (RakServer.Install())
	  {
		  printf("Service installed \n\n","");
	  }
	  else
	  {
	      printf("Service install failed \n\n","");
      }
	}
	else if( lstrcmpi( argv[1], TEXT("query")) == 0 )
    {
      rserver RakServer;
	  RakServer.Query();
	}


}
	else
	{    
		 rserver RakServer;
		 RakServer.Execute();
	}
   
	
}

VOID __stdcall DisplayUsage()
{ 
	printf("Service usage [command]"":\n");
	printf("\t  start\n");
    printf("\t  stop\n");
    printf("\t  remove\n");
    printf("\t  help\n");
    printf("\t  install\n");
	printf("\t  query\n");

}

