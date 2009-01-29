// $Id$
// Copyright (c) 2009 Oliver Lau <oliver@ersatzworld.net>

#include "stdafx.h"

VOID Warn(LPTSTR lpszMessage)
{
    MessageBox(NULL, (LPCTSTR)lpszMessage, TEXT("Fehler"), MB_OK); 
}


VOID Error(LPTSTR lpszFunction, LONG lErrCode)
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    if (lErrCode == 0)
        lErrCode = GetLastError();
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        lErrCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));

    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s fehlgeschlagen mit Fehler %d: %s"), 
        lpszFunction, lErrCode, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Fehler"), MB_OK); 
    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}


VOID ErrorExit(LPTSTR lpszFunction, LONG lErrCode)
{
    Error(lpszFunction, lErrCode);
    ExitProcess(lErrCode); 
}
