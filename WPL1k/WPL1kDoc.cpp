
// WPL1kDoc.cpp: Implementierung der Klasse CWPL1kDoc
//

#include "stdafx.h"
#include "WPL1k.h"

#include "WPL1kDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWPL1kDoc

IMPLEMENT_DYNCREATE(CWPL1kDoc, CDocument)

BEGIN_MESSAGE_MAP(CWPL1kDoc, CDocument)
END_MESSAGE_MAP()


// CWPL1kDoc-Erstellung/Zerst�rung

CWPL1kDoc::CWPL1kDoc()
{
	// TODO: Hier Code f�r One-Time-Konstruktion einf�gen

}

CWPL1kDoc::~CWPL1kDoc()
{
}

BOOL CWPL1kDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: Hier Code zur Reinitialisierung einf�gen
	// (SDI-Dokumente verwenden dieses Dokument)

	return TRUE;
}




// CWPL1kDoc-Serialisierung

void CWPL1kDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: Hier Code zum Speichern einf�gen
	}
	else
	{
		// TODO: Hier Code zum Laden einf�gen
	}
}


// CWPL1kDoc-Diagnose

#ifdef _DEBUG
void CWPL1kDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWPL1kDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CWPL1kDoc-Befehle
