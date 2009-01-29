// $Id$
// Copyright (c) 2009 Oliver Lau <oliver@ersatzworld.net>

#include "stdafx.h"
#include "wpl1000.h"


// TODO: nicht ungefragt Dateien überschreiben

// TODO: nur ausgewählte Tracks sichern


const int MAX_LOADSTRING = 100;
const int DefaultWindowHeight = 460;
const int DefaultWindowWidth = 460;
const int DefaultWindowTop = CW_USEDEFAULT;
const int DefaultWindowLeft = CW_USEDEFAULT;

HINSTANCE hInst;
HWND ghWnd;
HWND ghMainForm;
HMENU ghRecentFiles = NULL;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
const TCHAR* szKeyRecentFiles = TEXT("SOFTWARE\\Lau\\WPL1000\\Recent File List");
const TCHAR* szKeySettings    = TEXT("SOFTWARE\\Lau\\WPL1000\\Settings");
const TCHAR* szKeyWorkspace   = TEXT("SOFTWARE\\Lau\\WPL1000\\Workspace");
int nWindowWidth;
int nWindowHeight;
int nWindowTop;
int nWindowLeft;

GPS::WPL1000File wpl1000File;
GPS::GPXFile gpxFile;
std::string wpl1000Filename;
std::string gpxFilename;
BOOL multi = FALSE;
TRACKLISTINFOTYPE* trk = NULL;

RecentFileList RecentFiles;

VOID                Warn(LPTSTR);
VOID                Error(LPTSTR, LONG lErrCode = 0);
VOID                ErrorExit(LPTSTR, LONG lErrCode = 0);

ATOM				MyRegisterClass(HINSTANCE);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	MainFormProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

VOID                SetStatusBar(LPSTR, UINT Milli = 3000);

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
    return (int)msg.wParam;
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


BOOL SaveRecentFilesToReg(VOID)
{
    BOOL bSuccess = FALSE;
    const DWORD MAX_KEY_LENGTH = 255;
    const DWORD MAX_VALUE_NAME = 16383;
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeyRecentFiles, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        if (RegCreateKeyEx(HKEY_CURRENT_USER, szKeyRecentFiles, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL) != ERROR_SUCCESS)
            ErrorExit("RegCreateKeyEx()");
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeyRecentFiles, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        int iIndex = 0;
        TCHAR szValue[MAX_PATH];
        for (RecentFileList::const_iterator i = RecentFiles.begin(); i != RecentFiles.end(); ++i)
        {
            LPCSTR d = (*i).second.c_str();
            sprintf_s(szValue, MAX_PATH, "%d", iIndex);
            LONG retCode = RegSetValueEx(hKey, szValue, 0, REG_SZ, (LPBYTE)d, (*i).second.size());
            if (retCode != ERROR_SUCCESS)
                ErrorExit(TEXT("RegSetValueEx()"), retCode);
            ++iIndex;
        }
        RegCloseKey(hKey);
    }
    return bSuccess;
}


VOID LoadState(VOID)
{
    const int MAX_VALUE_NAME = 16383;
    DWORD dwType;
    DWORD dwSize;

    HKEY hKeyWorkspace;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeyWorkspace, 0, KEY_READ, &hKeyWorkspace) != ERROR_SUCCESS)
        Error(TEXT("RegOpenKeyEx()"));

    TCHAR* achValue = (TCHAR*)LocalAlloc(LMEM_FIXED, MAX_VALUE_NAME);
    if (achValue == NULL)
        ErrorExit("LocalAlloc()");

    nWindowTop = DefaultWindowTop;
    dwSize = MAX_VALUE_NAME;
    if (RegQueryValueEx(hKeyWorkspace, "top", 0, &dwType, (LPBYTE)achValue, &dwSize) == ERROR_SUCCESS)
        if (dwType == REG_DWORD)
            nWindowTop = *((int*)achValue);
    nWindowLeft = DefaultWindowLeft;
    dwSize = MAX_VALUE_NAME;
    if (RegQueryValueEx(hKeyWorkspace, "left", 0, &dwType, (LPBYTE)achValue, &dwSize) == ERROR_SUCCESS)
        if (dwType == REG_DWORD)
            nWindowLeft = *((int*)achValue);
    nWindowWidth = DefaultWindowWidth;
    dwSize = MAX_VALUE_NAME;
    if (RegQueryValueEx(hKeyWorkspace, "width", 0, &dwType, (LPBYTE)achValue, &dwSize) == ERROR_SUCCESS)
        if (dwType == REG_DWORD)
            nWindowWidth = *((int*)achValue);
    nWindowHeight = DefaultWindowHeight;
    dwSize = MAX_VALUE_NAME;
    if (RegQueryValueEx(hKeyWorkspace, "height", 0, &dwType, (LPBYTE)achValue, &dwSize) == ERROR_SUCCESS)
        if (dwType == REG_DWORD)
            nWindowHeight = *((int*)achValue);

    LocalFree(achValue);
    RegCloseKey(hKeyWorkspace);
}


BOOL SaveState(VOID)
{
    BOOL bSuccess = FALSE;
    const int MAX_VALUE_NAME = 16383;

    SaveRecentFilesToReg();

    //HKEY hKeySettings;
    //if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeySettings, 0, KEY_READ, &hKeySettings) != ERROR_SUCCESS)
    //    if (RegCreateKeyEx(HKEY_CURRENT_USER, szKeySettings, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeySettings, NULL) != ERROR_SUCCESS)
    //        Error(TEXT("RegCreateKeyEx()"));
    //if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeySettings, 0, KEY_SET_VALUE, &hKeySettings) != ERROR_SUCCESS)
    //    Error(TEXT("RegOpenKeyEx()"));
    //RegCloseKey(hKeySettings);

    HKEY hKeyWorkspace;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeyWorkspace, 0, KEY_READ, &hKeyWorkspace) != ERROR_SUCCESS)
        if (RegCreateKeyEx(HKEY_CURRENT_USER, szKeyWorkspace, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyWorkspace, NULL) != ERROR_SUCCESS)
            Error(TEXT("RegCreateKeyEx()"));
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeyWorkspace, 0, KEY_SET_VALUE, &hKeyWorkspace) != ERROR_SUCCESS)
        Error(TEXT("RegOpenKeyEx()"));

    TCHAR* achValue = (TCHAR*)LocalAlloc(LMEM_FIXED, MAX_VALUE_NAME);
    if (achValue == NULL)
        ErrorExit(TEXT("LocalAlloc()"));

    WINDOWINFO pwi;
    pwi.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(ghWnd, &pwi);
    nWindowTop = pwi.rcWindow.top;
    RegSetValueEx(hKeyWorkspace, TEXT("top"), 0, REG_DWORD, (LPBYTE)&nWindowTop, sizeof(nWindowTop));
    nWindowLeft = pwi.rcWindow.left;
    RegSetValueEx(hKeyWorkspace, TEXT("left"), 0, REG_DWORD, (LPBYTE)&nWindowLeft, sizeof(nWindowLeft));
    nWindowWidth = pwi.rcWindow.right - pwi.rcWindow.left;
    RegSetValueEx(hKeyWorkspace, TEXT("width"), 0, REG_DWORD, (LPBYTE)&nWindowWidth, sizeof(nWindowHeight));
    nWindowHeight = pwi.rcWindow.bottom - pwi.rcWindow.top;
    RegSetValueEx(hKeyWorkspace, TEXT("height"), 0, REG_DWORD, (LPBYTE)&nWindowHeight, sizeof(nWindowHeight));
    RegCloseKey(hKeyWorkspace);

    LocalFree(achValue);
    return bSuccess;
}


BOOL LoadRecentFilesFromReg(VOID)
{
    BOOL bSuccess = FALSE;
    const int MAX_KEY_LENGTH = 255;
    const int MAX_VALUE_NAME = 16383;
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeyRecentFiles, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        TCHAR* achValue = (TCHAR*)LocalAlloc(LMEM_FIXED, MAX_VALUE_NAME);
        if (achValue == NULL)
            ErrorExit(TEXT("LocalAlloc()"));
        DWORD cchValue = MAX_VALUE_NAME; 
        TCHAR achClass[MAX_PATH] = TEXT(""); 
        DWORD cchClassName = MAX_PATH;
        DWORD cSubKeys=0;
        DWORD cbMaxSubKey;
        DWORD cchMaxClass;
        DWORD cValues;
        DWORD cchMaxValue;
        DWORD cbMaxValueData;
        DWORD cbSecurityDescriptor;
        FILETIME ftLastWriteTime;
        LONG retCode = RegQueryInfoKey(hKey, achClass, &cchClassName, NULL,                 
            &cSubKeys, &cbMaxSubKey, &cchMaxClass, &cValues, &cchMaxValue,
            &cbMaxValueData, &cbSecurityDescriptor, &ftLastWriteTime);
        if (cValues > 0) 
        {
            TCHAR* pszData = (TCHAR*)LocalAlloc(LMEM_FIXED, MAX_VALUE_NAME);
            if (pszData == NULL)
                ErrorExit("LocalAlloc()");
            retCode = ERROR_SUCCESS;
            RecentFiles.clear();
            for (DWORD i = 0; i < cValues; ++i) 
            { 
                cchValue = MAX_VALUE_NAME; 
                achValue[0] = '\0'; 
                DWORD dwType;
                DWORD dwSize = MAX_VALUE_NAME;
                retCode = RegEnumValue(hKey, i, achValue, &cchValue, NULL,
                    &dwType, (LPBYTE)pszData, &dwSize);
                if (retCode == ERROR_SUCCESS)
                    RecentFiles.add(pszData);
            }
            LocalFree(pszData);
        }
        LocalFree(achValue);
        bSuccess = (RegCloseKey(hKey) == ERROR_SUCCESS);
    }
    return bSuccess;
}


HMENU GetRecentFilesMenu(VOID)
{
    if (ghRecentFiles == NULL)
    {
        HMENU hMainMenu = GetMenu(ghWnd);
        HMENU hFileMenu = GetSubMenu(hMainMenu, 0);
        ghRecentFiles = GetSubMenu(hFileMenu, 3);
    }
    return ghRecentFiles;
}


VOID ClearRecentFilesMenu(VOID)
{
    HMENU hRecentMenu = GetRecentFilesMenu();
    RecentFileListBaseClass::const_iterator i;
    for (i = RecentFiles.begin(); i != RecentFiles.end(); ++i)
        DeleteMenu(hRecentMenu, (*i).first, MF_BYCOMMAND);
    DeleteMenu(hRecentMenu, 0, MF_BYPOSITION);
}


BOOL PopulateRecentFilesMenu(VOID)
{
    ClearRecentFilesMenu();
    HMENU hRecentMenu = GetRecentFilesMenu();
    RecentFileList::const_iterator i;
    for (i = RecentFiles.begin(); i != RecentFiles.end(); ++i)
        AppendMenu(hRecentMenu, MF_STRING, (*i).first, (*i).second.c_str());
    return TRUE;
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance;

    LoadState();

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        nWindowLeft, nWindowTop, nWindowWidth, nWindowHeight,
        HWND_DESKTOP, NULL, hInstance, NULL);
    ghWnd = hWnd;

    ClearRecentFilesMenu();
    LoadRecentFilesFromReg();
    PopulateRecentFilesMenu();
    SetStatusBar(TEXT("Bereit."));
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}


VOID UpdateTitle(VOID)
{
    TCHAR szTitleCopy[MAX_LOADSTRING];
    LoadString(hInst, IDS_APP_TITLE, szTitleCopy, MAX_LOADSTRING);
    StringCchCopy(szTitle, MAX_LOADSTRING, wpl1000Filename.c_str());
    StringCchCat(szTitle, MAX_LOADSTRING, " - ");
    StringCchCat(szTitle, MAX_LOADSTRING, szTitleCopy);
    SendMessage(ghWnd, WM_SETTEXT, NULL, (LPARAM)szTitle);
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
    HWND hList;

    switch(Msg)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        switch (wmId)
        {
        case IDC_CHECK_MULTI:
            if (wmEvent == BN_CLICKED) 
                multi ^= TRUE;
            break;
        }
        break;
    case WM_SIZE:
        nWidth = LOWORD(lParam); 
        nHeight = HIWORD(lParam);
        hList = GetDlgItem(hWndDlg, IDC_LISTVIEW);
        SetWindowPos(hList, 0, 6, 36, nWidth-18, nHeight-36-20, SWP_NOMOVE); 
        break;
    case WM_INITDIALOG:
        {
            TRACKLISTCOLUMNTYPE cols[] = {
                { TEXT("Start"), 120, LVCFMT_LEFT },
                { TEXT("Ende"), 120, LVCFMT_LEFT },
                { TEXT("Punkte"), 48, LVCFMT_RIGHT },
                { TEXT("km"), 48, LVCFMT_RIGHT },
                { TEXT("Dauer"), 72, LVCFMT_LEFT },
                { NULL, 0, 0 }
            };
            hList = GetDlgItem(hWndDlg, IDC_LISTVIEW);
            LVCOLUMN lvc; 
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            int iCol = 0;
            while (cols[iCol].pszText != NULL)
            {
                lvc.iSubItem = iCol;
                lvc.pszText = cols[iCol].pszText;
                lvc.cx = cols[iCol].cx;
                lvc.fmt = cols[iCol].fmt;
                if (ListView_InsertColumn(hList, iCol, &lvc) == -1)
                    return FALSE;
                ++iCol;
            } 
        }
        return TRUE;
    case WM_NOTIFY:
        switch (((LPNMHDR) lParam)->code)
        {
        case LVN_GETDISPINFO:
            NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
            switch (plvdi->item.iSubItem)
            {
            case 0:
                plvdi->item.pszText = trk[plvdi->item.iItem].pszStart;
                break;
            case 1:
                plvdi->item.pszText = trk[plvdi->item.iItem].pszFinish;
                break;
            case 2:
                plvdi->item.pszText = trk[plvdi->item.iItem].pszPointCount;
                break;
            case 3:
                plvdi->item.pszText = trk[plvdi->item.iItem].pszDistance;
                break;
            case 4:
                plvdi->item.pszText = trk[plvdi->item.iItem].pszDuration;
                break;
            default:
                break;
            }
            return 0;
        }
    default:
        break;
    }
    return FALSE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int nWidth, nHeight;
    int wmId, wmEvent;
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
            SendMessage(hWnd, WM_CLOSE, NULL, NULL);
            break;
        case IDM_FILE_OPEN:
            if (OpenNVPIPE())
            {
                if (LoadNVPIPE())
                {
                    UpdateTitle();
                    if (RecentFiles.add(wpl1000Filename))
                        PopulateRecentFilesMenu();
                }
            }
            break;
        case IDM_FILE_SAVE:
            if (OpenGPX())
                SaveGPX();
            break;
        default:
            if (ID_RECENT_FILE_LIST <= wmId && wmId <= ID_RECENT_FILE_LIST+9)
            {
                wpl1000Filename = RecentFiles.find(wmId);
                if (wpl1000Filename != "")
                {
                    if (LoadNVPIPE())
                        UpdateTitle();
                }
            }
            else
                return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }
        break;
    case WM_CLOSE:
        SaveState();
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
            SetWindowPos(ghMainForm, 0, 0, 0, nWidth, nHeight-20, SWP_NOMOVE); 
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


BOOL OpenNVPIPE(VOID)
{
    BOOL bSuccess = FALSE;
    OPENFILENAME ofn;
    TCHAR szFileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = ghWnd;
    ofn.lpstrFilter = TEXT("DAT files (*.dat)\0*.dat\0All Files (*.*)\0*.*\0");
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = TEXT("dat");
    if (GetOpenFileName(&ofn))
    {
        wpl1000Filename = ofn.lpstrFile;
        bSuccess = (wpl1000Filename != "");
    }
    return bSuccess;
}


BOOL LoadNVPIPE(VOID)
{
    BOOL bSuccess = FALSE;
    wpl1000File.clearAll();
    errno_t rc = wpl1000File.load(wpl1000Filename);
    if (rc != 0) 
    {
        Error(TEXT("wpl1000File.load()"));
        return FALSE;
    }
    if (wpl1000File.tracks().size() == 0) 
    {
        Warn(TEXT("Die Datei enthält keine Tracks!"));
        return FALSE;
    }

    SetStatusBar(TEXT("Laden OK."));

    HWND hList = GetDlgItem(ghMainForm, IDC_LISTVIEW);
    ListView_DeleteAllItems(hList);

    if (trk != NULL)
        delete [] trk;

    LVITEM lvI;
    lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE; 
    lvI.state = 0; 
    lvI.stateMask = 0;
    int index = 0;
    trk = new TRACKLISTINFOTYPE[wpl1000File.tracks().size()];
    const DWORD BUFSIZE = 200;
    for (GPS::TrackList::const_iterator i = wpl1000File.tracks().begin(); i != wpl1000File.tracks().end(); ++i)
    {
        CHAR szBuf[BUFSIZE];

        const std::string& t0 = (*i)->startTimestamp().toString();
        trk[index].pszStart = new CHAR[t0.size()+1];
        StringCchCopy(trk[index].pszStart, t0.size(), t0.c_str());

        const std::string& t1 = (*i)->finishTimestamp().toString();
        trk[index].pszFinish = new CHAR[t1.size()+1];
        StringCchCopy(trk[index].pszFinish, t1.size(), t1.c_str());

        sprintf_s(szBuf, BUFSIZE, "%.1lf", 1e-3 * (*i)->distance());
        trk[index].pszDistance = new CHAR[strlen(szBuf)+1];
        StringCchCopy(trk[index].pszDistance, strlen(szBuf)+1, szBuf);

        sprintf_s(szBuf, BUFSIZE, "%u", (*i)->points().size());
        trk[index].pszPointCount = new CHAR[strlen(szBuf)+1];
        StringCchCopy(trk[index].pszPointCount, strlen(szBuf)+1, szBuf);

        GPS::Duration d((*i)->duration());
        const std::string dstr = d.toString();
        trk[index].pszDuration = new CHAR[dstr.size()+1];
        StringCchCopy(trk[index].pszDuration, dstr.size()+1, dstr.c_str());

        lvI.iItem = index;
        lvI.iImage = index;
        lvI.iSubItem = 0;
        lvI.lParam = (LPARAM)&trk[index];
        lvI.pszText = LPSTR_TEXTCALLBACK;
        ListView_InsertItem(hList, &lvI);
        ++index;
    }
    bSuccess = (rc == 0);
    return bSuccess;
}


BOOL SaveGPX(VOID)
{
    const DWORD BUFSIZE = 2048;
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
            if (rc != 0)
                Error(TEXT("trkFile.write()"));
            bSuccess = (rc == 0);
        }
        GPS::GPXFile wptFile;
        wptFile.setWaypoints(wpl1000File.waypoints());
        std::string wptFilename = gpxFilename;
        std::basic_string<char>::size_type ppos = gpxFilename.find_last_of('.');
        if (ppos == std::basic_string<char>::npos)
            ppos = gpxFilename.size();
        wptFilename.insert(ppos, "-waypoints");
        errno_t rc = wptFile.write(wptFilename);
        if (rc != 0)
            Error(TEXT("wptFile.write()"));
        bSuccess |= (rc == 0);
    }
    else // ! multi
    {   
        GPS::GPXFile gpxFile;
        gpxFile.setTracks(wpl1000File.tracks());
        gpxFile.setWaypoints(wpl1000File.waypoints());
        errno_t rc = gpxFile.write(gpxFilename);
        if (rc != 0)
            Error(TEXT("gpxFile.write()"));
        bSuccess = (rc == 0);
    }
    if (bSuccess)
        SetStatusBar(TEXT("Speichern OK."));
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
    ofn.lpstrFilter = TEXT("GPX files (*.gpx)\0*.gpx\0All Files (*.*)\0*.*\0");
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = TEXT("gpx");
    if(GetSaveFileName(&ofn))
    {
        gpxFilename = ofn.lpstrFile;
        bSuccess = (gpxFilename != "");
    }
    return bSuccess;
}
