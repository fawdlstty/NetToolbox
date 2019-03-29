#ifndef __UICOMBOBOX_H__
#define __UICOMBOBOX_H__

#pragma once

namespace DuiLib {
	/// 扩展下拉列表框
	/// 增加arrowimage属性,一张图片平均分成5份,Normal/Hot/Pushed/Focused/Disabled(必须有source属性)
	/// <Default name="ComboBox" value="arrowimage=&quot;file='sys_combo_btn.png' source='0,0,16,16'&quot; "/>
	class UILIB_API CComboBoxUI: public CComboUI {
		DECLARE_DUICONTROL (CComboBoxUI)
	public:
		CComboBoxUI ();
		faw::string_view_t GetClass () const;

		void SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue);

		void PaintText (HDC hDC);
		void PaintStatusImage (HDC hDC);

	protected:
		faw::String m_sArrowImage;
		int        m_nArrowWidth	= 0;
	};
}

#endif // __UICOMBOBOX_H__
