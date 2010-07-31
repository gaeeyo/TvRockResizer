// TvRock�̃L�[���[�h�����\������T�C�Y�ł���悤�ɂ���

#pragma once


class CKeywordReserveDlgResizer :
		public CWindowImplBase,
		public CDialogResize<CKeywordReserveDlgResizer>
{
public:
	// hWnd���u�L�[���[�h�����\��v�E�B���h�E���ǂ����̔��������
	static BOOL IsTargetWindow(HWND hWnd) {
		CWindow wnd(hWnd);
		CString	title;
		wnd.GetWindowText(title);
		
		if (_tcscmp(title, _T("�@�L�[���[�h�����\��")) == 0) {
			return TRUE;
		}
		return FALSE;
	}

	// ���T�C�Y�\�ɂ���
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

		// �폜���ꂽ���j���[�𕜊������āA���T�C�Y�ƍő剻���\�ɂ���
		CMenuHandle menu = GetSystemMenu(FALSE);
		menu.AppendMenu(MF_STRING, SC_SIZE, _T("�T�C�Y�ύX(&S)"));
		menu.AppendMenu(MF_STRING, SC_MAXIMIZE, _T("�ő剻(&X)"));

		// �E�B���h�E�X�^�C����ύX���ă��T�C�Y�ƍő剻���\�ɂ���
		ModifyStyleEx(WS_EX_TOOLWINDOW, 0);
		ModifyStyle(WS_THICKFRAME | WS_MAXIMIZEBOX, WS_THICKFRAME | WS_MAXIMIZEBOX);


		// �R���g���[���̃��C�A�E�g��ύX����ResizeMap������Ă���
		ChangeLayout();

		// ���T�C�Y�̏�����
		DlgResize_Init(false, true, 0);

		//// �E�B���h�E�X�^�C���̕ύX�𔽉f������
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

	// ���C�A�E�g�ύX����ResizeMap�����
	void ChangeLayout() {
		m_ResizeMap.RemoveAll();

		// �q�E�B���h�E�̃n���h�������ׂĎ擾
		CSimpleArray<HWND>	wnds;
		EnumChildWindows(m_hWnd, EnumChildWindowsProc, (LPARAM)&wnds);

		UINT	nextId = 0xD000;	// ����ɂ���CtrlID�̏����l

		// ��̈ʒu�����߂�
		// �u�S�`�F�b�N�v�{�^���� ctrlID:404 �̍�������ɂ���
		POINT	base = {0};
		{
			RECT	rc = {0};
			::GetWindowRect(GetDlgItem(0x404), &rc);
			ScreenToClient(&rc);
			base.x = rc.left;
			base.y = rc.bottom;
		}
		
		// �N���C�A���g�T�C�Y�擾
		CRect	cr;
		GetClientRect(&cr);

		// �q�E�B���h�E�𓮂���
		for (int j=0; j<wnds.GetSize(); j++) {
			if (::GetParent(wnds[j]) != m_hWnd) {	// �q�ȊO(��)�͖�������
				continue;
			}
			// CtrlID���擾���AID�������Ȃ�ID������ɕt����
			int id = ::GetDlgCtrlID(wnds[j]);

			if (id == 0 || id == 0xffffffff || id == 0x0000ffff) {	// �����̔���͗ǂ��Ȃ��Ǝv��
				::SetWindowLong(wnds[j], GWL_ID, nextId);
				id = nextId++;
			}

			// �E�B���h�E�̈ʒu�ɂ�� ResizeFlags ���C��������A�ʒu�����炵���肷��
			CRect	r;
			::GetWindowRect(wnds[j], &r);
			ScreenToClient(&r);

			if (r.top < base.y) {
				// �����̈ʒu�̃E�B���h�E�����ׂĉE�ɂ��炷
				::MoveWindow(wnds[j], r.left + base.x - 8, r.top, r.Width(), r.Height(), FALSE);
				if (r.top < 100) {	// ��������
					if (::GetWindowLong(wnds[j],GWL_STYLE) & WS_TABSTOP) {
						// �������ʈꗗ���͏c�ɐL�т�悤�ɂ���
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
				// ��̍���
				if (r.top < base.y + 100) {
					if (id == 0x457) {	// ���������\�񃊃X�g��ListView�͂��Ăɂ̂΂�
						::MoveWindow(wnds[j], r.left, r.top - base.y - 8, r.Width(), r.Height() + base.y + 8, FALSE);
						AddResizeMap(id, DLSZ_SIZE_X | DLSZ_SIZE_Y);
					}
					else {	// ����ȊO(���x��)
						::MoveWindow(wnds[j], r.left, r.top - base.y - 8, r.Width(), r.Height(), FALSE);
						AddResizeMap(id, 0);
					}
				}
				else {
					AddResizeMap(id, DLSZ_MOVE_Y);
				}
			}
			else {
				// ��̉E��
				AddResizeMap(id, DLSZ_MOVE_X | DLSZ_MOVE_Y);
			}
		}

		// ���炵���Ԃ�T�C�Y���g�傷��
		{
			CRect	r;
			GetWindowRect(&r);
			SetWindowPos(0, 0, 0, r.Width() + base.x, r.Height() + 8, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		}

		// ���T�C�Y�}�b�v�̏I���}�[�N������
		AddResizeMap(-1, 0);
	}

	// CDialogResize::GetDlgResizeMap �̃I�[�o�[���C�h
	// ResizeMap��Ԃ�
	_AtlDlgResizeMap* GetDlgResizeMap() {
		return m_ResizeMap.GetData();
	}


	void SaveWindowSize() {
		CRect	rect;
		GetWindowRect(&rect);

		CRegKey	reg;
		if (reg.Create(HKEY_CURRENT_USER, _T(TVROCK_RESIZER_KEY)) == ERROR_SUCCESS) {
			// �E�B���h�E�T�C�Y�ۑ�
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