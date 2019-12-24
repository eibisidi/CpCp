// ypyp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ypyp.h"

#include "Hook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		InstallHook(GetCurrentThreadId());

		MSG msg;
		while(TRUE)
		{
			if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			{
				switch(msg.message)
				{
				case WM_HOOKER_CC:
					printf("WM_HOOKER_CC\n");
					break;
				case WM_HOOKER_PASTE:
					printf("WM_HOOKER_PASTE\n");
					break;
				default:
					;
				}
			}
		}

		UninstallHook();
	}

	return nRetCode;
}
