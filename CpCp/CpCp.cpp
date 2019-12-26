// CpCp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CpCp.h"
#include "libHook.h"
#include <atlstr.h>		//for Cstring
#include <deque>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

#define MAX_CB_SIZE (10)
#define MAX_MENU_LEN (40)
HWND	appWnd;

typedef std::deque<CString> ClipBoardBuffer;
ClipBoardBuffer cbBuffer;							//CPCP ClipBoard Buffer
int				idxActive;							//Current Active entry

CString menuText[MAX_CB_SIZE];
void formatMenuText(size_t idx)
{
#define MAX_DISP_LEN (30)
	const CString & content = cbBuffer[idx];
	bool toolong = (content.GetLength() > MAX_DISP_LEN);
	menuText[idx].Format(_T("%-40s"), content.Left(MAX_DISP_LEN));
	if(toolong)
		menuText[idx].Append(_T("..."));
	else
		menuText[idx].Append(_T("   "));
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CPCP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPCP));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


	UninstallHook();

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPCP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CPCP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   appWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!appWnd)
   {
      return FALSE;
   }

   ShowWindow(appWnd, nCmdShow);
   UpdateWindow(appWnd);

   return TRUE;
}

//Get data from system clipboard
//Save it to buffer
void WINAPI UpdateCpCp(HWND hwnd)
{
	if (CountClipboardFormats() == 0) 
		return; 

	// Open the clipboard. 
	if (!OpenClipboard(hwnd)) 
		return; 

	if (IsClipboardFormatAvailable(CF_TEXT))
	{
		HGLOBAL		hglb;
		LPTSTR		lptstr;
		
		hglb = GetClipboardData(CF_UNICODETEXT);
		if(hglb != NULL)
		{
			lptstr = (LPTSTR)GlobalLock(hglb);
			if (lptstr != NULL)
			{
				if (cbBuffer.empty()
					|| 0 != cbBuffer[idxActive].Compare(lptstr))	//different from our explicitly SetClipboardData call 
				{
					cbBuffer.push_front(lptstr);
					if(cbBuffer.size() > MAX_CB_SIZE)		//if entries count exceeds 10, erase one
						cbBuffer.pop_back();
				}
			}
			GlobalUnlock(hglb);
		}
	}

	// Close the clipboard. 
	CloseClipboard(); 
}

void WINAPI HandlePaste(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	HWND prevForeWnd = (HWND)wParam;

	Sleep(100);

	//Create the ClipBoard Menu
	HMENU cpMenu = CreatePopupMenu();
	if (!cpMenu)
		return;

	//Populate menu
	for(size_t i = 0; i < cbBuffer.size(); ++i)
	{
		formatMenuText(i);
		AppendMenu(cpMenu, MF_STRING, (i+1)/*menuitem id*/, menuText[i]); 
	}

	//Change Focus(to enable arrow keys & Esc)
	HWND hCurWnd = ::GetForegroundWindow();   
	DWORD dwCurID = ::GetWindowThreadProcessId(hCurWnd, NULL);   
	DWORD dwThisID = ::GetCurrentThreadId();   
	::AttachThreadInput(dwCurID, dwThisID, TRUE);   
	::SetForegroundWindow(appWnd);   

	//Show menu
	int select = 
		TrackPopupMenu(cpMenu,
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD,
		100,
		100,
		0,
		appWnd,
		NULL);

	DestroyMenu(cpMenu);

	//Restore Focus
	::AttachThreadInput(dwCurID, dwThisID, FALSE);
	::SetForegroundWindow(prevForeWnd);

	//No selection
	if (select <= 0)
		return;

	if (!OpenClipboard(appWnd)) 
		return;
	
	EmptyClipboard(); //is it needed?

	idxActive = select - 1;

	// Allocate a global memory object for the text. 
	CString & selectedContent = cbBuffer[idxActive];
	int   cch = selectedContent.GetLength();
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (cch + 1) * sizeof(TCHAR)); 
	if (hglbCopy == NULL) 
	{ 
		CloseClipboard(); 
		return; 
	} 

	// Lock the handle and copy the text to the buffer. 
	LPTSTR  lptstrCopy = (LPTSTR)GlobalLock(hglbCopy); 
	memcpy(lptstrCopy, selectedContent.GetBuffer(), cch * sizeof(TCHAR));
	lptstrCopy[cch] = (TCHAR) 0;    // null character 
	
	GlobalUnlock(hglbCopy); 

	//Replace system Clip Board
	SetClipboardData(CF_UNICODETEXT, hglbCopy);
	CloseClipboard();

	//mimic ctrl+V
	keybd_event(VK_CONTROL, 0, 0, 0);				// press ctrl
	keybd_event(0x56, 0, 0, 0);						// press v
	keybd_event(0x56, 0, KEYEVENTF_KEYUP, 0);		// release v
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0); // release ctrl
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndNextViewer;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		ChangeClipboardChain(hWnd, hwndNextViewer); 
		PostQuitMessage(0);
		break;
	case WM_CREATE: 
		// Add the window to the clipboard viewer chain. 
		hwndNextViewer = SetClipboardViewer(hWnd); 
		InstallHook(hWnd);
		break; 
	case WM_CHANGECBCHAIN: 
		// If the next window is closing, repair the chain. 
		if ((HWND) wParam == hwndNextViewer) 
			hwndNextViewer = (HWND) lParam; 
		// Otherwise, pass the message to the next link. 
		else if (hwndNextViewer != NULL) 
			SendMessage(hwndNextViewer, message, wParam, lParam); 
		break; 
	case WM_DRAWCLIPBOARD:  // clipboard contents changed. 
		// Update our cbBuffer
		UpdateCpCp(hWnd); 
		// Pass the message to the next window in clipboard 
		// viewer chain. 
		SendMessage(hwndNextViewer, message, wParam, lParam); 
		break;
	case WM_HOOKER_PASTE:
		HandlePaste(wParam, lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
