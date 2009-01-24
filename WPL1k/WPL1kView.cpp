
// WPL1kView.cpp: Implementierung der Klasse CWPL1kView
//

#include "stdafx.h"
#include "WPL1k.h"

#include "WPL1kDoc.h"
#include "WPL1kView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWPL1kView

IMPLEMENT_DYNCREATE(CWPL1kView, CFormView)

BEGIN_MESSAGE_MAP(CWPL1kView, CFormView)
END_MESSAGE_MAP()

// CWPL1kView-Erstellung/Zerstörung

CWPL1kView::CWPL1kView()
	: CFormView(CWPL1kView::IDD)
{
	// TODO: Hier Code zur Konstruktion einfügen

}

CWPL1kView::~CWPL1kView()
{
}

void CWPL1kView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CWPL1kView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Ändern Sie hier die Fensterklasse oder die Darstellung, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CFormView::PreCreateWindow(cs);
}

void CWPL1kView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}

void CWPL1kView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CWPL1kView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CWPL1kView-Diagnose

#ifdef _DEBUG
void CWPL1kView::AssertValid() const
{
	CFormView::AssertValid();
}

void CWPL1kView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

#endif //_DEBUG


// CWPL1kView-Meldungshandler
