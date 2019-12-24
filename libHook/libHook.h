#ifndef HOOKS_H
#define HOOKS_H

#include "stdafx.h"

#define DllExport __declspec(dllexport)

#define WM_HOOKER_CC	(WM_USER + 1)
#define WM_HOOKER_PASTE	(WM_USER + 2)

DllExport void WINAPI InstallHook(DWORD dwThreadID);
DllExport void WINAPI UninstallHook();

#endif