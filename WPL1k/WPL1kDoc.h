
// WPL1kDoc.h: Schnittstelle der Klasse CWPL1kDoc
//


#pragma once


class CWPL1kDoc : public CDocument
{
protected: // Nur aus Serialisierung erstellen
	CWPL1kDoc();
	DECLARE_DYNCREATE(CWPL1kDoc)

// Attribute
public:

// Vorg�nge
public:

// �berschreibungen
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementierung
public:
	virtual ~CWPL1kDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen f�r die Meldungstabellen
protected:
	DECLARE_MESSAGE_MAP()
};


