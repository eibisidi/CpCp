#ifndef HOOKS_H
#define HOOKS_H

#include "stdafx.h"

#define DllExport __declspec(dllexport)

#define WM_TRAY_ICON	(WM_USER + 1)
#define WM_HOOKER_PASTE	(WM_USER + 2)

DllExport void WINAPI InstallHook(HWND hWnd);
DllExport void WINAPI UninstallHook();

#endif