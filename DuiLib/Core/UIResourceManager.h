#ifndef __UIRESOURCEMANAGER_H__
#define __UIRESOURCEMANAGER_H__
#pragma once

namespace DuiLib {
	// 控件文字查询接口
	class UILIB_API IQueryControlText {
	public:
		virtual string_view_t QueryControlText (string_view_t lpstrId, string_view_t lpstrType) = 0;
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
		BOOL LoadResource (std::variant<UINT, string_t> xml, string_view_t type = _T (""));
		BOOL LoadResource (CMarkupNode Root);
		void ResetResourceMap ();
		string_view_t GetImagePath (string_view_t lpstrId);
		string_view_t GetXmlPath (string_view_t lpstrId);

	public:
		void SetLanguage (string_view_t pstrLanguage) { m_sLauguage = pstrLanguage; }
		string_view_t GetLanguage () { return m_sLauguage; }
		BOOL LoadLanguage (string_view_t pstrXml);

	public:
		void SetTextQueryInterface (IQueryControlText* pInterface) {
			m_pQuerypInterface = pInterface;
		}
		CDuiString GetText (string_view_t lpstrId, string_view_t lpstrType = _T (""));
		void ReloadText ();
		void ResetTextMap ();

	private:
		CStdStringPtrMap m_mTextResourceHashMap;
		IQueryControlText	*m_pQuerypInterface;
		CStdStringPtrMap m_mImageHashMap;
		CStdStringPtrMap m_mXmlHashMap;
		CMarkup m_xml;
		CDuiString m_sLauguage;
		CStdStringPtrMap m_mTextHashMap;
	};

} // namespace DuiLib

#endif // __UIRESOURCEMANAGER_H__