// $Id$
// Copyright (c) 2009 Oliver Lau <oliver@ersatzworld.net>

#include "stdafx.h"
#include "wpl1000.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
HWND ghWnd;
HWND ghMainForm;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

GPS::WPL1000File wpl1000File;
GPS::GPXFile gpxFile;
std::string wpl1000Filename;
std::string gpxFilename;
BOOL multi = FALSE;


ATOM				MyRegisterClass(HINSTANCE);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	MainFormProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

VOID                SetStatusBar(LPSTR, UINT Milli = 3000);
VOID                AppendToLog(LPSTR);

BOOL                OpenNVPIPE(VOID);
BOOL                OpenGPX(VOID);
BOOL                LoadNVPIPE(VOID);
BOOL                SaveGPX(VOID);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MSG msg;
    HACCEL hAccelTable;
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_WPL1000, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;
    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WPL1000));
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


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;
    wcex.cbSize         = sizeof(WNDCLASSEX);
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


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance;
    hWnd = CreateWindow(szWindowClass, szTitle, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, 0, 423, 468, HWND_DESKTOP, NULL, hInstance, NULL);
    ghWnd = hWnd;
    SetStatusBar(_T("Bereit."));
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}


VOID AppendToLog(LPSTR Msg)
{
    SendDlgItemMessage(ghMainForm, IDC_EDIT_LOG, EM_REPLACESEL, 0, (LPARAM)Msg);
    SendDlgItemMessage(ghMainForm, IDC_EDIT_LOG, EM_REPLACESEL, 0, (LPARAM)"\r\n");
}


VOID SetStatusBar(LPSTR Msg, UINT Milli)
{
    HWND hStatus = GetDlgItem(ghWnd, IDC_MAIN_STATUS);
    SetTimer(ghWnd, IDT_STATUSBAR, Milli, (TIMERPROC)NULL);
    SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)Msg);
}


VOID ClearStatus(VOID)
{
    HWND hStatus = GetDlgItem(ghWnd, IDC_MAIN_STATUS);
    SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"");
}


LRESULT CALLBACK MainFormProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    int nWidth, nHeight;
    HWND hComboOpen, hComboSave, hEdit;

    switch(Msg)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        switch (wmId)
        {
        case IDC_COMBO_OPEN:
            switch (wmEvent)
            {
            case CBN_SETFOCUS:
                hEdit = GetDlgItem(hWndDlg, IDC_EDIT_LOG);
                SendMessage(hEdit, WM_SETTEXT, NULL, (LPARAM)"");
                if (OpenNVPIPE())
                {
                    hComboOpen = GetDlgItem(hWndDlg, IDC_COMBO_OPEN);
                    SetWindowText(hComboOpen, (LPCSTR)wpl1000Filename.c_str());
                    LoadNVPIPE();
                }
                break;
            case CBN_KILLFOCUS:
                {
                    TCHAR buf[MAX_PATH];
                    hComboOpen = GetDlgItem(hWndDlg, IDC_COMBO_OPEN);
                    GetWindowText(hComboOpen, buf, MAX_PATH);
                }
                break;
            case CBN_DROPDOWN: 
                break;
            default:
                break;
            }
            break;
        case IDC_COMBO_SAVE:
            switch (wmEvent)
            {
            case CBN_SETFOCUS:
                if (OpenGPX()) 
                {
                    hComboSave = GetDlgItem(hWndDlg, IDC_COMBO_SAVE);
                    SetWindowText(hComboSave, (LPCSTR)gpxFilename.c_str());
                    SaveGPX();
                }
                break;
            case CBN_KILLFOCUS:
                break;
            }
            break;
        case IDC_BUTTON_OPEN:
            if (OpenNVPIPE())
            {
                hComboOpen = GetDlgItem(hWndDlg, IDC_COMBO_OPEN);
                SetWindowText(hComboOpen, (LPCSTR)wpl1000Filename.c_str());
                LoadNVPIPE();
            }
            break;
        case IDC_BUTTON_SAVE:
            SaveGPX();
            break;
        case IDC_CHECK_MULTI:
            if (wmEvent == BN_CLICKED) 
                multi ^= TRUE;
            break;
        }
        break;
    case WM_SIZE:
        nWidth = LOWORD(lParam); 
        nHeight = HIWORD(lParam);
        break;
    case WM_INITDIALOG:
        return TRUE;
    default:
        break;
    }
    return FALSE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int nWidth, nHeight;
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;
    HWND hStatus, hMainForm;

    switch (message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
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
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case IDT_STATUSBAR:
            ClearStatus();
            break;
        default:
            break;
        }
        break;
    case WM_SIZE:
        {
            hStatus = GetDlgItem(hWnd, IDC_MAIN_STATUS);
            nWidth = LOWORD(lParam); 
            nHeight = HIWORD(lParam);
            int PartSize[2] = { nWidth, -1 };
            SendMessage(hStatus, SB_SETPARTS, sizeof(PartSize)/sizeof(int), (LPARAM)PartSize);
            SetWindowPos(hStatus, 0, 0, 0, nWidth, 20, SWP_NOMOVE); 
        }
        break;
    case WM_CREATE:
        {
            InitCommonControls();
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            int PartSize[2] = { clientRect.right, -1 };
            hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
                WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
                hWnd, (HMENU)IDC_MAIN_STATUS, hInst, NULL);
            SendMessage(hStatus, SB_SETPARTS, sizeof(PartSize)/sizeof(int), (LPARAM)PartSize);
            RECT statusBarRect;
            GetClientRect(hStatus, &statusBarRect);
            hMainForm = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MAINFORM), hWnd, (DLGPROC)MainFormProc);
            ghMainForm = hMainForm;
            ShowWindow(hMainForm, SW_SHOW);
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


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


BOOL LoadNVPIPE(VOID)
{
    BOOL bSuccess = FALSE;
    errno_t rc = wpl1000File.load(wpl1000Filename);
    SetStatusBar((rc != 0)? _T("Laden fehlgeschlagen.") : _T("Laden OK."));
    if (wpl1000File.tracks().size() == 0)
        SetStatusBar(_T("Die Datei enthält keine Tracks!"));
    char* buf = (char*)VirtualAlloc(NULL, MAX_PATH+2, MEM_COMMIT, PAGE_READWRITE);
    if (buf == NULL)
        return FALSE;
    AppendToLog("Die Datei enthält folgende Tracks:");
    for (GPS::TrackList::const_iterator i = wpl1000File.tracks().begin(); i != wpl1000File.tracks().end(); ++i)
    {
        sprintf_s(buf, MAX_PATH+2, "- %s",  (*i)->name().c_str());
        AppendToLog(buf);
    }
    bSuccess = (rc != 0);
    VirtualFree(buf, 0, MEM_RELEASE);
    return bSuccess;
}


BOOL OpenNVPIPE(VOID)
{
    BOOL bSuccess = FALSE;
    OPENFILENAME ofn;
    TCHAR szFileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = ghWnd;
    ofn.lpstrFilter = _T("DAT files (*.dat)\0*.dat\0All Files (*.*)\0*.*\0");
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = _T("dat");
    if (GetOpenFileName(&ofn))
    {
        wpl1000Filename = ofn.lpstrFile;
        bSuccess = TRUE;
    }
    return bSuccess;
}


BOOL SaveGPX(VOID)
{
    BOOL bSuccess = FALSE;
    if (multi)
    {
        for (GPS::TrackList::const_iterator i = wpl1000File.tracks().begin(); i != wpl1000File.tracks().end(); ++i)
        {
            GPS::GPXFile trkFile;
            trkFile.addTrack(*i);
            std::basic_string<char>::size_type spos = gpxFilename.find_last_of(PathDelimiter);
            std::string trkFilename = gpxFilename;
            trkFilename.insert(spos+1, (*i)->startTimestamp().toString("%Y%m%d-%H%M") + "-");
            errno_t rc = trkFile.write(trkFilename);
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
        for (GPS::TrackList::const_iterator i = gpxFile.tracks().begin(); i != gpxFile.tracks().end(); ++i)
            AppendToLog((LPSTR)(*i)->name().c_str()); 
        errno_t rc = gpxFile.write(gpxFilename);
        bSuccess = (rc == 0);
    }
    SetStatusBar((bSuccess)? _T("Speichern OK.") : _T("FEHLER: Speichern fehlgeschlagen."));
    return bSuccess;
}


BOOL OpenGPX(VOID)
{
    BOOL bSuccess = FALSE;
    OPENFILENAME ofn;
    TCHAR szFileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = ghWnd;
    ofn.lpstrFilter = _T("GPX files (*.gpx)\0*.gpx\0All Files (*.*)\0*.*\0");
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = _T("gpx");
    if(GetSaveFileName(&ofn))
    {
        gpxFilename = ofn.lpstrFile;
        bSuccess = TRUE;
    }
    return bSuccess;
}
