// $Id$
// Copyright (c) 2009 Oliver Lau <oliver@ersatzworld.net>

#include "stdafx.h"
#include "wpl1000.h"

const DWORD MAX_KEY_LENGTH = 255;
const DWORD MAX_VALUE_NAME = 16383;
const DWORD MAX_LOADSTRING = 100;

const int DefaultWindowHeight = 460;
const int DefaultWindowWidth = 460;
const int DefaultWindowTop = CW_USEDEFAULT;
const int DefaultWindowLeft = CW_USEDEFAULT;

HINSTANCE ghInst;
HWND ghWnd;
HWND ghMainForm;
HMENU ghRecentFiles = NULL;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
const TCHAR* szKeyRecentFiles = TEXT("SOFTWARE\\Lau\\WPL1k\\Recent File List");
const TCHAR* szKeyWorkspace   = TEXT("SOFTWARE\\Lau\\WPL1k\\Workspace");
int nWindowWidth  = DefaultWindowWidth;
int nWindowHeight = DefaultWindowHeight;
int nWindowTop    = DefaultWindowTop;
int nWindowLeft   = DefaultWindowLeft;

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
HRESULT             LoadNVPIPE(VOID);
HRESULT             SaveGPX(VOID);

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
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeyRecentFiles, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        if (RegCreateKeyEx(HKEY_CURRENT_USER, szKeyRecentFiles, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL) != ERROR_SUCCESS)
            ErrorExit(TEXT("RegCreateKeyEx()"));
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
    DWORD dwType, dwSize;
    HKEY hKeyWorkspace;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeyWorkspace, 0, KEY_READ, &hKeyWorkspace) == ERROR_SUCCESS)
    {
        TCHAR* pszValue = (TCHAR*)LocalAlloc(LMEM_FIXED, MAX_VALUE_NAME);
        if (pszValue == NULL)
            ErrorExit(TEXT("LocalAlloc()"));

        dwSize = MAX_VALUE_NAME;
        if (RegQueryValueEx(hKeyWorkspace, "top", 0, &dwType, (LPBYTE)pszValue, &dwSize) == ERROR_SUCCESS)
            if (dwType == REG_DWORD)
                nWindowTop = *((int*)pszValue);
        dwSize = MAX_VALUE_NAME;
        if (RegQueryValueEx(hKeyWorkspace, "left", 0, &dwType, (LPBYTE)pszValue, &dwSize) == ERROR_SUCCESS)
            if (dwType == REG_DWORD)
                nWindowLeft = *((int*)pszValue);
        dwSize = MAX_VALUE_NAME;
        if (RegQueryValueEx(hKeyWorkspace, "width", 0, &dwType, (LPBYTE)pszValue, &dwSize) == ERROR_SUCCESS)
            if (dwType == REG_DWORD)
                nWindowWidth = *((int*)pszValue);
        dwSize = MAX_VALUE_NAME;
        if (RegQueryValueEx(hKeyWorkspace, "height", 0, &dwType, (LPBYTE)pszValue, &dwSize) == ERROR_SUCCESS)
            if (dwType == REG_DWORD)
                nWindowHeight = *((int*)pszValue);

        LocalFree(pszValue);
        RegCloseKey(hKeyWorkspace);
    }
}


BOOL SaveState(VOID)
{
    BOOL bSuccess = TRUE;

    SaveRecentFilesToReg();

    HKEY hKeyWorkspace;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeyWorkspace, 0, KEY_READ, &hKeyWorkspace) != ERROR_SUCCESS)
        if (RegCreateKeyEx(HKEY_CURRENT_USER, szKeyWorkspace, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyWorkspace, NULL) != ERROR_SUCCESS)
            Error(TEXT("RegCreateKeyEx()"));
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeyWorkspace, 0, KEY_SET_VALUE, &hKeyWorkspace) != ERROR_SUCCESS)
        Error(TEXT("RegOpenKeyEx()"));

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

    return bSuccess;
}


BOOL LoadRecentFilesFromReg(VOID)
{
    BOOL bSuccess = FALSE;
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szKeyRecentFiles, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        TCHAR* pszValue = (TCHAR*)LocalAlloc(LMEM_FIXED, MAX_VALUE_NAME);
        if (pszValue == NULL)
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
                *pszValue = '\0'; 
                DWORD dwType;
                DWORD dwSize = MAX_VALUE_NAME;
                retCode = RegEnumValue(hKey, i, pszValue, &cchValue, NULL,
                    &dwType, (LPBYTE)pszData, &dwSize);
                if (retCode == ERROR_SUCCESS)
                    RecentFiles.add(pszData);
            }
            LocalFree(pszData);
        }
        LocalFree(pszValue);
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
    DeleteMenu(hRecentMenu, 0, MF_BYPOSITION); // remove separator
}


VOID PopulateRecentFilesMenu(VOID)
{
    ClearRecentFilesMenu();
    HMENU hRecentMenu = GetRecentFilesMenu();
    RecentFileList::const_iterator i;
    for (i = RecentFiles.begin(); i != RecentFiles.end(); ++i)
        AppendMenu(hRecentMenu, MF_STRING, (*i).first, (*i).second.c_str());
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    ghInst = hInstance;

    LoadState();

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE | DS_CONTROL | WS_CLIPCHILDREN,
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
    LoadString(ghInst, IDS_APP_TITLE, szTitleCopy, MAX_LOADSTRING);
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
        SetWindowPos(hList, 0, 6, 36, nWidth-18, nHeight-18, SWP_NOMOVE); 
        break;
    case WM_INITDIALOG:
        {
            TRACKLISTCOLUMNTYPE cols[] = {
                { TEXT("Start"),  136, LVCFMT_LEFT  },
                { TEXT("Ende"),   124, LVCFMT_LEFT  },
                { TEXT("Punkte"),  52, LVCFMT_RIGHT },
                { TEXT("km"),      52, LVCFMT_RIGHT },
                { TEXT("Dauer"),   80, LVCFMT_LEFT  },
                { NULL,             0, 0 }
            };
            hList = GetDlgItem(hWndDlg, IDC_LISTVIEW);
            ListView_SetExtendedListViewStyle(hList,
                LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER |
                LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |
                LVS_EX_ONECLICKACTIVATE);
            LVCOLUMN lvc; 
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
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
            {
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
            }
            return 0;
        //case LVN_ITEMCHANGED:
        //    {
        //        LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
        //        if (pnmv->uNewState != pnmv->uOldState)
        //        {
        //            UINT uPrevState = (pnmv->uOldState & LVIS_STATEIMAGEMASK) >> 12;
        //            UINT uNewState = (pnmv->uNewState & LVIS_STATEIMAGEMASK) >> 12;
        //            hList = GetDlgItem(hWndDlg, IDC_LISTVIEW);
        //        }
        //    }
        //    break;
        default:
            break;
        }
    default:
        break;
    }
    return 0;
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
            DialogBox(ghInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
            {
                HRESULT res = IDCANCEL;
                do {
                    if (OpenGPX())
                        res = SaveGPX();
                }
                while (res != IDOK && res != IDCANCEL);
            }
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
                hWnd, (HMENU)IDC_MAIN_STATUS, ghInst, NULL);
            SendMessage(hStatus, SB_SETPARTS, sizeof(PartSize)/sizeof(int), (LPARAM)PartSize);
            RECT statusBarRect;
            GetClientRect(hStatus, &statusBarRect);
            hMainForm = CreateDialog(ghInst, MAKEINTRESOURCE(IDD_MAINFORM), hWnd, (DLGPROC)MainFormProc);
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
            return 1;
        }
        break;
    }
    return 0;
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


HRESULT LoadNVPIPE(VOID)
{
    wpl1000File.clearAll();
    errno_t rc = wpl1000File.load(wpl1000Filename);
    if (rc != 0) 
    {
        Error(TEXT("wpl1000File.load()"));
        return IDABORT;
    }
    if (wpl1000File.tracks().size() == 0) 
    {
        Warn(TEXT("Die Datei enth�lt keine Tracks!"));
        return IDIGNORE;
    }

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
        trk[index].pszStart = new CHAR[t0.size()+2];
        StringCchCopy(trk[index].pszStart, t0.size()+1, t0.c_str());

        const std::string& t1 = (*i)->finishTimestamp().toString();
        trk[index].pszFinish = new CHAR[t1.size()+2];
        StringCchCopy(trk[index].pszFinish, t1.size()+1, t1.c_str());

        sprintf_s(szBuf, BUFSIZE, "%.1lf", 1e-3 * (*i)->distance());
        trk[index].pszDistance = new CHAR[strlen(szBuf)+2];
        StringCchCopy(trk[index].pszDistance, strlen(szBuf)+1, szBuf);

        sprintf_s(szBuf, BUFSIZE, "%u", (*i)->points().size());
        trk[index].pszPointCount = new CHAR[strlen(szBuf)+2];
        StringCchCopy(trk[index].pszPointCount, strlen(szBuf)+1, szBuf);

        GPS::Duration d((*i)->duration());
        const std::string dstr = d.toString();
        trk[index].pszDuration = new CHAR[dstr.size()+2];
        StringCchCopy(trk[index].pszDuration, dstr.size()+1, dstr.c_str());

        lvI.iItem = index;
        lvI.iImage = index;
        lvI.iSubItem = 0;
        lvI.lParam = (LPARAM)&trk[index];
        lvI.pszText = LPSTR_TEXTCALLBACK;
        ListView_InsertItem(hList, &lvI);
        ListView_SetItemState(hList, index, LVIS_SELECTED << 12, LVIS_STATEIMAGEMASK);
        ++index;
    }
    SetStatusBar(TEXT("Laden OK."));
    return IDOK;
}


UINT_PTR CALLBACK ObsOpenDialogHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
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
    default:
        break;
    }
    return 0;
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
    ofn.lpstrDefExt = TEXT("gpx");
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY;
    ofn.Flags |= OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;
    ofn.hInstance = ghInst;
    ofn.lpTemplateName = MAKEINTRESOURCE(IDD_MULTITEMPL);
    ofn.lpfnHook = ObsOpenDialogHook;
    if (GetSaveFileName(&ofn))
    {
        gpxFilename = ofn.lpstrFile;
        bSuccess = (gpxFilename != "");
    }
    return bSuccess;
}


bool ItemIsChecked(const LPCSTR pszText)
{
    HWND hList = GetDlgItem(ghMainForm, IDC_LISTVIEW);
    int nCount = ListView_GetItemCount(hList);
    for (int iIndex = 0; iIndex < nCount; ++iIndex)
    {
        const DWORD BUFSIZE = 100;
        TCHAR achText[BUFSIZE];
        LVITEM lvi;
        lvi.iItem = iIndex;
        lvi.iSubItem = 0;
        lvi.mask = LVIF_TEXT;
        lvi.pszText = achText;
        lvi.cchTextMax = BUFSIZE;
        ListView_GetItem(hList, &lvi);
        if (StrCmp(lvi.pszText, pszText) == 0)
        {
            UINT uResult = ListView_GetItemState(hList, iIndex, LVIS_STATEIMAGEMASK) >> 12;
            return uResult == LVIS_SELECTED;
        }
    }
    return false;
}


HRESULT SaveGPX(VOID)
{
    HRESULT answer = IDYES;
    if (multi)
    {
        for (GPS::TrackList::const_iterator i = wpl1000File.tracks().begin(); i != wpl1000File.tracks().end(); ++i)
        {
            const std::string& t0 = (*i)->startTimestamp().toString();
            if (ItemIsChecked(t0.c_str()))
            {
                GPS::GPXFile trkFile;
                trkFile.addTrack(*i);
                std::basic_string<char>::size_type spos = gpxFilename.find_last_of(PathDelimiter);
                std::string trkFilename = gpxFilename;
                trkFilename.insert(spos+1, (*i)->startTimestamp().toString("%Y%m%d-%H%M") + "-");
                errno_t rc = trkFile.write(trkFilename);
                if (rc != 0)
                    Error(TEXT("trkFile.write()"));
                answer = (rc == 0)? IDOK : IDRETRY;
            }
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
        answer = (rc == 0)? IDOK : IDRETRY;
    }
    else // ! multi
    {   
        if (PathFileExists(gpxFilename.c_str()))
        {
            answer = MessageBox(ghWnd, TEXT("Die gew�hlte Datei existiert bereits. Datei �berschreiben?"),
                TEXT("Datei �berschreiben?"), MB_ICONQUESTION | MB_YESNOCANCEL);
        }
        if (answer == IDYES)
        {
            GPS::GPXFile gpxFile;
            gpxFile.setTracks(wpl1000File.tracks());
            gpxFile.setWaypoints(wpl1000File.waypoints());
            errno_t rc = gpxFile.write(gpxFilename);
            if (rc != 0)
                Error(TEXT("gpxFile.write()"));
            answer = (rc == 0)? IDOK : IDRETRY;
        }
    }
    if (answer == IDOK)
        SetStatusBar(TEXT("Speichern OK."));
    return answer;
}
