#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once
#include "OAIdl.h"
#include <vector>

namespace DuiLib {
	class UILIB_API CStdPtrArray {
	public:
		CStdPtrArray (int iPreallocSize = 0);
		CStdPtrArray (const CStdPtrArray& src);
		virtual ~CStdPtrArray ();

		void Empty ();
		void Resize (int iSize);
		bool empty () const;
		int Find (LPVOID iIndex) const;
		bool Add (LPVOID pData);
		bool SetAt (int iIndex, LPVOID pData);
		bool InsertAt (int iIndex, LPVOID pData);
		bool Remove (int iIndex);
		int GetSize () const;
		LPVOID* GetData ();

		LPVOID GetAt (int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdValArray {
	public:
		CStdValArray (int iElementSize, int iPreallocSize = 0);
		virtual ~CStdValArray ();

		void Empty ();
		bool empty () const;
		bool Add (LPCVOID pData);
		bool Remove (int iIndex);
		int GetSize () const;
		LPVOID GetData ();

		LPVOID GetAt (int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiString : public string_t {
	public:
		CDuiString ();
		CDuiString (const TCHAR ch);
		CDuiString (LPCTSTR str);
		CDuiString (const string_t& src);
		CDuiString (string_view_t lpsz, int nLen = -1);

		inline int Compare (string_view_t pstr) const { return _tcscmp (c_str (), pstr.data ()); }
		inline int CompareNoCase (string_view_t pstr) const { return _tcsicmp (c_str (), pstr.data ()); }

		inline void MakeUpper () { _tcsupr (&this->operator[] (0)); }
		inline void MakeLower () { _tcslwr (&this->operator[] (0)); }

		inline CDuiString Left (size_t nLength) const { return substr (0, nLength); }
		inline CDuiString Mid (size_t iPos, size_t nLength = string_t::npos) const { return substr (iPos, nLength); }
		inline CDuiString Right (size_t nLength) const { return substr (length () - nLength); }

		int Replace (string_view_t pstrFrom, string_view_t pstrTo);
		int Format (string_view_t pstrFormat, ...);
	};

	static std::vector<CDuiString> StrSplit (CDuiString text, CDuiString sp) {
		std::vector<CDuiString> vResults;
		size_t pos = text.find (sp);
		while (pos != string_t::npos) {
			CDuiString t = text.Left (pos);
			vResults.push_back (t);
			text = text.Right (text.length () - pos - sp.length ());
			pos = text.find (sp);
		}
		vResults.push_back (text);
		return vResults;
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//

	struct TITEM {
		CDuiString Key;
		LPVOID Data;
		struct TITEM* pPrev;
		struct TITEM* pNext;
	};

	class UILIB_API CStdStringPtrMap {
	public:
		CStdStringPtrMap (int nSize = 83);
		virtual ~CStdStringPtrMap ();

		void Resize (int nSize = 83);
		LPVOID Find (string_view_t key, bool optimize = true) const;
		bool Insert (string_view_t key, LPVOID pData);
		LPVOID Set (string_view_t key, LPVOID pData);
		bool Remove (string_view_t key);
		void RemoveAll ();
		int GetSize () const;
		TITEM *GetAt (int iIndex) const;
		TITEM *operator[] (int nIndex) const;

	protected:
		TITEM** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CWaitCursor {
	public:
		CWaitCursor ();
		virtual ~CWaitCursor ();

	protected:
		HCURSOR m_hOrigCursor;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CDuiVariant: public VARIANT {
	public:
		CDuiVariant () {
			VariantInit (this);
		}
		CDuiVariant (int i) {
			VariantInit (this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		CDuiVariant (float f) {
			VariantInit (this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		CDuiVariant (LPOLESTR s) {
			VariantInit (this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		CDuiVariant (IDispatch *disp) {
			VariantInit (this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		virtual ~CDuiVariant () {
			VariantClear (this);
		}
	};

	///////////////////////////////////////////////////////////////////////////////////////
	////
	//struct TImageInfo;
	//class CPaintManagerUI;
	//class UILIB_API CImageString
	//{
	//public:
	//	CImageString();
	//	CImageString(const CImageString&);
	//	const CImageString& operator=(const CImageString&);
	//	virtual ~CImageString();

	//	const CDuiString& GetAttributeString() const;
	//	void SetAttributeString(string_view_t pStrImageAttri);
	//	void ModifyAttribute(string_view_t pStrModify);
	//	bool LoadImage(CPaintManagerUI* pManager);
	//	bool IsLoadSuccess();

	//	RECT GetDest() const;
	//	void SetDest(const RECT &rcDest);
	//	const TImageInfo* GetImageInfo() const;

	//private:
	//	void Clone(const CImageString&);
	//	void Clear();
	//	void ParseAttribute(string_view_t pStrImageAttri);

	//protected:
	//	friend class CRenderEngine;
	//	CDuiString	m_sImageAttribute;

	//	CDuiString	m_sImage;
	//	CDuiString	m_sResType;
	//	TImageInfo	*m_imageInfo;
	//	bool		m_bLoadSuccess;

	//	RECT	m_rcDest;
	//	RECT	m_rcSource;
	//	RECT	m_rcCorner;
	//	BYTE	m_bFade;
	//	DWORD	m_dwMask;
	//	bool	m_bHole;
	//	bool	m_bTiledX;
	//	bool	m_bTiledY;
	//};
}// namespace DuiLib

#endif // __UTILS_H__