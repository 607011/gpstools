// $Id$
// Copyright (c) 2009 Oliver Lau <oliver@ersatzworld.net>

#include "stdafx.h"
#include "wpl1000.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

GPS::WPL1000File wpl1000File;
GPS::GPXFile gpxFile;
std::string wpl1000Filename;
std::string gpxFilename;
BOOL multi = FALSE;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

BOOL                OpenNVPIPE(HWND);
BOOL                SaveGPX(HWND);

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
    LoadString(hInstance, IDC_WPL1000, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WPL1000));

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
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
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WPL1000));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WPL1000);
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
    HWND hWnd;

    hInst = hInstance; // Store instance handle in our global variable

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 300, 200, NULL, NULL, hInstance, NULL);

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
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;
    HWND hStatus = NULL;
    HWND hEdit = NULL;

    switch (message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_FILEOPEN:
            OpenNVPIPE(hWnd);
            break;
        case IDM_FILESAVE:
            SaveGPX(hWnd);
            break;
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
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        {
            hStatus = GetDlgItem(hWnd, IDC_MAIN_STATUS);
            int nWidth = LOWORD(lParam); 
            int nHeight = HIWORD(lParam);
            int PartSize[2] = { nWidth/2, -1 };
            SendMessage(hStatus, SB_SETPARTS, sizeof(PartSize)/sizeof(int), (LPARAM)PartSize);
            SetWindowPos(hStatus, 0, 0, 0, nWidth, 20, SWP_NOMOVE); 
            hEdit = GetDlgItem(hWnd, IDC_EDIT);
            MoveWindow(hEdit, 0, 0, nWidth, nHeight-20, TRUE);
        }
        break;
    case WM_CREATE:
        {
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            int PartSize[2] = { clientRect.right/2, -1 };
            hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
                hWnd, (HMENU)IDC_MAIN_STATUS, hInst, NULL);
            SendMessage(hStatus, SB_SETPARTS, sizeof(PartSize)/sizeof(int), (LPARAM)PartSize);
            RECT statusBarRect;
            GetClientRect(hStatus, &statusBarRect);
            hEdit = CreateWindowEx(NULL, "Edit", NULL,
                ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL |WS_CHILD | WS_VISIBLE,
                0, 0,
                statusBarRect.right, clientRect.bottom - (statusBarRect.bottom - statusBarRect.top),
                hWnd, (HMENU)IDC_EDIT, hInst, NULL);
        }
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


BOOL OpenNVPIPE(HWND hWnd)
{
    BOOL bSuccess = FALSE;
    OPENFILENAME ofn;
    TCHAR szFileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = "DAT files (*.dat)\0*.dat\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "dat";
    if(GetOpenFileName(&ofn))
    {
        wpl1000Filename = ofn.lpstrFile;
        errno_t rc = wpl1000File.load(wpl1000Filename);
        if (wpl1000File.tracks().size() == 0)
        {
            HWND hStatus = GetDlgItem(hWnd, IDC_MAIN_STATUS);
            SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Die Datei enthält keine Tracks!");
        }
        bSuccess = (rc != 0);
    }
    return bSuccess;
}


BOOL SaveGPX(HWND hWnd)
{
    BOOL bSuccess = FALSE;
    OPENFILENAME ofn;
    TCHAR szFileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = "GPX files (*.gpx)\0*.gpx\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "gpx";
    if(GetSaveFileName(&ofn))
    {
        gpxFilename = ofn.lpstrFile;
        HWND hStatus = GetDlgItem(hWnd, IDC_MAIN_STATUS);
        if (multi)
        {
            for (GPS::TrackList::const_iterator i = wpl1000File.tracks().begin(); i != wpl1000File.tracks().end(); ++i)
            {
                GPS::GPXFile trkFile;
                trkFile.addTrack(*i);
                std::basic_string<char>::size_type spos = gpxFilename.find_last_of(PathDelimiter);
                std::string trkFilename = gpxFilename;
                trkFilename.insert(spos+1, (*i)->startTimestamp().toString("%Y%m%d-%H%M") + "-");
                SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Gespeichert");
                errno_t rc = trkFile.write(trkFilename);
                if (rc != 0)
                    SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Speichern fehlgeschlagen!");
                bSuccess = (rc == 0);
            }
            GPS::GPXFile wptFile;
            wptFile.setWaypoints(wpl1000File.waypoints());
            std::string wptFilename = gpxFilename;
            std::basic_string<char>::size_type ppos = gpxFilename.find_last_of('.');
            if (ppos == std::basic_string<char>::npos)
                ppos = gpxFilename.size();
            wptFilename.insert(ppos, "-waypoints");
            wptFile.write(wptFilename);
        }
        else // ! multi
        {   
            GPS::GPXFile gpxFile;
            gpxFile.setTracks(wpl1000File.tracks());
            gpxFile.setWaypoints(wpl1000File.waypoints());
            HWND hEdit = GetDlgItem(hWnd, IDC_EDIT);
            for (GPS::TrackList::const_iterator i = gpxFile.tracks().begin(); i != gpxFile.tracks().end(); ++i)
            {
                SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM) (*i)->name().c_str()); 
            }
            errno_t rc = gpxFile.write(gpxFilename);
            if (rc != 0) {
                SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"FEHLER: Speichern fehlgeschlagen.");
            }
            bSuccess = (rc == 0);
        }
    }
    return bSuccess;
}
