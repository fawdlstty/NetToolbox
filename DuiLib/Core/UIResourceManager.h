#ifndef __UIRESOURCEMANAGER_H__
#define __UIRESOURCEMANAGER_H__
#pragma once

namespace DuiLib {
	// 控件文字查询接口
	class UILIB_API IQueryControlText {
	public:
		virtual faw::string_view_t QueryControlText (faw::string_view_t lpstrId, faw::string_view_t lpstrType) = 0;
	};

	class UILIB_API CResourceManager {
	private:
		CResourceManager (void);
		virtual ~CResourceManager (void);

	public:
		static CResourceManager* GetInstance () {
			static CResourceManager * p = new CResourceManager;
			return p;
		};
		void Release (void) {
			delete this;
		}

	public:
		BOOL LoadResource (std::variant<UINT, faw::String> xml, faw::string_view_t type = _T (""));
		BOOL LoadResource (CMarkupNode Root);
		void ResetResourceMap ();
		faw::string_view_t GetImagePath (faw::string_view_t lpstrId);
		faw::string_view_t GetXmlPath (faw::string_view_t lpstrId);

	public:
		void SetLanguage (faw::string_view_t pstrLanguage) { m_sLauguage = pstrLanguage; }
		faw::string_view_t GetLanguage () { return m_sLauguage.str_view (); }
		BOOL LoadLanguage (faw::string_view_t pstrXml);

	public:
		void SetTextQueryInterface (IQueryControlText* pInterface) {
			m_pQuerypInterface = pInterface;
		}
		faw::String GetText (faw::string_view_t lpstrId, faw::string_view_t lpstrType = _T (""));
		void ReloadText ();
		void ResetTextMap ();

	private:
		CStdStringPtrMap m_mTextResourceHashMap;
		IQueryControlText	*m_pQuerypInterface;
		CStdStringPtrMap m_mImageHashMap;
		CStdStringPtrMap m_mXmlHashMap;
		CMarkup m_xml;
		faw::String m_sLauguage;
		CStdStringPtrMap m_mTextHashMap;
	};

} // namespace DuiLib

#endif // __UIRESOURCEMANAGER_H__