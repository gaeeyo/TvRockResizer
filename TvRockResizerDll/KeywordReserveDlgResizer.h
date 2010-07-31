// TvRockのキーワード検索予約をリサイズできるようにする

#pragma once


class CKeywordReserveDlgResizer :
		public CWindowImplBase,
		public CDialogResize<CKeywordReserveDlgResizer>
{
public:
	// hWndが「キーワード検索予約」ウィンドウかどうかの判定をする
	static BOOL IsTargetWindow(HWND hWnd) {
		CWindow wnd(hWnd);
		CString	title;
		wnd.GetWindowText(title);
		
		if (_tcscmp(title, _T("　キーワード検索予約")) == 0) {
			return TRUE;
		}
		return FALSE;
	}

	// リサイズ可能にする
	static CKeywordReserveDlgResizer* Apply(HWND hWnd) {
		CKeywordReserveDlgResizer	*p = new CKeywordReserveDlgResizer();
		p->Init(hWnd);
		return p;
	}

private:

	CSimpleArray<_AtlDlgResizeMap>	m_ResizeMap;	// ResizeMap

	BEGIN_MSG_MAP_EX(CKeywordReserveDlgResizer)
		
		MESSAGE_HANDLER_EX(WM_NCHITTEST, DispatchDefault)
		MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP(CDialogResize<CKeywordReserveDlgResizer>)
	END_MSG_MAP()


	virtual void OnFinalMessage(HWND /*hWnd*/)	{
		delete this;
	}

	virtual ~CKeywordReserveDlgResizer() {
		//ATLTRACE(_T(__FUNCTION__));
	}

	void OnDestroy() {
		SaveWindowSize();
	}

	LRESULT DispatchDefault(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		SetMsgHandled(TRUE);
		return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}

	void Init(HWND hWnd) {
		SubclassWindow(hWnd);

		// 削除されたメニューを復活させて、リサイズと最大化を可能にする
		CMenuHandle menu = GetSystemMenu(FALSE);
		menu.AppendMenu(MF_STRING, SC_SIZE, _T("サイズ変更(&S)"));
		menu.AppendMenu(MF_STRING, SC_MAXIMIZE, _T("最大化(&X)"));

		// ウィンドウスタイルを変更してリサイズと最大化を可能にする
		ModifyStyleEx(WS_EX_TOOLWINDOW, 0);
		ModifyStyle(WS_THICKFRAME | WS_MAXIMIZEBOX, WS_THICKFRAME | WS_MAXIMIZEBOX);


		// コントロールのレイアウトを変更してResizeMapを作っておく
		ChangeLayout();

		// リサイズの初期化
		DlgResize_Init(false, true, 0);

		//// ウィンドウスタイルの変更を反映させる
		//SetWindowPos(0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE
		//		| SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);

		LoadWindowSize();
	}

	// EnumChildWindowsProc
	static BOOL CALLBACK EnumChildWindowsProc(HWND hWnd, LPARAM lParam) {
		CSimpleArray<HWND> *pArray = (CSimpleArray<HWND> *)lParam;
		pArray->Add(hWnd);
		return TRUE;
	}

	void AddResizeMap(UINT id, DWORD resizeFlags) {
		struct _AtlDlgResizeMap x;
		x.m_nCtlID		  = id;
		x.m_dwResizeFlags = resizeFlags;
		m_ResizeMap.Add(x);
	}

	// レイアウト変更しつつResizeMapを作る
	void ChangeLayout() {
		m_ResizeMap.RemoveAll();

		// 子ウィンドウのハンドルをすべて取得
		CSimpleArray<HWND>	wnds;
		EnumChildWindows(m_hWnd, EnumChildWindowsProc, (LPARAM)&wnds);

		UINT	nextId = 0xD000;	// 勝手につけるCtrlIDの初期値

		// 基準の位置を求める
		// 「全チェック」ボタンの ctrlID:404 の左下を基準にする
		POINT	base = {0};
		{
			RECT	rc = {0};
			::GetWindowRect(GetDlgItem(0x404), &rc);
			ScreenToClient(&rc);
			base.x = rc.left;
			base.y = rc.bottom;
		}
		
		// クライアントサイズ取得
		CRect	cr;
		GetClientRect(&cr);

		// 子ウィンドウを動かす
		for (int j=0; j<wnds.GetSize(); j++) {
			if (::GetParent(wnds[j]) != m_hWnd) {	// 子以外(孫)は無視する
				continue;
			}
			// CtrlIDを取得し、IDが無効ならIDを勝手に付ける
			int id = ::GetDlgCtrlID(wnds[j]);

			if (id == 0 || id == 0xffffffff || id == 0x0000ffff) {	// ←この判定は良くないと思う
				::SetWindowLong(wnds[j], GWL_ID, nextId);
				id = nextId++;
			}

			// ウィンドウの位置により ResizeFlags を修正したり、位置をずらしたりする
			CRect	r;
			::GetWindowRect(wnds[j], &r);
			ScreenToClient(&r);

			if (r.top < base.y) {
				// 基準より上の位置のウィンドウをすべて右にずらす
				::MoveWindow(wnds[j], r.left + base.x - 8, r.top, r.Width(), r.Height(), FALSE);
				if (r.top < 100) {	// いい加減
					if (::GetWindowLong(wnds[j],GWL_STYLE) & WS_TABSTOP) {
						// 検索結果一覧をは縦に伸びるようにする
						AddResizeMap(id, DLSZ_MOVE_X | DLSZ_SIZE_Y);
					}
					else {
						AddResizeMap(id, DLSZ_MOVE_X);
					}
				}
				else {
					AddResizeMap(id, DLSZ_MOVE_X | DLSZ_MOVE_Y);
				}
			}
			else if (r.left < base.x) {
				// 基準の左下
				if (r.top < base.y + 100) {
					if (id == 0x457) {	// 自動検索予約リストのListViewはたてにのばす
						::MoveWindow(wnds[j], r.left, r.top - base.y - 8, r.Width(), r.Height() + base.y + 8, FALSE);
						AddResizeMap(id, DLSZ_SIZE_X | DLSZ_SIZE_Y);
					}
					else {	// それ以外(ラベル)
						::MoveWindow(wnds[j], r.left, r.top - base.y - 8, r.Width(), r.Height(), FALSE);
						AddResizeMap(id, 0);
					}
				}
				else {
					AddResizeMap(id, DLSZ_MOVE_Y);
				}
			}
			else {
				// 基準の右下
				AddResizeMap(id, DLSZ_MOVE_X | DLSZ_MOVE_Y);
			}
		}

		// ずらしたぶんサイズを拡大する
		{
			CRect	r;
			GetWindowRect(&r);
			SetWindowPos(0, 0, 0, r.Width() + base.x, r.Height() + 8, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		}

		// リサイズマップの終了マークをつける
		AddResizeMap(-1, 0);
	}

	// CDialogResize::GetDlgResizeMap のオーバーライド
	// ResizeMapを返す
	_AtlDlgResizeMap* GetDlgResizeMap() {
		return m_ResizeMap.GetData();
	}


	void SaveWindowSize() {
		CRect	rect;
		GetWindowRect(&rect);

		CRegKey	reg;
		if (reg.Create(HKEY_CURRENT_USER, _T(TVROCK_RESIZER_KEY)) == ERROR_SUCCESS) {
			// ウィンドウサイズ保存
			//reg.SetDWORDValue(_T("KeywordReserveDlg_Width"), rect.Width());
			//reg.SetDWORDValue(_T("KeywordReserveDlg_Height"), rect.Height());
			WINDOWPLACEMENT	wp = {0};
			wp.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(&wp);
			reg.SetBinaryValue(_T("KeywordReserveDlg"), &wp, sizeof(WINDOWPLACEMENT));
		}
	}

	void LoadWindowSize() {
		CRegKey	reg;
		if (reg.Open(HKEY_CURRENT_USER, _T(TVROCK_RESIZER_KEY), KEY_READ) == ERROR_SUCCESS) {
			DWORD	width = 0;
			DWORD	height = 0;
			//if (reg.QueryDWORDValue(_T("KeywordReserveDlg_Width"), width) == ERROR_SUCCESS
			//	&& reg.QueryDWORDValue(_T("KeywordReserveDlg_Height"), height) == ERROR_SUCCESS) {
			//	SetWindowPos(0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
			//}
			WINDOWPLACEMENT wp = {0};
			DWORD size = sizeof(WINDOWPLACEMENT);
			reg.QueryBinaryValue(_T("KeywordReserveDlg"), &wp, &size);
			wp.length = sizeof(WINDOWPLACEMENT);
			wp.flags = WPF_RESTORETOMAXIMIZED;
			SetWindowPlacement(&wp);
		}
	}

};