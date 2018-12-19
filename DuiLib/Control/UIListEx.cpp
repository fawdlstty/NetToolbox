#include "stdafx.h"
#include "UIListEx.h"

namespace DuiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL (CListExUI)

	CListExUI::CListExUI () {}

	string_view_t CListExUI::GetClass () const {
		return _T ("XListUI");
	}

	UINT CListExUI::GetControlFlags () const {
		return UIFLAG_TABSTOP;
	}

	LPVOID CListExUI::GetInterface (string_view_t pstrName) {
		if (pstrName == _T ("ListEx")) return static_cast<IListOwnerUI*>(this);
		return CListUI::GetInterface (pstrName);
	}
	BOOL CListExUI::CheckColumEditable (int nColum) {
		CListContainerHeaderItemUI* pHItem = static_cast<CListContainerHeaderItemUI*>(m_pHeader->GetItemAt (nColum));
		return pHItem ? pHItem->GetColumeEditable () : FALSE;
	}
	void CListExUI::InitListCtrl () {
		if (!m_bAddMessageFilter) {
			GetManager ()->AddNotifier (this);
			m_bAddMessageFilter = TRUE;
		}
	}
	CRichEditUI* CListExUI::GetEditUI () {
		if (!m_pEditUI) {
			m_pEditUI = new CRichEditUI;
			m_pEditUI->SetName (_T ("ListEx_Edit"));
			string_view_t pDefaultAttributes = GetManager ()->GetDefaultAttributeList (_T ("RichEdit"));
			if (!pDefaultAttributes.empty ()) {
				m_pEditUI->ApplyAttributeList (pDefaultAttributes);
			}

			m_pEditUI->SetBkColor (0xFFFFFFFF);
			m_pEditUI->SetRich (false);
			m_pEditUI->SetMultiLine (false);
			m_pEditUI->SetWantReturn (true);
			m_pEditUI->SetFloat (true);
			m_pEditUI->SetAttribute (_T ("autohscroll"), _T ("true"));
			Add (m_pEditUI);
		}
		if (m_pComboBoxUI) {
			RECT rc = { 0, 0, 0, 0 };
			m_pComboBoxUI->SetPos (rc);
		}

		return m_pEditUI;
	}

	BOOL CListExUI::CheckColumComboBoxable (int nColum) {
		CListContainerHeaderItemUI* pHItem = static_cast<CListContainerHeaderItemUI*>(m_pHeader->GetItemAt (nColum));
		return pHItem ? pHItem->GetColumeComboable () : FALSE;
	}

	CComboBoxUI* CListExUI::GetComboBoxUI () {
		if (!m_pComboBoxUI) {
			m_pComboBoxUI = new CComboBoxUI;
			m_pComboBoxUI->SetName (_T ("ListEx_Combo"));
			string_view_t pDefaultAttributes = GetManager ()->GetDefaultAttributeList (_T ("Combo"));
			if (!pDefaultAttributes.empty ()) {
				m_pComboBoxUI->ApplyAttributeList (pDefaultAttributes);
			}

			Add (m_pComboBoxUI);
		}
		if (m_pEditUI) {
			RECT rc = { 0, 0, 0, 0 };
			m_pEditUI->SetPos (rc);
		}

		return m_pComboBoxUI;
	}

	BOOL CListExUI::CheckColumCheckBoxable (int nColum) {
		CControlUI* p = m_pHeader->GetItemAt (nColum);
		CListContainerHeaderItemUI* pHItem = static_cast<CListContainerHeaderItemUI*>(p->GetInterface (_T ("ListContainerHeaderItem")));
		return pHItem ? pHItem->GetColumeCheckable () : FALSE;
	}

	void CListExUI::Notify (TNotifyUI& msg) {
		CDuiString strName = msg.pSender->GetName ();

		//复选框
		if (msg.sType == _T ("listheaditemchecked")) {
			BOOL bCheck = (BOOL) msg.lParam;
			//判断是否是本LIST发送的notify
			CListHeaderUI* pHeader = GetHeader ();
			for (int i = 0; i < pHeader->GetCount (); i++) {
				if (pHeader->GetItemAt (i) == msg.pSender) {
					for (int j = 0; j < GetCount (); ++j) {
						CControlUI* p = GetItemAt (j);
						CListTextExtElementUI* pLItem = static_cast<CListTextExtElementUI*>(p->GetInterface (_T ("ListTextExElement")));
						if (pLItem) {
							pLItem->SetCheck (bCheck);
						}
					}
					break;
				}
			}
		} else if (msg.sType == DUI_MSGTYPE_LISTITEMCHECKED) {
			for (int i = 0; i < GetCount (); ++i) {
				CControlUI* p = GetItemAt (i);
				CListTextExtElementUI* pLItem = static_cast<CListTextExtElementUI*>(p->GetInterface (_T ("ListTextExElement")));
				if (pLItem && pLItem == msg.pSender) {
					OnListItemChecked (LOWORD (msg.wParam), HIWORD (msg.wParam), (BOOL) msg.lParam);
					break;
				}
			}
		}

		//编辑框、组合框
		if (strName == _T ("ListEx_Edit") && m_pEditUI && m_nRow >= 0 && m_nColum >= 0) {
			if (msg.sType == DUI_MSGTYPE_SETFOCUS) {

			} else if (msg.sType == DUI_MSGTYPE_KILLFOCUS) {
				CDuiString sText = m_pEditUI->GetText ();
				CListTextExtElementUI* pRowCtrl = (CListTextExtElementUI*) GetItemAt (m_nRow);
				if (pRowCtrl) {
					pRowCtrl->SetText (m_nColum, sText);
				}

				//重置当前行列
				SetEditRowAndColum (-1, -1);

				//隐藏编辑框
				RECT rc = { 0, 0, 0, 0 };
				m_pEditUI->SetPos (rc);
				m_pEditUI->SetVisible (false);
			}
		} else if (strName == _T ("ListEx_Combo") && m_pComboBoxUI && m_nRow >= 0 && m_nColum >= 0) {
			int  iCurSel, iOldSel;
			iCurSel = (int) msg.wParam;
			iOldSel = (int) msg.lParam;

			if (msg.sType == DUI_MSGTYPE_SETFOCUS) {

			} else if (msg.sType == DUI_MSGTYPE_KILLFOCUS) {
			} else if (msg.sType == DUI_MSGTYPE_LISTITEMSELECT && iOldSel >= 0) {
				CListTextExtElementUI* pRowCtrl = (CListTextExtElementUI*) GetItemAt (m_nRow);
				if (pRowCtrl) {
					pRowCtrl->SetText (m_nColum, m_pComboBoxUI->GetText ());
				}

				//隐藏组合框
				RECT rc = { 0, 0, 0, 0 };
				m_pComboBoxUI->SetPos (rc);
			}
		} else if (msg.sType == _T ("scroll") && (m_pComboBoxUI || m_pEditUI) && m_nRow >= 0 && m_nColum >= 0) {
			HideEditAndComboCtrl ();
		}
	}
	void CListExUI::HideEditAndComboCtrl () {
		//隐藏编辑框
		RECT rc = { 0, 0, 0, 0 };
		if (m_pEditUI) {
			m_pEditUI->SetVisible (false);
		}

		if (m_pComboBoxUI) {
			m_pComboBoxUI->SetPos (rc);
		}
	}
	IListComboCallbackUI* CListExUI::GetTextArrayCallback () const {
		return m_pXCallback;
	}

	void CListExUI::SetTextArrayCallback (IListComboCallbackUI* pCallback) {
		m_pXCallback = pCallback;
	}
	void CListExUI::OnListItemClicked (int nIndex, int nColum, RECT* lpRCColum, string_view_t lpstrText) {
		RECT rc = { 0, 0, 0, 0 };
		if (nColum < 0) {
			if (m_pEditUI) {
				m_pEditUI->SetPos (rc);
				m_pEditUI->SetVisible (false);
			}
			if (m_pComboBoxUI) {
				m_pComboBoxUI->SetPos (rc);
			}
		} else {
			if (CheckColumEditable (nColum) && GetEditUI ()) {
				//保存当前行列
				SetEditRowAndColum (nIndex, nColum);

				m_pEditUI->SetVisible (true);
				//移动位置
				m_pEditUI->SetFixedWidth (lpRCColum->right - lpRCColum->left);
				m_pEditUI->SetFixedHeight (lpRCColum->bottom - lpRCColum->top);
				m_pEditUI->SetFixedXY ({ lpRCColum->left, lpRCColum->top });
				SIZE szTextSize = CRenderEngine::GetTextSize (m_pManager->GetPaintDC (), m_pManager, _T ("TTT"), m_ListInfo.nFont, DT_CALCRECT | DT_SINGLELINE);
				m_pEditUI->SetTextPadding ({ 2, (lpRCColum->bottom - lpRCColum->top - szTextSize.cy) / 2, 2, 0 });

				//设置文字
				m_pEditUI->SetText (lpstrText);

				m_pEditUI->SetFocus ();
			} else if (CheckColumComboBoxable (nColum) && GetComboBoxUI ()) {
				//重置组合框
				m_pComboBoxUI->RemoveAll ();

				//保存当前行列
				SetEditRowAndColum (nIndex, nColum);

				//设置文字
				m_pComboBoxUI->SetText (lpstrText);

				//获取
				if (m_pXCallback) {
					m_pXCallback->GetItemComboTextArray (m_pComboBoxUI, nIndex, nColum);
				}

				//移动位置
				m_pComboBoxUI->SetPos (*lpRCColum);
				m_pComboBoxUI->SetVisible (TRUE);
			} else {
				if (m_pEditUI) {
					m_pEditUI->SetPos (rc);
					m_pEditUI->SetVisible (false);
				}
				if (m_pComboBoxUI) {
					m_pComboBoxUI->SetPos (rc);
				}
			}
		}
	}
	void CListExUI::OnListItemChecked (int nIndex, int nColum, BOOL bChecked) {
		CControlUI* p = m_pHeader->GetItemAt (nColum);
		CListContainerHeaderItemUI* pHItem = static_cast<CListContainerHeaderItemUI*>(p->GetInterface (_T ("ListContainerHeaderItem")));
		if (!pHItem)
			return;

		//如果选中，那么检查是否全部都处于选中状态
		if (bChecked) {
			BOOL bCheckAll = TRUE;
			for (int i = 0; i < GetCount (); i++) {
				p = GetItemAt (i);
				CListTextExtElementUI* pLItem = static_cast<CListTextExtElementUI*>(p->GetInterface (_T ("ListTextExElement")));
				if (pLItem && !pLItem->GetCheck ()) {
					bCheckAll = FALSE;
					break;
				}
			}
			if (bCheckAll) {
				pHItem->SetCheck (TRUE);
			} else {
				pHItem->SetCheck (FALSE);
			}
		} else {
			pHItem->SetCheck (FALSE);
		}
	}
	void CListExUI::DoEvent (TEventUI& event) {
		if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_SCROLLWHEEL) {
			HideEditAndComboCtrl ();
		}

		CListUI::DoEvent (event);
	}
	void CListExUI::SetColumItemColor (int nIndex, int nColum, DWORD iBKColor) {
		CControlUI* p = GetItemAt (nIndex);
		CListTextExtElementUI* pLItem = static_cast<CListTextExtElementUI*>(p->GetInterface (_T ("ListTextExElement")));
		if (pLItem) {
			DWORD iTextBkColor = iBKColor;
			pLItem->SetColumItemColor (nColum, iTextBkColor);
		}
	}

	BOOL CListExUI::GetColumItemColor (int nIndex, int nColum, DWORD& iBKColor) {
		CControlUI* p = GetItemAt (nIndex);
		CListTextExtElementUI* pLItem = static_cast<CListTextExtElementUI*>(p->GetInterface (_T ("ListTextExElement")));
		if (!pLItem)
			return FALSE;
		pLItem->GetColumItemColor (nColum, iBKColor);
		return TRUE;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL (CListContainerHeaderItemUI)

		CListContainerHeaderItemUI::CListContainerHeaderItemUI (): m_bDragable (TRUE), m_uButtonState (0), m_iSepWidth (4),
		m_uTextStyle (DT_VCENTER | DT_CENTER | DT_SINGLELINE), m_dwTextColor (0), m_iFont (-1), m_bShowHtml (FALSE),
		m_bEditable (FALSE), m_bComboable (FALSE), m_bCheckBoxable (FALSE), m_uCheckBoxState (0), m_bChecked (FALSE), m_pOwner (nullptr) {
		SetTextPadding ({ 2, 0, 2, 0 });
		ptLastMouse.x = ptLastMouse.y = 0;
		SetMinWidth (16);
	}

	string_view_t CListContainerHeaderItemUI::GetClass () const {
		return _T ("ListContainerHeaderItem");
	}

	LPVOID CListContainerHeaderItemUI::GetInterface (string_view_t pstrName) {
		if (pstrName == _T ("ListContainerHeaderItem")) return this;
		return CContainerUI::GetInterface (pstrName);
	}

	UINT CListContainerHeaderItemUI::GetControlFlags () const {
		if (IsEnabled () && m_iSepWidth != 0) return UIFLAG_SETCURSOR;
		else return 0;
	}

	void CListContainerHeaderItemUI::SetEnabled (BOOL bEnable) {
		CContainerUI::SetEnabled (bEnable);
		if (!IsEnabled ()) {
			m_uButtonState = 0;
		}
	}

	BOOL CListContainerHeaderItemUI::IsDragable () const {
		return m_bDragable;
	}

	void CListContainerHeaderItemUI::SetDragable (BOOL bDragable) {
		m_bDragable = bDragable;
		if (!m_bDragable) m_uButtonState &= ~UISTATE_CAPTURED;
	}

	DWORD CListContainerHeaderItemUI::GetSepWidth () const {
		return m_iSepWidth;
	}

	void CListContainerHeaderItemUI::SetSepWidth (int iWidth) {
		m_iSepWidth = iWidth;
	}

	DWORD CListContainerHeaderItemUI::GetTextStyle () const {
		return m_uTextStyle;
	}

	void CListContainerHeaderItemUI::SetTextStyle (UINT uStyle) {
		m_uTextStyle = uStyle;
		Invalidate ();
	}

	DWORD CListContainerHeaderItemUI::GetTextColor () const {
		return m_dwTextColor;
	}


	void CListContainerHeaderItemUI::SetTextColor (DWORD dwTextColor) {
		m_dwTextColor = dwTextColor;
	}

	RECT CListContainerHeaderItemUI::GetTextPadding () const {
		return m_rcTextPadding;
	}

	void CListContainerHeaderItemUI::SetTextPadding (RECT rc) {
		m_rcTextPadding = rc;
		Invalidate ();
	}

	void CListContainerHeaderItemUI::SetFont (int index) {
		m_iFont = index;
	}

	BOOL CListContainerHeaderItemUI::IsShowHtml () {
		return m_bShowHtml;
	}

	void CListContainerHeaderItemUI::SetShowHtml (BOOL bShowHtml) {
		if (m_bShowHtml == bShowHtml) return;

		m_bShowHtml = bShowHtml;
		Invalidate ();
	}

	string_view_t CListContainerHeaderItemUI::GetNormalImage () const {
		return m_sNormalImage;
	}

	void CListContainerHeaderItemUI::SetNormalImage (string_view_t pStrImage) {
		m_sNormalImage = pStrImage;
		Invalidate ();
	}

	string_view_t CListContainerHeaderItemUI::GetHotImage () const {
		return m_sHotImage;
	}

	void CListContainerHeaderItemUI::SetHotImage (string_view_t pStrImage) {
		m_sHotImage = pStrImage;
		Invalidate ();
	}

	string_view_t CListContainerHeaderItemUI::GetPushedImage () const {
		return m_sPushedImage;
	}

	void CListContainerHeaderItemUI::SetPushedImage (string_view_t pStrImage) {
		m_sPushedImage = pStrImage;
		Invalidate ();
	}

	string_view_t CListContainerHeaderItemUI::GetFocusedImage () const {
		return m_sFocusedImage;
	}

	void CListContainerHeaderItemUI::SetFocusedImage (string_view_t pStrImage) {
		m_sFocusedImage = pStrImage;
		Invalidate ();
	}

	string_view_t CListContainerHeaderItemUI::GetSepImage () const {
		return m_sSepImage;
	}

	void CListContainerHeaderItemUI::SetSepImage (string_view_t pStrImage) {
		m_sSepImage = pStrImage;
		Invalidate ();
	}

	void CListContainerHeaderItemUI::SetAttribute (string_view_t pstrName, string_view_t pstrValue) {
		if (pstrName == _T ("dragable")) SetDragable (FawTools::parse_bool (pstrValue));
		else if (pstrName == _T ("sepwidth")) SetSepWidth (FawTools::parse_dec (pstrValue));
		else if (pstrName == _T ("align")) {
			if (pstrValue.find (_T ("left")) != string_t::npos) {
				m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_uTextStyle |= DT_LEFT;
			}
			if (pstrValue.find (_T ("center")) != string_t::npos) {
				m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_uTextStyle |= DT_CENTER;
			}
			if (pstrValue.find (_T ("right")) != string_t::npos) {
				m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_uTextStyle |= DT_RIGHT;
			}
		} else if (pstrName == _T ("endellipsis")) {
			if (FawTools::parse_bool (pstrValue)) m_uTextStyle |= DT_END_ELLIPSIS;
			else m_uTextStyle &= ~DT_END_ELLIPSIS;
		} else if (pstrName == _T ("font")) SetFont (FawTools::parse_dec (pstrValue));
		else if (pstrName == _T ("textcolor")) {
			SetTextColor ((DWORD) FawTools::parse_hex (pstrValue));
		} else if (pstrName == _T ("textpadding")) {
			RECT rcTextPadding = FawTools::parse_rect (pstrValue);
			SetTextPadding (rcTextPadding);
		} else if (pstrName == _T ("showhtml")) SetShowHtml (FawTools::parse_bool (pstrValue));
		else if (pstrName == _T ("normalimage")) SetNormalImage (pstrValue);
		else if (pstrName == _T ("hotimage")) SetHotImage (pstrValue);
		else if (pstrName == _T ("pushedimage")) SetPushedImage (pstrValue);
		else if (pstrName == _T ("focusedimage")) SetFocusedImage (pstrValue);
		else if (pstrName == _T ("sepimage")) SetSepImage (pstrValue);

		else if (pstrName == _T ("editable")) SetColumeEditable (FawTools::parse_bool (pstrValue));
		else if (pstrName == _T ("comboable")) SetColumeComboable (FawTools::parse_bool (pstrValue));
		else if (pstrName == _T ("checkable")) SetColumeCheckable (FawTools::parse_bool (pstrValue));
		else if (pstrName == _T ("checkboxwidth")) SetCheckBoxWidth (FawTools::parse_dec (pstrValue));
		else if (pstrName == _T ("checkboxheight")) SetCheckBoxHeight (FawTools::parse_dec (pstrValue));
		else if (pstrName == _T ("checkboxnormalimage")) SetCheckBoxNormalImage (pstrValue);
		else if (pstrName == _T ("checkboxhotimage")) SetCheckBoxHotImage (pstrValue);
		else if (pstrName == _T ("checkboxpushedimage")) SetCheckBoxPushedImage (pstrValue);
		else if (pstrName == _T ("checkboxfocusedimage")) SetCheckBoxFocusedImage (pstrValue);
		else if (pstrName == _T ("checkboxdisabledimage")) SetCheckBoxDisabledImage (pstrValue);
		else if (pstrName == _T ("checkboxselectedimage")) SetCheckBoxSelectedImage (pstrValue);
		else if (pstrName == _T ("checkboxforeimage")) SetCheckBoxForeImage (pstrValue);

		else CContainerUI::SetAttribute (pstrName, pstrValue);
	}

	void CListContainerHeaderItemUI::DoEvent (TEventUI& event) {
		if (!IsMouseEnabled () && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pParent) m_pParent->DoEvent (event);
			else CContainerUI::DoEvent (event);
			return;
		}

		//CheckBoxAble
		if (m_bCheckBoxable) {
			RECT rcCheckBox = { 0 };
			GetCheckBoxRect (rcCheckBox);

			if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK) {
				if (::PtInRect (&rcCheckBox, event.ptMouse)) {
					m_uCheckBoxState |= UISTATE_PUSHED | UISTATE_CAPTURED;
					Invalidate ();
				}
			} else if (event.Type == UIEVENT_MOUSEMOVE) {
				if ((m_uCheckBoxState & UISTATE_CAPTURED) != 0) {
					if (::PtInRect (&rcCheckBox, event.ptMouse))
						m_uCheckBoxState |= UISTATE_PUSHED;
					else
						m_uCheckBoxState &= ~UISTATE_PUSHED;
					Invalidate ();
				} else if (::PtInRect (&rcCheckBox, event.ptMouse)) {
					m_uCheckBoxState |= UISTATE_HOT;
					Invalidate ();
				} else {
					m_uCheckBoxState &= ~UISTATE_HOT;
					Invalidate ();
				}
			} else if (event.Type == UIEVENT_BUTTONUP) {
				if ((m_uCheckBoxState & UISTATE_CAPTURED) != 0) {
					if (::PtInRect (&rcCheckBox, event.ptMouse)) {
						SetCheck (!GetCheck ());
						CContainerUI* pOwner = (CContainerUI*) m_pParent;
						if (pOwner) {
							m_pManager->SendNotify (this, DUI_MSGTYPE_LISTHEADITEMCHECKED, pOwner->GetItemIndex (this), m_bChecked);
						}

					}
					m_uCheckBoxState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
					Invalidate ();
				} else if (::PtInRect (&rcCheckBox, event.ptMouse)) {

				}
			} else if (event.Type == UIEVENT_MOUSEENTER) {
				if (::PtInRect (&rcCheckBox, event.ptMouse)) {
					m_uCheckBoxState |= UISTATE_HOT;
					Invalidate ();
				}
			} else if (event.Type == UIEVENT_MOUSELEAVE) {
				m_uCheckBoxState &= ~UISTATE_HOT;
				Invalidate ();
			}
		}

		if (event.Type == UIEVENT_SETFOCUS) {
			Invalidate ();
		}
		if (event.Type == UIEVENT_KILLFOCUS) {
			Invalidate ();
		}
		if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK) {
			if (!IsEnabled ()) return;
			RECT rcSeparator = GetThumbRect ();
			if (m_iSepWidth >= 0)
				rcSeparator.left -= 4;
			else
				rcSeparator.right += 4;
			if (::PtInRect (&rcSeparator, event.ptMouse)) {
				if (m_bDragable) {
					m_uButtonState |= UISTATE_CAPTURED;
					ptLastMouse = event.ptMouse;
				}
			} else {
				m_uButtonState |= UISTATE_PUSHED;
				m_pManager->SendNotify (this, DUI_MSGTYPE_LISTHEADERCLICK);
				Invalidate ();
			}
			return;
		}
		if (event.Type == UIEVENT_BUTTONUP) {
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
				m_uButtonState &= ~UISTATE_CAPTURED;
				if (GetParent ())
					GetParent ()->NeedParentUpdate ();
			} else if ((m_uButtonState & UISTATE_PUSHED) != 0) {
				m_uButtonState &= ~UISTATE_PUSHED;
				Invalidate ();
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSEMOVE) {
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
				RECT rc = m_rcItem;
				if (m_iSepWidth >= 0) {
					rc.right -= ptLastMouse.x - event.ptMouse.x;
				} else {
					rc.left -= ptLastMouse.x - event.ptMouse.x;
				}

				if (rc.right - rc.left > GetMinWidth ()) {
					m_cxyFixed.cx = rc.right - rc.left;
					ptLastMouse = event.ptMouse;
					if (GetParent ())
						GetParent ()->NeedParentUpdate ();
				}
			}
			return;
		}
		if (event.Type == UIEVENT_SETCURSOR) {
			RECT rcSeparator = GetThumbRect ();
			if (m_iSepWidth >= 0)
				rcSeparator.left -= 4;
			else
				rcSeparator.right += 4;
			if (IsEnabled () && m_bDragable && ::PtInRect (&rcSeparator, event.ptMouse)) {
				::SetCursor (::LoadCursor (nullptr, IDC_SIZEWE));
				return;
			}
		}
		if (event.Type == UIEVENT_MOUSEENTER) {
			if (IsEnabled ()) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate ();
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSELEAVE) {
			if (IsEnabled ()) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate ();
			}
			return;
		}
		CContainerUI::DoEvent (event);
	}

	SIZE CListContainerHeaderItemUI::EstimateSize (SIZE szAvailable) {
		if (m_cxyFixed.cy == 0) return { m_cxyFixed.cx, m_pManager->GetDefaultFontInfo ()->tm.tmHeight + 14 };
		return CContainerUI::EstimateSize (szAvailable);
	}

	RECT CListContainerHeaderItemUI::GetThumbRect () const {
		if (m_iSepWidth >= 0) return { m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom };
		else return { m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom };
	}

	void CListContainerHeaderItemUI::PaintStatusImage (HDC hDC) {
		//HeadItem Bkgnd
		if (IsFocused ()) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~UISTATE_FOCUSED;

		if ((m_uButtonState & UISTATE_PUSHED) != 0) {
			if (m_sPushedImage.empty () && !m_sNormalImage.empty ()) DrawImage (hDC, m_sNormalImage);
			if (!DrawImage (hDC, m_sPushedImage)) {
			}
		} else if ((m_uButtonState & UISTATE_HOT) != 0) {
			if (m_sHotImage.empty () && !m_sNormalImage.empty ()) DrawImage (hDC, m_sNormalImage);
			if (!DrawImage (hDC, m_sHotImage)) {
			}
		} else if ((m_uButtonState & UISTATE_FOCUSED) != 0) {
			if (m_sFocusedImage.empty () && !m_sNormalImage.empty ()) DrawImage (hDC, m_sNormalImage);
			if (!DrawImage (hDC, m_sFocusedImage)) {
			}
		} else {
			if (!m_sNormalImage.empty ()) {
				if (!DrawImage (hDC, m_sNormalImage)) {
				}
			}
		}

		if (!m_sSepImage.empty ()) {
			RECT rcThumb = GetThumbRect ();
			rcThumb.left -= m_rcItem.left;
			rcThumb.top -= m_rcItem.top;
			rcThumb.right -= m_rcItem.left;
			rcThumb.bottom -= m_rcItem.top;

			m_sSepImageModify.clear ();
			m_sSepImageModify.Format (_T ("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
			if (!DrawImage (hDC, m_sSepImage, m_sSepImageModify)) {
			}
		}

		if (m_bCheckBoxable) {
			m_uCheckBoxState &= ~UISTATE_PUSHED;

			if ((m_uCheckBoxState & UISTATE_SELECTED) != 0) {
				if (!m_sCheckBoxSelectedImage.empty ()) {
					if (!DrawCheckBoxImage (hDC, m_sCheckBoxSelectedImage)) {
					} else goto Label_ForeImage;
				}
			}

			if (IsFocused ()) m_uCheckBoxState |= UISTATE_FOCUSED;
			else m_uCheckBoxState &= ~UISTATE_FOCUSED;
			if (!IsEnabled ()) m_uCheckBoxState |= UISTATE_DISABLED;
			else m_uCheckBoxState &= ~UISTATE_DISABLED;

			if ((m_uCheckBoxState & UISTATE_DISABLED) != 0) {
				if (!m_sCheckBoxDisabledImage.empty ()) {
					if (!DrawCheckBoxImage (hDC, m_sCheckBoxDisabledImage)) {
					} else return;
				}
			} else if ((m_uCheckBoxState & UISTATE_PUSHED) != 0) {
				if (!m_sCheckBoxPushedImage.empty ()) {
					if (!DrawCheckBoxImage (hDC, m_sCheckBoxPushedImage)) {
					} else return;
				}
			} else if ((m_uCheckBoxState & UISTATE_HOT) != 0) {
				if (!m_sCheckBoxHotImage.empty ()) {
					if (!DrawCheckBoxImage (hDC, m_sCheckBoxHotImage)) {
					} else return;
				}
			} else if ((m_uCheckBoxState & UISTATE_FOCUSED) != 0) {
				if (!m_sCheckBoxFocusedImage.empty ()) {
					if (!DrawCheckBoxImage (hDC, m_sCheckBoxFocusedImage)) {
					} else return;
				}
			}

			if (!m_sCheckBoxNormalImage.empty ()) {
				if (!DrawCheckBoxImage (hDC, m_sCheckBoxNormalImage)) {
				} else return;
			}

		Label_ForeImage:
			if (!m_sCheckBoxForeImage.empty ()) {
				if (!DrawCheckBoxImage (hDC, m_sCheckBoxForeImage)) {
				}
			}
		}
	}

	void CListContainerHeaderItemUI::PaintText (HDC hDC) {
		if (m_dwTextColor == 0) m_dwTextColor = m_pManager->GetDefaultFontColor ();

		RECT rcText = m_rcItem;
		rcText.left += m_rcTextPadding.left;
		rcText.top += m_rcTextPadding.top;
		rcText.right -= m_rcTextPadding.right;
		rcText.bottom -= m_rcTextPadding.bottom;
		if (m_bCheckBoxable) {
			RECT rcCheck = { 0 };
			GetCheckBoxRect (rcCheck);
			rcText.left += (rcCheck.right - rcCheck.left);
		}

		CDuiString sText = GetText ();
		if (sText.empty ()) return;

		int nLinks = 0;
		if (m_bShowHtml)
			CRenderEngine::DrawHtmlText (hDC, m_pManager, rcText, sText, m_dwTextColor, \
				nullptr, nullptr, nLinks, m_iFont, DT_SINGLELINE | m_uTextStyle);
		else
			CRenderEngine::DrawText (hDC, m_pManager, rcText, sText, m_dwTextColor, \
				m_iFont, DT_SINGLELINE | m_uTextStyle);
	}

	BOOL CListContainerHeaderItemUI::GetColumeEditable () {
		return m_bEditable;
	}

	void CListContainerHeaderItemUI::SetColumeEditable (BOOL bEnable) {
		m_bEditable = bEnable;
	}

	BOOL CListContainerHeaderItemUI::GetColumeComboable () {
		return m_bComboable;
	}

	void CListContainerHeaderItemUI::SetColumeComboable (BOOL bEnable) {
		m_bComboable = bEnable;
	}

	BOOL CListContainerHeaderItemUI::GetColumeCheckable () {
		return m_bCheckBoxable;
	}
	void CListContainerHeaderItemUI::SetColumeCheckable (BOOL bEnable) {
		m_bCheckBoxable = bEnable;
	}
	void CListContainerHeaderItemUI::SetCheck (BOOL bCheck) {
		if (m_bChecked == bCheck) return;
		m_bChecked = bCheck;
		if (m_bChecked) m_uCheckBoxState |= UISTATE_SELECTED;
		else m_uCheckBoxState &= ~UISTATE_SELECTED;
		Invalidate ();
	}

	BOOL CListContainerHeaderItemUI::GetCheck () {
		return m_bChecked;
	}
	BOOL CListContainerHeaderItemUI::DrawCheckBoxImage (HDC hDC, string_view_t pStrImage, string_view_t pStrModify) {
		RECT rcCheckBox = { 0 };
		GetCheckBoxRect (rcCheckBox);
		return CRenderEngine::DrawImageString (hDC, m_pManager, rcCheckBox, m_rcPaint, pStrImage, pStrModify);
	}
	string_view_t CListContainerHeaderItemUI::GetCheckBoxNormalImage () {
		return m_sCheckBoxNormalImage;
	}

	void CListContainerHeaderItemUI::SetCheckBoxNormalImage (string_view_t pStrImage) {
		m_sCheckBoxNormalImage = pStrImage;
	}

	string_view_t CListContainerHeaderItemUI::GetCheckBoxHotImage () {
		return m_sCheckBoxHotImage;
	}

	void CListContainerHeaderItemUI::SetCheckBoxHotImage (string_view_t pStrImage) {
		m_sCheckBoxHotImage = pStrImage;
	}

	string_view_t CListContainerHeaderItemUI::GetCheckBoxPushedImage () {
		return m_sCheckBoxPushedImage;
	}

	void CListContainerHeaderItemUI::SetCheckBoxPushedImage (string_view_t pStrImage) {
		m_sCheckBoxPushedImage = pStrImage;
	}

	string_view_t CListContainerHeaderItemUI::GetCheckBoxFocusedImage () {
		return m_sCheckBoxFocusedImage;
	}

	void CListContainerHeaderItemUI::SetCheckBoxFocusedImage (string_view_t pStrImage) {
		m_sCheckBoxFocusedImage = pStrImage;
	}

	string_view_t CListContainerHeaderItemUI::GetCheckBoxDisabledImage () {
		return m_sCheckBoxDisabledImage;
	}

	void CListContainerHeaderItemUI::SetCheckBoxDisabledImage (string_view_t pStrImage) {
		m_sCheckBoxDisabledImage = pStrImage;
	}
	string_view_t CListContainerHeaderItemUI::GetCheckBoxSelectedImage () {
		return m_sCheckBoxSelectedImage;
	}

	void CListContainerHeaderItemUI::SetCheckBoxSelectedImage (string_view_t pStrImage) {
		m_sCheckBoxSelectedImage = pStrImage;
	}
	string_view_t CListContainerHeaderItemUI::GetCheckBoxForeImage () {
		return m_sCheckBoxForeImage;
	}

	void CListContainerHeaderItemUI::SetCheckBoxForeImage (string_view_t pStrImage) {
		m_sCheckBoxForeImage = pStrImage;
	}
	int CListContainerHeaderItemUI::GetCheckBoxWidth () const {
		return m_cxyCheckBox.cx;
	}

	void CListContainerHeaderItemUI::SetCheckBoxWidth (int cx) {
		if (cx < 0) return;
		m_cxyCheckBox.cx = cx;
	}

	int CListContainerHeaderItemUI::GetCheckBoxHeight ()  const {
		return m_cxyCheckBox.cy;
	}

	void CListContainerHeaderItemUI::SetCheckBoxHeight (int cy) {
		if (cy < 0) return;
		m_cxyCheckBox.cy = cy;
	}
	void CListContainerHeaderItemUI::GetCheckBoxRect (RECT &rc) {
		memset (&rc, 0x00, sizeof (rc));
		int nItemHeight = m_rcItem.bottom - m_rcItem.top;
		rc.left = m_rcItem.left + 6;
		rc.top = m_rcItem.top + (nItemHeight - GetCheckBoxHeight ()) / 2;
		rc.right = rc.left + GetCheckBoxWidth ();
		rc.bottom = rc.top + GetCheckBoxHeight ();
	}

	void CListContainerHeaderItemUI::SetOwner (CContainerUI* pOwner) {
		m_pOwner = pOwner;
	}
	CContainerUI* CListContainerHeaderItemUI::GetOwner () {
		return m_pOwner;
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL (CListTextExtElementUI)

		CListTextExtElementUI::CListTextExtElementUI ():
		m_nLinks (0), m_nHoverLink (-1), m_pOwner (nullptr), m_uCheckBoxState (0), m_bChecked (FALSE) {
		::ZeroMemory (&m_rcLinks, sizeof (m_rcLinks));
		m_cxyCheckBox.cx = m_cxyCheckBox.cy = 0;

		::ZeroMemory (&ColumCorlorArray, sizeof (ColumCorlorArray));
	}

	CListTextExtElementUI::~CListTextExtElementUI () {
		CDuiString* pText;
		for (int it = 0; it < m_aTexts.GetSize (); it++) {
			pText = static_cast<CDuiString*>(m_aTexts[it]);
			if (pText) delete pText;
		}
		m_aTexts.Empty ();
	}

	string_view_t CListTextExtElementUI::GetClass () const {
		return _T ("ListTextExElementUI");
	}

	LPVOID CListTextExtElementUI::GetInterface (string_view_t pstrName) {
		if (pstrName == _T ("ListTextExElement")) return static_cast<CListTextExtElementUI*>(this);
		return CListLabelElementUI::GetInterface (pstrName);
	}

	UINT CListTextExtElementUI::GetControlFlags () const {
		return UIFLAG_WANTRETURN | ((IsEnabled () && m_nLinks > 0) ? UIFLAG_SETCURSOR : 0);
	}

	CDuiString CListTextExtElementUI::GetText (int iIndex) const {
		CDuiString* pText = static_cast<CDuiString*>(m_aTexts.GetAt (iIndex));
		if (pText) return *pText;
		return nullptr;
	}

	void CListTextExtElementUI::SetText (int iIndex, string_view_t pstrText) {
		if (!m_pOwner) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo ();
		if (iIndex < 0 || iIndex >= pInfo->nColumns) return;
		while (m_aTexts.GetSize () < pInfo->nColumns) {
			m_aTexts.Add (nullptr);
		}

		CDuiString* pText = static_cast<CDuiString*>(m_aTexts[iIndex]);
		if ((!pText && pstrText.empty ()) || (pText && *pText == pstrText)) return;

		if (pText)
			*pText = pstrText;
		else
			m_aTexts.SetAt (iIndex, new CDuiString (pstrText));
		Invalidate ();
	}

	void CListTextExtElementUI::SetOwner (CControlUI* pOwner) {
		CListElementUI::SetOwner (pOwner);
		m_pOwner = static_cast<CListUI*>(pOwner->GetInterface (_T ("List")));
	}

	CDuiString* CListTextExtElementUI::GetLinkContent (int iIndex) {
		if (iIndex >= 0 && iIndex < m_nLinks) return &m_sLinks[iIndex];
		return nullptr;
	}

	void CListTextExtElementUI::DoEvent (TEventUI& event) {
		if (!IsMouseEnabled () && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pOwner) m_pOwner->DoEvent (event);
			else CListLabelElementUI::DoEvent (event);
			return;
		}

		// When you hover over a link
		if (event.Type == UIEVENT_SETCURSOR) {
			for (int i = 0; i < m_nLinks; i++) {
				if (::PtInRect (&m_rcLinks[i], event.ptMouse)) {
					::SetCursor (::LoadCursor (nullptr, IDC_HAND));
					return;
				}
			}
		}
		if (event.Type == UIEVENT_BUTTONUP && IsEnabled ()) {
			for (int i = 0; i < m_nLinks; i++) {
				if (::PtInRect (&m_rcLinks[i], event.ptMouse)) {
					m_pManager->SendNotify (this, DUI_MSGTYPE_LINK, i);
					return;
				}
			}
		}
		if (m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE) {
			int nHoverLink = -1;
			for (int i = 0; i < m_nLinks; i++) {
				if (::PtInRect (&m_rcLinks[i], event.ptMouse)) {
					nHoverLink = i;
					break;
				}
			}

			if (m_nHoverLink != nHoverLink) {
				Invalidate ();
				m_nHoverLink = nHoverLink;
			}
		}
		if (m_nLinks > 0 && event.Type == UIEVENT_MOUSELEAVE) {
			if (m_nHoverLink != -1) {
				Invalidate ();
				m_nHoverLink = -1;
			}
		}

		//检查是否需要显示编辑框或者组合框	
		CListExUI * pListCtrl = (CListExUI *) m_pOwner;
		int nColum = HitTestColum (event.ptMouse);
		if (event.Type == UIEVENT_BUTTONUP && m_pOwner->IsFocused ()) {
			RECT rc = { 0, 0, 0, 0 };
			if (nColum >= 0) {
				GetColumRect (nColum, rc);
			}

			pListCtrl->OnListItemClicked (GetIndex (), nColum, &rc, GetText (nColum));
		}

		//检查是否需要显示CheckBox
		TListInfoUI* pInfo = m_pOwner->GetListInfo ();
		for (int i = 0; i < pInfo->nColumns; i++) {
			if (pListCtrl->CheckColumCheckBoxable (i)) {
				RECT rcCheckBox = { 0 };
				GetCheckBoxRect (i, rcCheckBox);

				if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK) {
					if (::PtInRect (&rcCheckBox, event.ptMouse)) {
						m_uCheckBoxState |= UISTATE_PUSHED | UISTATE_CAPTURED;
						Invalidate ();
					}
				} else if (event.Type == UIEVENT_MOUSEMOVE) {
					if ((m_uCheckBoxState & UISTATE_CAPTURED) != 0) {
						if (::PtInRect (&rcCheckBox, event.ptMouse))
							m_uCheckBoxState |= UISTATE_PUSHED;
						else
							m_uCheckBoxState &= ~UISTATE_PUSHED;
						Invalidate ();
					}
				} else if (event.Type == UIEVENT_BUTTONUP) {
					if ((m_uCheckBoxState & UISTATE_CAPTURED) != 0) {
						if (::PtInRect (&rcCheckBox, event.ptMouse)) {
							SetCheck (!GetCheck ());
							if (m_pManager) {
								m_pManager->SendNotify (this, DUI_MSGTYPE_LISTITEMCHECKED, MAKEWPARAM (GetIndex (), 0), m_bChecked);
							}
						}
						m_uCheckBoxState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
						Invalidate ();
					}
				} else if (event.Type == UIEVENT_MOUSEENTER) {
					if (::PtInRect (&rcCheckBox, event.ptMouse)) {
						m_uCheckBoxState |= UISTATE_HOT;
						Invalidate ();
					}
				} else if (event.Type == UIEVENT_MOUSELEAVE) {
					m_uCheckBoxState &= ~UISTATE_HOT;
					Invalidate ();
				}
			}
		}

		CListLabelElementUI::DoEvent (event);
	}

	SIZE CListTextExtElementUI::EstimateSize (SIZE szAvailable) {
		TListInfoUI* pInfo = nullptr;
		if (m_pOwner) pInfo = m_pOwner->GetListInfo ();

		SIZE cXY = m_cxyFixed;
		if (cXY.cy == 0 && m_pManager && pInfo) {
			cXY.cy = m_pManager->GetFontInfo (pInfo->nFont)->tm.tmHeight + 8;
			cXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
		}

		return cXY;
	}

	void CListTextExtElementUI::DrawItemText (HDC hDC, const RECT& /*rcItem*/) {
		if (!m_pOwner) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo ();
		DWORD iTextColor = pInfo->dwTextColor;

		if ((m_uButtonState & UISTATE_HOT) != 0) {
			iTextColor = pInfo->dwHotTextColor;
		}
		if (IsSelected ()) {
			iTextColor = pInfo->dwSelectedTextColor;
		}
		if (!IsEnabled ()) {
			iTextColor = pInfo->dwDisabledTextColor;
		}
		IListCallbackUI* pCallback = m_pOwner->GetTextCallback ();
		//DUIASSERT(pCallback);
		//if (!pCallback) return;

		CListExUI * pListCtrl = (CListExUI *) m_pOwner;
		m_nLinks = 0;
		int nLinks = lengthof (m_rcLinks);
		for (int i = 0; i < pInfo->nColumns; i++) {
			RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };

			DWORD iTextBkColor = 0;
			if (GetColumItemColor (i, iTextBkColor)) {
				CRenderEngine::DrawColor (hDC, rcItem, iTextBkColor);
			}

			rcItem.left += pInfo->rcTextPadding.left;
			rcItem.right -= pInfo->rcTextPadding.right;
			rcItem.top += pInfo->rcTextPadding.top;
			rcItem.bottom -= pInfo->rcTextPadding.bottom;

			//检查是否需要显示CheckBox
			if (pListCtrl->CheckColumCheckBoxable (i)) {
				RECT rcCheckBox = { 0 };
				GetCheckBoxRect (i, rcCheckBox);
				rcItem.left += (rcCheckBox.right - rcCheckBox.left);
			}

			CDuiString strText;//不使用LPstring_view_t则限制太多 by cddjr 2011/10/20
			if (pCallback)
				strText = pCallback->GetItemText (this, m_iIndex, i);
			else
				strText = GetText (i);
			if (pInfo->bShowHtml)
				CRenderEngine::DrawHtmlText (hDC, m_pManager, rcItem, strText, iTextColor, \
					&m_rcLinks[m_nLinks], &m_sLinks[m_nLinks], nLinks, pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
			else
				CRenderEngine::DrawText (hDC, m_pManager, rcItem, strText, iTextColor, \
					pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);

			m_nLinks += nLinks;
			nLinks = lengthof (m_rcLinks) - m_nLinks;
		}
		for (int i = m_nLinks; i < lengthof (m_rcLinks); i++) {
			::ZeroMemory (m_rcLinks + i, sizeof (RECT));
			((CDuiString*) (m_sLinks + i))->clear ();
		}
	}
	void CListTextExtElementUI::PaintStatusImage (HDC hDC) {
		CListExUI * pListCtrl = (CListExUI *) m_pOwner;
		TListInfoUI* pInfo = m_pOwner->GetListInfo ();
		for (int i = 0; i < pInfo->nColumns; i++) {
			if (pListCtrl->CheckColumCheckBoxable (i)) {
				RECT rcCheckBox = { 0 };
				GetCheckBoxRect (i, rcCheckBox);

				m_uCheckBoxState &= ~UISTATE_PUSHED;

				if ((m_uCheckBoxState & UISTATE_SELECTED) != 0) {
					if (!m_sCheckBoxSelectedImage.empty ()) {
						if (!DrawCheckBoxImage (hDC, m_sCheckBoxSelectedImage, nullptr, rcCheckBox)) {
						} else goto Label_ForeImage;
					}
				}

				if (IsFocused ()) m_uCheckBoxState |= UISTATE_FOCUSED;
				else m_uCheckBoxState &= ~UISTATE_FOCUSED;
				if (!IsEnabled ()) m_uCheckBoxState |= UISTATE_DISABLED;
				else m_uCheckBoxState &= ~UISTATE_DISABLED;

				if ((m_uCheckBoxState & UISTATE_DISABLED) != 0) {
					if (!m_sCheckBoxDisabledImage.empty ()) {
						if (!DrawCheckBoxImage (hDC, m_sCheckBoxDisabledImage, nullptr, rcCheckBox)) {
						} else return;
					}
				} else if ((m_uCheckBoxState & UISTATE_PUSHED) != 0) {
					if (!m_sCheckBoxPushedImage.empty ()) {
						if (!DrawCheckBoxImage (hDC, m_sCheckBoxPushedImage, nullptr, rcCheckBox)) {
						} else return;
					}
				} else if ((m_uCheckBoxState & UISTATE_HOT) != 0) {
					if (!m_sCheckBoxHotImage.empty ()) {
						if (!DrawCheckBoxImage (hDC, m_sCheckBoxHotImage, nullptr, rcCheckBox)) {
						} else return;
					}
				} else if ((m_uCheckBoxState & UISTATE_FOCUSED) != 0) {
					if (!m_sCheckBoxFocusedImage.empty ()) {
						if (!DrawCheckBoxImage (hDC, m_sCheckBoxFocusedImage, nullptr, rcCheckBox)) {
						} else return;
					}
				}

				if (!m_sCheckBoxNormalImage.empty ()) {
					if (!DrawCheckBoxImage (hDC, m_sCheckBoxNormalImage, nullptr, rcCheckBox)) {
					} else return;
				}

			Label_ForeImage:
				if (!m_sCheckBoxForeImage.empty ()) {
					if (!DrawCheckBoxImage (hDC, m_sCheckBoxForeImage, nullptr, rcCheckBox)) {
					}
				}
			}
		}
	}
	BOOL CListTextExtElementUI::DrawCheckBoxImage (HDC hDC, string_view_t pStrImage, string_view_t pStrModify, RECT& rcCheckBox) {
		return CRenderEngine::DrawImageString (hDC, m_pManager, rcCheckBox, m_rcPaint, pStrImage, pStrModify);
	}
	void CListTextExtElementUI::SetAttribute (string_view_t pstrName, string_view_t pstrValue) {
		if (pstrName == _T ("checkboxwidth")) SetCheckBoxWidth (FawTools::parse_dec (pstrValue));
		else if (pstrName == _T ("checkboxheight")) SetCheckBoxHeight (FawTools::parse_dec (pstrValue));
		else if (pstrName == _T ("checkboxnormalimage")) SetCheckBoxNormalImage (pstrValue);
		else if (pstrName == _T ("checkboxhotimage")) SetCheckBoxHotImage (pstrValue);
		else if (pstrName == _T ("checkboxpushedimage")) SetCheckBoxPushedImage (pstrValue);
		else if (pstrName == _T ("checkboxfocusedimage")) SetCheckBoxFocusedImage (pstrValue);
		else if (pstrName == _T ("checkboxdisabledimage")) SetCheckBoxDisabledImage (pstrValue);
		else if (pstrName == _T ("checkboxselectedimage")) SetCheckBoxSelectedImage (pstrValue);
		else if (pstrName == _T ("checkboxforeimage")) SetCheckBoxForeImage (pstrValue);
		else CListLabelElementUI::SetAttribute (pstrName, pstrValue);
	}
	string_view_t CListTextExtElementUI::GetCheckBoxNormalImage () {
		return m_sCheckBoxNormalImage;
	}

	void CListTextExtElementUI::SetCheckBoxNormalImage (string_view_t pStrImage) {
		m_sCheckBoxNormalImage = pStrImage;
	}

	string_view_t CListTextExtElementUI::GetCheckBoxHotImage () {
		return m_sCheckBoxHotImage;
	}

	void CListTextExtElementUI::SetCheckBoxHotImage (string_view_t pStrImage) {
		m_sCheckBoxHotImage = pStrImage;
	}

	string_view_t CListTextExtElementUI::GetCheckBoxPushedImage () {
		return m_sCheckBoxPushedImage;
	}

	void CListTextExtElementUI::SetCheckBoxPushedImage (string_view_t pStrImage) {
		m_sCheckBoxPushedImage = pStrImage;
	}

	string_view_t CListTextExtElementUI::GetCheckBoxFocusedImage () {
		return m_sCheckBoxFocusedImage;
	}

	void CListTextExtElementUI::SetCheckBoxFocusedImage (string_view_t pStrImage) {
		m_sCheckBoxFocusedImage = pStrImage;
	}

	string_view_t CListTextExtElementUI::GetCheckBoxDisabledImage () {
		return m_sCheckBoxDisabledImage;
	}

	void CListTextExtElementUI::SetCheckBoxDisabledImage (string_view_t pStrImage) {
		m_sCheckBoxDisabledImage = pStrImage;
	}
	string_view_t CListTextExtElementUI::GetCheckBoxSelectedImage () {
		return m_sCheckBoxSelectedImage;
	}

	void CListTextExtElementUI::SetCheckBoxSelectedImage (string_view_t pStrImage) {
		m_sCheckBoxSelectedImage = pStrImage;
	}
	string_view_t CListTextExtElementUI::GetCheckBoxForeImage () {
		return m_sCheckBoxForeImage;
	}

	void CListTextExtElementUI::SetCheckBoxForeImage (string_view_t pStrImage) {
		m_sCheckBoxForeImage = pStrImage;
	}

	bool CListTextExtElementUI::DoPaint (HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
		if (!::IntersectRect (&m_rcPaint, &rcPaint, &m_rcItem)) return true;
		DrawItemBk (hDC, m_rcItem);
		PaintStatusImage (hDC);
		DrawItemText (hDC, m_rcItem);
		return true;
	}
	void CListTextExtElementUI::GetCheckBoxRect (int nIndex, RECT &rc) {
		memset (&rc, 0x00, sizeof (rc));
		int nItemHeight = m_rcItem.bottom - m_rcItem.top;
		rc.left = m_rcItem.left + 6;
		rc.top = m_rcItem.top + (nItemHeight - GetCheckBoxHeight ()) / 2;
		rc.right = rc.left + GetCheckBoxWidth ();
		rc.bottom = rc.top + GetCheckBoxHeight ();
	}
	int CListTextExtElementUI::GetCheckBoxWidth () const {
		return m_cxyCheckBox.cx;
	}

	void CListTextExtElementUI::SetCheckBoxWidth (int cx) {
		if (cx < 0) return;
		m_cxyCheckBox.cx = cx;
	}

	int CListTextExtElementUI::GetCheckBoxHeight ()  const {
		return m_cxyCheckBox.cy;
	}

	void CListTextExtElementUI::SetCheckBoxHeight (int cy) {
		if (cy < 0) return;
		m_cxyCheckBox.cy = cy;
	}

	void CListTextExtElementUI::SetCheck (BOOL bCheck) {
		if (m_bChecked == bCheck) return;
		m_bChecked = bCheck;
		if (m_bChecked) m_uCheckBoxState |= UISTATE_SELECTED;
		else m_uCheckBoxState &= ~UISTATE_SELECTED;
		Invalidate ();
	}

	BOOL  CListTextExtElementUI::GetCheck () const {
		return m_bChecked;
	}

	int CListTextExtElementUI::HitTestColum (POINT ptMouse) {
		TListInfoUI* pInfo = m_pOwner->GetListInfo ();
		for (int i = 0; i < pInfo->nColumns; i++) {
			RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };
			rcItem.left += pInfo->rcTextPadding.left;
			rcItem.right -= pInfo->rcTextPadding.right;
			rcItem.top += pInfo->rcTextPadding.top;
			rcItem.bottom -= pInfo->rcTextPadding.bottom;

			if (::PtInRect (&rcItem, ptMouse)) {
				return i;
			}
		}
		return -1;
	}

	BOOL CListTextExtElementUI::CheckColumEditable (int nColum) {
		return m_pOwner->CheckColumEditable (nColum);
	}
	void CListTextExtElementUI::GetColumRect (int nColum, RECT &rc) {
		TListInfoUI* pInfo = m_pOwner->GetListInfo ();
		RECT rcOwnerPos = m_pOwner->GetPos ();
		rc.left = pInfo->rcColumn[nColum].left + 1;
		rc.top = 1;
		rc.right = pInfo->rcColumn[nColum].right - 1;
		rc.bottom = m_rcItem.bottom - m_rcItem.top - 1;
		OffsetRect (&rc, -rcOwnerPos.left, m_rcItem.top - rcOwnerPos.top);
	}

	void CListTextExtElementUI::SetColumItemColor (int nColum, DWORD iBKColor) {
		ColumCorlorArray[nColum].bEnable = TRUE;
		ColumCorlorArray[nColum].iBKColor = iBKColor;
		Invalidate ();
	}
	BOOL CListTextExtElementUI::GetColumItemColor (int nColum, DWORD& iBKColor) {
		if (!ColumCorlorArray[nColum].bEnable) {
			return FALSE;
		}
		iBKColor = ColumCorlorArray[nColum].iBKColor;
		return TRUE;
	}

} // namespace DuiLib

