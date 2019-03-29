#include "StdAfx.h"

#ifndef TRACE
#define TRACE
#endif

namespace DuiLib {
	///////////////////////////////////////////////////////////////////////////////////////
	//
	//
	//
	CMarkupNode::CMarkupNode (): m_pOwner (nullptr) {}

	CMarkupNode::CMarkupNode (CMarkup* pOwner, int iPos) : m_pOwner (pOwner), m_iPos (iPos), m_nAttributes (0) {}

	CMarkupNode CMarkupNode::GetSibling () {
		if (!m_pOwner) return CMarkupNode ();
		ULONG iPos = m_pOwner->m_pElements[m_iPos].iNext;
		if (iPos == 0) return CMarkupNode ();
		return CMarkupNode (m_pOwner, iPos);
	}

	bool CMarkupNode::HasSiblings () const {
		if (!m_pOwner) return false;
		ULONG iPos = m_pOwner->m_pElements[m_iPos].iNext;
		return iPos > 0;
	}

	CMarkupNode CMarkupNode::GetChild () {
		if (!m_pOwner) return CMarkupNode ();
		ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
		if (iPos == 0) return CMarkupNode ();
		return CMarkupNode (m_pOwner, iPos);
	}

	CMarkupNode CMarkupNode::GetChild (faw::string_view_t pstrName) {
		if (!m_pOwner) return CMarkupNode ();
		ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
		while (iPos != 0) {
			if (pstrName == &m_pOwner->m_pstrXML[m_pOwner->m_pElements[iPos].iStart]) {
				return CMarkupNode (m_pOwner, iPos);
			}
			iPos = m_pOwner->m_pElements[iPos].iNext;
		}
		return CMarkupNode ();
	}

	bool CMarkupNode::HasChildren () const {
		if (!m_pOwner) return false;
		return m_pOwner->m_pElements[m_iPos].iChild != 0;
	}

	CMarkupNode CMarkupNode::GetParent () {
		if (!m_pOwner) return CMarkupNode ();
		ULONG iPos = m_pOwner->m_pElements[m_iPos].iParent;
		if (iPos == 0) return CMarkupNode ();
		return CMarkupNode (m_pOwner, iPos);
	}

	bool CMarkupNode::IsValid () const {
		return m_pOwner;
	}

	faw::String CMarkupNode::GetName () const {
		if (!m_pOwner) return _T ("");
		return &m_pOwner->m_pstrXML[m_pOwner->m_pElements[m_iPos].iStart];
	}

	faw::String CMarkupNode::GetValue () const {
		if (!m_pOwner) return _T ("");
		return &m_pOwner->m_pstrXML[m_pOwner->m_pElements[m_iPos].iData];
	}

	faw::String CMarkupNode::GetAttributeName (int iIndex) {
		if (!m_pOwner) return _T ("");
		if (m_nAttributes == 0) _MapAttributes ();
		if (iIndex < 0 || iIndex >= m_nAttributes) return _T ("");
		return &m_pOwner->m_pstrXML[m_aAttributes[iIndex].iName];
	}

	faw::String CMarkupNode::GetAttributeValue (int iIndex) {
		if (!m_pOwner) return _T ("");
		if (m_nAttributes == 0) _MapAttributes ();
		if (iIndex < 0 || iIndex >= m_nAttributes) return _T ("");
		return &m_pOwner->m_pstrXML[m_aAttributes[iIndex].iValue];
	}

	faw::String CMarkupNode::GetAttributeValue (faw::string_view_t pstrName) {
		if (!m_pOwner) return _T ("");
		if (m_nAttributes == 0) _MapAttributes ();
		for (int i = 0; i < m_nAttributes; i++) {
			if (pstrName == &m_pOwner->m_pstrXML[m_aAttributes[i].iName]) return &m_pOwner->m_pstrXML[m_aAttributes[i].iValue];
		}
		return _T ("");
	}

	int CMarkupNode::GetAttributeCount () {
		if (!m_pOwner) return 0;
		if (m_nAttributes == 0) _MapAttributes ();
		return m_nAttributes;
	}

	bool CMarkupNode::HasAttributes () {
		if (!m_pOwner) return false;
		if (m_nAttributes == 0) _MapAttributes ();
		return m_nAttributes > 0;
	}

	bool CMarkupNode::HasAttribute (faw::string_view_t pstrName) {
		if (!m_pOwner) return false;
		if (m_nAttributes == 0) _MapAttributes ();
		for (int i = 0; i < m_nAttributes; i++) {
			if (pstrName == &m_pOwner->m_pstrXML[m_aAttributes[i].iName]) return true;
		}
		return false;
	}

	void CMarkupNode::_MapAttributes () {
		m_nAttributes = 0;
		LPCTSTR pstr = &m_pOwner->m_pstrXML[m_pOwner->m_pElements[m_iPos].iStart];
		LPCTSTR pstrEnd = &m_pOwner->m_pstrXML[m_pOwner->m_pElements[m_iPos].iData];
		pstr += _tcslen (pstr) + 1;
		while (pstr < pstrEnd) {
			m_pOwner->_SkipWhitespace (pstr);
			m_aAttributes[m_nAttributes].iName = (ULONG) (pstr - &m_pOwner->m_pstrXML[0]);
			pstr += _tcslen (pstr) + 1;
			m_pOwner->_SkipWhitespace (pstr);
			if (*pstr++ != _T ('\"')) return; // if( *pstr != _T('\"') ) { pstr = ::CharNext(pstr); return; }

			m_aAttributes[m_nAttributes++].iValue = (ULONG) (pstr - &m_pOwner->m_pstrXML[0]);
			if (m_nAttributes >= MAX_XML_ATTRIBUTES) return;
			pstr += _tcslen (pstr) + 1;
		}
	}


	///////////////////////////////////////////////////////////////////////////////////////
	//
	//
	//

	CMarkup::CMarkup (faw::string_view_t pstrXML) {
		m_pstrXML = _T ("");
		m_pElements = nullptr;
		m_nElements = 0;
		m_bPreserveWhitespace = true;
		if (!pstrXML.empty ()) Load (pstrXML);
	}

	CMarkup::~CMarkup () {
		Release ();
	}

	bool CMarkup::IsValid () const {
		return m_pElements;
	}

	void CMarkup::SetPreserveWhitespace (bool bPreserve) {
		m_bPreserveWhitespace = bPreserve;
	}

	bool CMarkup::Load (faw::string_view_t pstrXML) {
		Release ();
		m_pstrXML = pstrXML;
		bool bRes = _Parse ();
		if (!bRes) Release ();
		return bRes;
	}

	bool CMarkup::LoadFromMem (BYTE* pByte, DWORD dwSize, int encoding) {
		if (encoding == XMLFILE_ENCODING_UTF8) {
			if (dwSize >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF) {
				pByte += 3; dwSize -= 3;
			}
			std::string_view data ((const char*) pByte, (size_t) dwSize);
			m_pstrXML = FawTools::utf8_to_T (data);
		} else if (encoding == XMLFILE_ENCODING_ASNI) {
			std::string_view data ((const char*) pByte, (size_t) dwSize);
			m_pstrXML = FawTools::gb18030_to_T (data);
		} else {
			if (dwSize >= 2 && ((pByte[0] == 0xFE && pByte[1] == 0xFF) || (pByte[0] == 0xFF && pByte[1] == 0xFE))) {
				dwSize = dwSize / 2 - 1;
				if (pByte[0] == 0xFE && pByte[1] == 0xFF) {
					pByte += 2;
					for (DWORD nSwap = 0; nSwap < dwSize; nSwap++) {
						CHAR nTemp = pByte[(nSwap << 1) + 0];
						pByte[(nSwap << 1) + 0] = pByte[(nSwap << 1) + 1];
						pByte[(nSwap << 1) + 1] = nTemp;
					}
				} else {
					pByte += 2;
				}
				std::wstring_view data ((const wchar_t*) pByte, (size_t) dwSize);
				m_pstrXML = FawTools::utf16_to_T (data);
				pByte -= 2;
			}
		}

		bool bRes = _Parse ();
		if (!bRes) Release ();
		return bRes;
	}

	bool CMarkup::LoadFromFile (faw::string_view_t pstrFilename, int encoding) {
		Release ();
		faw::String sFile = CPaintManagerUI::GetResourcePath ();
		if (CPaintManagerUI::GetResourceZip ().empty ()) {
			sFile += pstrFilename;
			HANDLE hFile = ::CreateFile (sFile.c_str (), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) return _Failed (_T ("Error opening file"));
			DWORD dwSize = ::GetFileSize (hFile, nullptr);
			if (dwSize == 0) return _Failed (_T ("File is empty"));
			if (dwSize > 4096 * 1024) return _Failed (_T ("File too large"));

			DWORD dwRead = 0;
			BYTE* pByte = new BYTE[dwSize];
			::ReadFile (hFile, pByte, dwSize, &dwRead, nullptr);
			::CloseHandle (hFile);
			if (dwRead != dwSize) {
				delete[] pByte;
				pByte = nullptr;
				Release ();
				return _Failed (_T ("Could not read file"));
			}

			bool ret = LoadFromMem (pByte, dwSize, encoding);
			delete[] pByte;
			pByte = nullptr;

			return ret;
		} else {
			sFile += CPaintManagerUI::GetResourceZip ();
			HZIP hz = nullptr;
			if (CPaintManagerUI::IsCachedResourceZip ()) hz = (HZIP) CPaintManagerUI::GetResourceZipHandle ();
			else {
				faw::String sFilePwd = CPaintManagerUI::GetResourceZipPwd ();
				std::string pwd = sFilePwd.stra ();
				hz = OpenZip (sFile.c_str (), pwd.c_str ());
			}
			if (!hz) return _Failed (_T ("Error opening zip file"));
			ZIPENTRY ze;
			int i = 0;
			faw::String key = pstrFilename;
			key.replace_self (_T ("\\"), _T ("/"));
			if (FindZipItem (hz, key.c_str (), true, &i, &ze) != 0) return _Failed (_T ("Could not find ziped file"));
			DWORD dwSize = ze.unc_size;
			if (dwSize == 0) return _Failed (_T ("File is empty"));
			if (dwSize > 4096 * 1024) return _Failed (_T ("File too large"));
			BYTE* pByte = new BYTE[dwSize];
			int res = UnzipItem (hz, i, pByte, dwSize);
			if (res != 0x00000000 && res != 0x00000600) {
				delete[] pByte;
				if (!CPaintManagerUI::IsCachedResourceZip ()) CloseZip (hz);
				return _Failed (_T ("Could not unzip file"));
			}
			if (!CPaintManagerUI::IsCachedResourceZip ()) CloseZip (hz);
			bool ret = LoadFromMem (pByte, dwSize, encoding);
			delete[] pByte;
			pByte = nullptr;
			return ret;
		}
	}

	void CMarkup::Release () {
		if (m_pElements) free (m_pElements);
		m_pstrXML = _T ("");
		m_pElements = nullptr;
		m_nElements = 0;
	}

	faw::string_view_t CMarkup::GetLastErrorMessage () const {
		return m_szErrorMsg.str_view ();
	}

	faw::string_view_t CMarkup::GetLastErrorLocation () const {
		return m_szErrorXML.str_view ();
	}

	CMarkupNode CMarkup::GetRoot () {
		if (m_nElements == 0) return CMarkupNode ();
		return CMarkupNode (this, 1);
	}

	bool CMarkup::_Parse () {
		_ReserveElement (); // Reserve index 0 for errors
		m_szErrorMsg = _T ("");
		m_szErrorXML = _T ("");
		LPTSTR pstrXML = &m_pstrXML[0];
		return _Parse (pstrXML, 0);
	}

	bool CMarkup::_Parse (LPTSTR& pstrText, ULONG iParent) {
		_SkipWhitespace (pstrText);
		ULONG iPrevious = 0;
		for (; ; ) {
			if (*pstrText == _T ('\0') && iParent <= 1)
				return true;
			_SkipWhitespace (pstrText);
			if (*pstrText != _T ('<'))
				return _Failed (_T ("Expected start tag"), pstrText);
			if (pstrText[1] == _T ('/'))
				return true;
			*pstrText++ = _T ('\0');
			_SkipWhitespace (pstrText);
			// Skip comment or processing directive
			if (*pstrText == _T ('!') || *pstrText == _T ('?')) {
				TCHAR ch = *pstrText;
				if (*pstrText == _T ('!')) ch = _T ('-');
				while (*pstrText != _T ('\0') && !(*pstrText == ch && *(pstrText + 1) == _T ('>'))) pstrText = ::CharNext (pstrText);
				if (*pstrText != _T ('\0')) pstrText += 2;
				_SkipWhitespace (pstrText);
				continue;
			}
			_SkipWhitespace (pstrText);
			// Fill out element structure
			XMLELEMENT* pEl = _ReserveElement ();
			ULONG iPos = pEl - m_pElements;
			pEl->iStart = (ULONG) (pstrText - &m_pstrXML[0]);
			pEl->iParent = iParent;
			pEl->iNext = pEl->iChild = 0;
			if (iPrevious != 0) m_pElements[iPrevious].iNext = iPos;
			else if (iParent > 0) m_pElements[iParent].iChild = iPos;
			iPrevious = iPos;
			// Parse name
			LPCTSTR pstrName = pstrText;
			_SkipIdentifier (pstrText);
			LPTSTR pstrNameEnd = pstrText;
			if (*pstrText == _T ('\0'))
				return _Failed (_T ("Error parsing element name"), pstrText);
			// Parse attributes
			if (!_ParseAttributes (pstrText))
				return false;
			_SkipWhitespace (pstrText);
			if (pstrText[0] == _T ('/') && pstrText[1] == _T ('>')) {
				pEl->iData = (ULONG) (pstrText - &m_pstrXML[0]);
				*pstrText = _T ('\0');
				pstrText += 2;
			} else {
				if (*pstrText != _T ('>'))
					return _Failed (_T ("Expected start-tag closing"), pstrText);
				// Parse node data
				pEl->iData = (ULONG) (++pstrText - &m_pstrXML[0]);
				LPTSTR pstrDest = pstrText;
				if (!_ParseData (pstrText, pstrDest, _T ('<')))
					return false;
				// Determine type of next element
				if (*pstrText == _T ('\0') && iParent <= 1)
					return true;
				if (*pstrText != _T ('<'))
					return _Failed (_T ("Expected end-tag start"), pstrText);
				if (pstrText[0] == _T ('<') && pstrText[1] != _T ('/')) {
					if (!_Parse (pstrText, iPos))
						return false;
				}
				if (pstrText[0] == _T ('<') && pstrText[1] == _T ('/')) {
					*pstrDest = _T ('\0');
					*pstrText = _T ('\0');
					pstrText += 2;
					_SkipWhitespace (pstrText);
					SIZE_T cchName = pstrNameEnd - pstrName;
					if (_tcsncmp (pstrText, pstrName, cchName) != 0)
						return _Failed (_T ("Unmatched closing tag"), pstrText);
					pstrText += cchName;
					_SkipWhitespace (pstrText);
					if (*pstrText++ != _T ('>'))
						return _Failed (_T ("Unmatched closing tag"), pstrText);
				}
			}
			*pstrNameEnd = _T ('\0');
			_SkipWhitespace (pstrText);
		}
	}

	CMarkup::XMLELEMENT* CMarkup::_ReserveElement () {
		if (m_nElements == 0) m_nReservedElements = 0;
		if (m_nElements >= m_nReservedElements) {
			m_nReservedElements += (m_nReservedElements / 2) + 500;
			m_pElements = static_cast<XMLELEMENT*>(realloc (m_pElements, m_nReservedElements * sizeof (XMLELEMENT)));
		}
		return &m_pElements[m_nElements++];
	}

	void CMarkup::_SkipWhitespace (LPTSTR& pstr) const {
		while (*pstr > _T ('\0') && *pstr <= _T (' ')) pstr = ::CharNext (pstr);
	}

	void CMarkup::_SkipWhitespace (LPCTSTR& pstr) const {
		while (*pstr > _T ('\0') && *pstr <= _T (' ')) pstr = ::CharNext (pstr);
	}

	void CMarkup::_SkipIdentifier (LPTSTR& pstr) const {
		// 属性只能用英文，所以这样处理没有问题
		while (*pstr != _T ('\0') && (*pstr == _T ('_') || *pstr == _T (':') || _istalnum (*pstr))) pstr = ::CharNext (pstr);
	}

	void CMarkup::_SkipIdentifier (LPCTSTR& pstr) const {
		// 属性只能用英文，所以这样处理没有问题
		while (*pstr != _T ('\0') && (*pstr == _T ('_') || *pstr == _T (':') || _istalnum (*pstr))) pstr = ::CharNext (pstr);
	}

	bool CMarkup::_ParseAttributes (LPTSTR& pstrText) {
		// 无属性
		LPTSTR pstrIdentifier = pstrText;
		if (*pstrIdentifier == _T ('/') && *++pstrIdentifier == _T ('>')) return true;
		if (*pstrText == _T ('>')) return true;
		*pstrText++ = _T ('\0');
		_SkipWhitespace (pstrText);
		while (*pstrText != _T ('\0') && *pstrText != _T ('>') && *pstrText != _T ('/')) {
			_SkipIdentifier (pstrText);
			LPTSTR pstrIdentifierEnd = pstrText;
			_SkipWhitespace (pstrText);
			if (*pstrText != _T ('=')) return _Failed (_T ("Error while parsing attributes"), pstrText);
			*pstrText++ = _T (' ');
			*pstrIdentifierEnd = _T ('\0');
			_SkipWhitespace (pstrText);
			if (*pstrText++ != _T ('\"')) return _Failed (_T ("Expected attribute value"), pstrText);
			LPTSTR pstrDest = pstrText;
			if (!_ParseData (pstrText, pstrDest, _T ('\"'))) return false;
			if (*pstrText == _T ('\0')) return _Failed (_T ("Error while parsing attribute string"), pstrText);
			*pstrDest = _T ('\0');
			if (pstrText != pstrDest) *pstrText = _T (' ');
			pstrText++;
			_SkipWhitespace (pstrText);
		}
		return true;
	}

	bool CMarkup::_ParseData (LPTSTR& pstrText, LPTSTR& pstrDest, char cEnd) {
		while (*pstrText != _T ('\0') && *pstrText != cEnd) {
			if (*pstrText == _T ('&')) {
				while (*pstrText == _T ('&')) {
					_ParseMetaChar (++pstrText, pstrDest);
				}
				if (*pstrText == cEnd)
					break;
			}

			if (*pstrText == _T (' ')) {
				*pstrDest++ = *pstrText++;
				if (!m_bPreserveWhitespace) _SkipWhitespace (pstrText);
			} else {
				LPTSTR pstrTemp = ::CharNext (pstrText);
				while (pstrText < pstrTemp) {
					*pstrDest++ = *pstrText++;
				}
			}
		}
		// Make sure that MapAttributes() works correctly when it parses
		// over a value that has been transformed.
		LPTSTR pstrFill = pstrDest + 1;
		while (pstrFill < pstrText) *pstrFill++ = _T (' ');
		return true;
	}

	void CMarkup::_ParseMetaChar (LPTSTR& pstrText, LPTSTR& pstrDest) {
		if (pstrText[0] == _T ('a') && pstrText[1] == _T ('m') && pstrText[2] == _T ('p') && pstrText[3] == _T (';')) {
			*pstrDest++ = _T ('&');
			pstrText += 4;
		} else if (pstrText[0] == _T ('l') && pstrText[1] == _T ('t') && pstrText[2] == _T (';')) {
			*pstrDest++ = _T ('<');
			pstrText += 3;
		} else if (pstrText[0] == _T ('g') && pstrText[1] == _T ('t') && pstrText[2] == _T (';')) {
			*pstrDest++ = _T ('>');
			pstrText += 3;
		} else if (pstrText[0] == _T ('q') && pstrText[1] == _T ('u') && pstrText[2] == _T ('o') && pstrText[3] == _T ('t') && pstrText[4] == _T (';')) {
			*pstrDest++ = _T ('\"');
			pstrText += 5;
		} else if (pstrText[0] == _T ('a') && pstrText[1] == _T ('p') && pstrText[2] == _T ('o') && pstrText[3] == _T ('s') && pstrText[4] == _T (';')) {
			*pstrDest++ = _T ('\'');
			pstrText += 5;
		} else {
			*pstrDest++ = _T ('&');
		}
	}

	bool CMarkup::_Failed (faw::string_view_t pstrError, faw::string_view_t pstrLocation) {
		// Register last error
		TRACE (_T ("XML Error: %s"), pstrError.data ());
		if (!pstrLocation.empty ()) TRACE (pstrLocation);
		m_szErrorMsg = pstrError;
		m_szErrorXML = pstrLocation;
		return false; // Always return 'false'
	}

} // namespace DuiLib
