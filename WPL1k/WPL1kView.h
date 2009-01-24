
// WPL1kView.h: Schnittstelle der Klasse CWPL1kView
//


#pragma once


class CWPL1kView : public CFormView
{
protected: // Nur aus Serialisierung erstellen
	CWPL1kView();
	DECLARE_DYNCREATE(CWPL1kView)

public:
	enum{ IDD = IDD_WPL1K_FORM };

// Attribute
public:
	CWPL1kDoc* GetDocument() const;

// Vorgänge
public:

// Überschreibungen
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual void OnInitialUpdate(); // Erster Aufruf nach Erstellung

// Implementierung
public:
	virtual ~CWPL1kView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Debugversion in WPL1kView.cpp
inline CWPL1kDoc* CWPL1kView::GetDocument() const
   { return reinterpret_cast<CWPL1kDoc*>(m_pDocument); }
#endif

