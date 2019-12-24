// Hook.h : main header file for the Hook DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#define DllExport __declspec(dllexport)

#define WM_HOOKER_CC	(WM_USER + 1)
#define WM_HOOKER_PASTE	(WM_USER + 2)

DllExport void WINAPI InstallHook(DWORD dwThreadID);
DllExport void WINAPI UninstallHook();

// CHookApp
// See Hook.cpp for the implementation of this class
//

class CHookApp : public CWinApp
{
public:
	CHookApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
