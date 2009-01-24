
// WPL1k.h: Hauptheaderdatei f�r die WPL1k-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
	#error "'stdafx.h' vor dieser Datei f�r PCH einschlie�en"
#endif

#include "resource.h"       // Hauptsymbole


// CWPL1kApp:
// Siehe WPL1k.cpp f�r die Implementierung dieser Klasse
//

class CWPL1kApp : public CWinAppEx
{
public:
	CWPL1kApp();


// �berschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CWPL1kApp theApp;
