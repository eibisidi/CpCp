// libHook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libHook.h"
#include <fstream>

extern HMODULE hookModule;
HHOOK HookPaste;	//Paste
HWND HookerTid;
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
#if 0
			&& (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			&& (GetAsyncKeyState(VK_CONTROL) & 0x8000))
#endif
		{//V key pressed while Shift & Ctrl key down
			HWND hForeWnd = GetForegroundWindow();
			ret = PostMessage(HookerTid, WM_HOOKER_PASTE, (WPARAM)hForeWnd, 0);
			outFile << "Win + V" << ret << std::endl;
			return 1;
		}
		break;
	case WM_KEYUP:
		KBDLLHOOKSTRUCT *  key = (KBDLLHOOKSTRUCT *)lParam;

		if (0x56 == key->vkCode
			&& (GetAsyncKeyState(VK_LWIN) & 0x8000))
		{
			//todo
		}
		break;
	}

	return CallNextHookEx(HookPaste, nCode, wParam, lParam); 
}

void WINAPI InstallHook(HWND hWnd)
{
	HookPaste=(HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL,(HOOKPROC)KeyBoard_LL, hookModule,0);
	HookerTid = hWnd;
	outFile.open("save.log");
}

void WINAPI UninstallHook()
{
	UnhookWindowsHookEx(HookPaste);
}

