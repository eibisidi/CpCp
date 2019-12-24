// Hook.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Hook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CHookApp

BEGIN_MESSAGE_MAP(CHookApp, CWinApp)
END_MESSAGE_MAP()


// CHookApp construction

CHookApp::CHookApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CHookApp object

CHookApp theApp;


// CHookApp initialization

BOOL CHookApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

HHOOK HookPaste;	//Paste
DWORD HookerTid;

LRESULT CALLBACK KeyBoard_LL(int nCode,WPARAM wParam,LPARAM lParam)
{
	if (nCode < 0)  // do not process message 
		return CallNextHookEx(HookPaste, nCode, wParam, lParam); 

	switch (wParam)
	{
	case WM_KEYDOWN:
		KBDLLHOOKSTRUCT *  key = (KBDLLHOOKSTRUCT *)lParam;

		if (0x56 == key->vkCode
			&& (GetAsyncKeyState(VK_LWIN) & 0x8000))
		{//V key pressed while left Windows key down
			PostThreadMessage(HookerTid, WM_HOOKER_PASTE, 0, 0);
		}
		break;
	}

	return CallNextHookEx(HookPaste, nCode, wParam, lParam); 
}

DllExport void WINAPI InstallHook(DWORD dwThreadID)
{
	HookPaste=(HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL,(HOOKPROC)KeyBoard_LL,theApp.m_hInstance,0);
	HookerTid = dwThreadID;
}

DllExport void WINAPI UninstallHook()
{
	UnhookWindowsHookEx(HookPaste);
}

