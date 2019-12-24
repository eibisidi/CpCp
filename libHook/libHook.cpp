// libHook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libHook.h"
#include <fstream>

extern HMODULE hookModule;
HHOOK HookPaste;	//Paste
DWORD HookerTid;
std::ofstream outFile;


LRESULT CALLBACK KeyBoard_LL(int nCode,WPARAM wParam,LPARAM lParam)
{
	if (nCode < 0)  // do not process message 
		return CallNextHookEx(HookPaste, nCode, wParam, lParam); 

	BOOL ret;

	switch (wParam)
	{
	case WM_KEYDOWN:
		KBDLLHOOKSTRUCT *  key = (KBDLLHOOKSTRUCT *)lParam;

		if (0x56 == key->vkCode
			&& (GetAsyncKeyState(VK_LWIN) & 0x8000))
		{//V key pressed while left Windows key down
			
			ret = PostThreadMessage(HookerTid, WM_HOOKER_PASTE, 0, 0);
			outFile << "Win + V" << ret << std::endl;
		}
		break;
	}

	return CallNextHookEx(HookPaste, nCode, wParam, lParam); 
}

void WINAPI InstallHook(DWORD dwThreadID)
{
	HookPaste=(HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL,(HOOKPROC)KeyBoard_LL, hookModule,0);
	HookerTid = dwThreadID;
	outFile.open("save.log");
}

void WINAPI UninstallHook()
{
	UnhookWindowsHookEx(HookPaste);
}

