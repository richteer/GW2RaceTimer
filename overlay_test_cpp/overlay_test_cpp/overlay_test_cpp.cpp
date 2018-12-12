// overlay_test_cpp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "overlay_test_cpp.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hCurrent;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI WndPositioner()
{
	HWND hGW2;
	WINDOWINFO GWWndInfo;
	WINDOWINFO CurrentWndInfo;
	WINDOWPLACEMENT CurrentWndPlacement;
	WINDOWPLACEMENT GWWndPlacement;
	RECT GWRect;
	BOOL bMin = FALSE;
	BOOL bTop = FALSE;

	hGW2 = FindWindow(NULL, _T("Guild Wars 2"));
	if (hGW2 == NULL)
	{
		PostMessage(hCurrent, WM_DESTROY, NULL, NULL);
	}

	// Start with GW2 on top
	SetForegroundWindow(hGW2);

	while(hGW2 != NULL)
	{
		hGW2 = FindWindow(NULL, _T("Guild Wars 2"));
		if (hGW2 == NULL)
		{
			PostMessage(hCurrent, WM_DESTROY, NULL, NULL);
			break;
		}

		// Access to window informations
		GetClientRect(hGW2, &GWRect);
		GetWindowInfo(hGW2, &GWWndInfo);
		GetWindowInfo(hCurrent, &CurrentWndInfo);
		GetWindowPlacement(hCurrent, &CurrentWndPlacement);
		GetWindowPlacement(hGW2, &GWWndPlacement);

		// If the overlay is active, make GW active instead
		if (CurrentWndInfo.dwWindowStatus == WS_ACTIVECAPTION)
		{
			SetForegroundWindow(hGW2);
			continue;
		}
		// If GW2 is active, put overlay on top
		// TODO: readjust to be exactly on top of GW2's window rect
		if (GWWndInfo.dwWindowStatus == WS_ACTIVECAPTION)
		{
			ShowWindow(hCurrent, GWWndPlacement.showCmd);
		
			if (GWWndPlacement.showCmd == SW_NORMAL || GWWndPlacement.showCmd == SW_MAXIMIZE)
			{
				// Get screen coordinates
				ClientToScreen(hGW2, (LPPOINT)&GWRect.left);
				ClientToScreen(hGW2, (LPPOINT)&GWRect.right);

				SetWindowPos(
					hCurrent,
					HWND_TOPMOST,
					GWRect.left,
					GWRect.top,
					GWRect.right - GWRect.left,
					GWRect.bottom - GWRect.top,
					SWP_NOACTIVATE);
			}

			bTop = TRUE;
		}
		else if(bTop != FALSE)
		{
			// Remove TOPMOST attribute but keep overlay on top of GW2
			SetWindowPos(hCurrent, hGW2, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			SetWindowPos(hGW2, hCurrent, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			bTop = FALSE;
		}
		// Minimize overlay if GW2 is minimized
		if (GWWndPlacement.showCmd == SW_MINIMIZE && bMin != FALSE)
		{
			ShowWindow(hCurrent, SW_MINIMIZE);
			bMin = FALSE;
		}
	}

	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

	// Verify if GW2 is open
	// Get GW2 window handle
	// Get GW2 window rect
	// Draw over GW2 window

	// This to learn:
	// Draw a transparend window
	// Forward commands (if needed)

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OVERLAY_TEST_CPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	// Get window handles
	hCurrent = FindWindow(szWindowClass, szTitle);

	// Set window style (TODO: Make this a function)
	LONG lStyle = GetWindowLong(hCurrent, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
	SetWindowLong(hCurrent, GWL_STYLE, lStyle);

	LONG cur_style = GetWindowLong(hCurrent, GWL_EXSTYLE);
	cur_style &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	cur_style |= WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE;
	SetWindowLong(hCurrent, GWL_EXSTYLE, cur_style);

	if (hCurrent == NULL)
	{
		return FALSE;
	}

	// Start positioning thread
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WndPositioner, NULL, 0, NULL);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OVERLAY_TEST_CPP));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OVERLAY_TEST_CPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    //wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OVERLAY_TEST_CPP);
	wcex.lpszMenuName = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
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
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
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
