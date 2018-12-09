#ifndef __UIMARKUP_H__
#define __UIMARKUP_H__

#pragma once

namespace DuiLib {

	enum {
		XMLFILE_ENCODING_UTF8 = 0,
		XMLFILE_ENCODING_UNICODE = 1,
		XMLFILE_ENCODING_ASNI = 2,
	};

	class CMarkup;
	class CMarkupNode;


	class UILIB_API CMarkup {
		friend class CMarkupNode;
	public:
		CMarkup (string_view_t pstrXML = _T (""));
		virtual ~CMarkup ();

		bool Load (string_view_t pstrXML);
		bool LoadFromMem (BYTE* pByte, DWORD dwSize, int encoding = XMLFILE_ENCODING_UTF8);
		bool LoadFromFile (string_view_t pstrFilename, int encoding = XMLFILE_ENCODING_UTF8);
		void Release ();
		bool IsValid () const;

		void SetPreserveWhitespace (bool bPreserve = true);
		string_view_t GetLastErrorMessage () const;
		string_view_t GetLastErrorLocation () const;

		CMarkupNode GetRoot ();

	private:
		typedef struct tagXMLELEMENT {
			ULONG iStart;
			ULONG iChild;
			ULONG iNext;
			ULONG iParent;
			ULONG iData;
		} XMLELEMENT;

		string_t m_pstrXML;
		XMLELEMENT* m_pElements;
		ULONG m_nElements;
		ULONG m_nReservedElements;
		string_t m_szErrorMsg;
		string_t m_szErrorXML;
		bool m_bPreserveWhitespace;

	private:
		bool _Parse ();
		bool _Parse (LPTSTR& pstrText, ULONG iParent);
		XMLELEMENT* _ReserveElement ();
		inline void _SkipWhitespace (LPTSTR& pstr) const;
		inline void _SkipWhitespace (LPCTSTR& pstr) const;
		inline void _SkipIdentifier (LPTSTR& pstr) const;
		inline void _SkipIdentifier (LPCTSTR& pstr) const;
		bool _ParseData (LPTSTR& pstrText, LPTSTR& pstrData, char cEnd);
		void _ParseMetaChar (LPTSTR& pstrText, LPTSTR& pstrDest);
		bool _ParseAttributes (LPTSTR& pstrText);
		bool _Failed (string_view_t pstrError, string_view_t pstrLocation = _T (""));
	};


	class UILIB_API CMarkupNode {
		friend class CMarkup;
	private:
		CMarkupNode ();
		CMarkupNode (CMarkup* pOwner, int iPos);

	public:
		bool IsValid () const;

		CMarkupNode GetParent ();
		CMarkupNode GetSibling ();
		CMarkupNode GetChild ();
		CMarkupNode GetChild (string_view_t pstrName);

		bool HasSiblings () const;
		bool HasChildren () const;
		string_t GetName () const;
		string_t GetValue () const;

		bool HasAttributes ();
		bool HasAttribute (string_view_t pstrName);
		int GetAttributeCount ();
		string_t GetAttributeName (int iIndex);
		string_t GetAttributeValue (int iIndex);
		string_t GetAttributeValue (string_view_t pstrName);

	private:
		void _MapAttributes ();

		enum { MAX_XML_ATTRIBUTES = 64 };

		typedef struct {
			ULONG iName;
			ULONG iValue;
		} XMLATTRIBUTE;

		int m_iPos;
		int m_nAttributes;
		XMLATTRIBUTE m_aAttributes[MAX_XML_ATTRIBUTES];
		CMarkup* m_pOwner;
	};

} // namespace DuiLib

#endif // __UIMARKUP_H__
