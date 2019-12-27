// libHook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libHook.h"

extern HMODULE hookModule;
HHOOK	HookPaste;		//Paste
HWND	cpcpAppWnd;		//cpcpAppWnd Handle

LRESULT CALLBACK KeyBoard_LL(int nCode,WPARAM wParam,LPARAM lParam)
{
	if (nCode < 0)  // do not process message 
		return CallNextHookEx(HookPaste, nCode, wParam, lParam); 

	KBDLLHOOKSTRUCT *  key = (KBDLLHOOKSTRUCT *)lParam;
	switch (wParam)
	{
	case WM_KEYDOWN:
		if (0x56 == key->vkCode
			&& (GetAsyncKeyState(VK_LWIN) & 0x8000))
#if 0
			//Shift + Ctrl + V Alternative
			&& (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			&& (GetAsyncKeyState(VK_CONTROL) & 0x8000))
#endif
		{//V key pressed while Shift & Ctrl key down
			HWND hForeWnd = GetForegroundWindow();
			PostMessage(cpcpAppWnd, WM_HOOKER_PASTE, (WPARAM)hForeWnd, 0);
			return 1;
		}
		break;
	case WM_KEYUP:
		break;
	}

	return CallNextHookEx(HookPaste, nCode, wParam, lParam); 
}

void WINAPI InstallHook(HWND hWnd)
{
	HookPaste=(HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL,(HOOKPROC)KeyBoard_LL, hookModule,0);
	cpcpAppWnd = hWnd;
}

void WINAPI UninstallHook()
{
	UnhookWindowsHookEx(HookPaste);
}

