// $Id$
// Copyright (c) 2009 Oliver Lau <oliver@ersatzworld.net>

#include "stdafx.h"

VOID Warn(LPTSTR lpszMessage, DWORD dw)
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    if (dw == 0)
        dw = GetLastError();
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszMessage) + 40) * sizeof(TCHAR));

    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s fehlgeschlagen mit Fehler %d: %s"), 
        lpszMessage, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Fehler"), MB_OK); 
    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}


VOID ErrorExit(LPTSTR lpszFunction, DWORD dw) 
{ 
    Warn(lpszFunction, dw);
    ExitProcess(dw); 
}
