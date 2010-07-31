#pragma once


extern HWND FindTvRockMainWindow();
extern HWND FindRockBar();

#define TVROCK_RESIZER_KEY "Software\\TvRock\\Resizer"

/* �߂�
 ���[�U�f�[�^: HKEY_CURRENT_USER\Software\TvRock
*/
class CRockBarResizer :
		public CWindowImplBase,
		public CDialogResize<CRockBarResizer>
{
public:
	#define	IDC_CHLIST		0x3EA
	#define	IDC_DEVICELIST	0x46A
	#define	IDC_RESERVELIST 0x3EB
	#define	IDC_PROGRAMINFO	0x4CB

	BEGIN_DLGRESIZE_MAP(CWindowResizer)
		DLGRESIZE_CONTROL(IDC_CHLIST,		DLSZ_SIZE_X | DLSZ_SIZE_Y)	// �`�����l���ꗗ
		DLGRESIZE_CONTROL(IDC_DEVICELIST,	DLSZ_MOVE_X | DLSZ_SIZE_Y)	// �f�o�C�X�ꗗ
		DLGRESIZE_CONTROL(IDC_RESERVELIST,	DLSZ_SIZE_X | DLSZ_SIZE_Y)	// ?
		DLGRESIZE_CONTROL(IDC_PROGRAMINFO,	DLSZ_SIZE_X | DLSZ_SIZE_Y)	// �ԑg���
		DLGRESIZE_CONTROL(0x403,               DLSZ_MOVE_Y)	// �d��
		DLGRESIZE_CONTROL(0x404,               DLSZ_MOVE_Y)	// �^��
		DLGRESIZE_CONTROL(0x405,               DLSZ_MOVE_Y)	// �^���~
		DLGRESIZE_CONTROL(0x40B,               DLSZ_MOVE_Y)	// �X�N���[���V���b�g
		DLGRESIZE_CONTROL(0x46E, DLSZ_SIZE_X | DLSZ_MOVE_Y)	// �X�e�[�^�X
		DLGRESIZE_CONTROL(0x406, DLSZ_MOVE_X )	// �~
		DLGRESIZE_CONTROL(0x407, DLSZ_MOVE_X )	// �~�̉�
		DLGRESIZE_CONTROL(0x408, DLSZ_MOVE_X )	// �~�̉��̉�
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP_EX(CRockBarResizer)
		MSG_WM_ERASEBKGND(OnEraseBkGnd)
		MSG_WM_NCHITTEST(OnNcHitTest)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_QUERYENDSESSION(OnQueryEndSession)
		CHAIN_MSG_MAP(CDialogResize<CRockBarResizer>)
	END_MSG_MAP()

public:

	// DlgResize_Init �łւ�ȃX�^�C�����ݒ肳���̂𖳗��������邽�߂�override
	DWORD GetStyle() {
		return WS_CHILD;
	}

	// ������
	void Init(HWND hRockBar) {
	
		// �����T�C�Y�ۑ�
		CRect	rect;
		::GetWindowRect(hRockBar, &rect);
		m_OrgSize = rect.Size();

		// �T�u�N���X��
		SubclassWindow(hRockBar);

		// WS_CLIPCHILDREN��ǉ�(���T�C�Y�����Ƃ��̃������`�����΍�)
		ModifyStyle(0, WS_CLIPCHILDREN);

		// ���[�W������ۑ����āA���
		SetWindowRgn(NULL);

		// �R���g���[���̃��C�A�E�g��������
		DlgResize_Init(false, false, 0);

		// �ݒ蕜��
		{
			CRegKey	reg;
			if (reg.Open(HKEY_CURRENT_USER, _T(TVROCK_RESIZER_KEY), KEY_READ) == ERROR_SUCCESS) {
				DWORD	width = m_OrgSize.cx;
				DWORD	height = m_OrgSize.cy;
				reg.QueryDWORDValue(_T("Width"), width);
				reg.QueryDWORDValue(_T("Height"), height);
				
				SetWindowPos(0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
		// TvRock�̃f�[�^�̃p�X���擾����
		// ��������Rock�o�[�̔w�i�摜�����[�h
		{
			CString	dataDir;

			CRegKey	reg;
			if (reg.Open(HKEY_CURRENT_USER, _T("Software\\TvRock"), KEY_READ) == ERROR_SUCCESS) {
				DWORD size = MAX_PATH;
				reg.QueryStringValue(_T("DOCUMENT"), dataDir.GetBuffer(size+1), &size);
				dataDir.ReleaseBuffer(size);
			}

			TCHAR	bmpPath[MAX_PATH+1] = {0};
			PathCombine(bmpPath, dataDir, _T("rockbar1.bmp"));

			m_Bmp.Attach(static_cast<HBITMAP>(
					::LoadImage(NULL, bmpPath, IMAGE_BITMAP, 0, 0, 
						LR_DEFAULTCOLOR | LR_LOADFROMFILE)
			));

		}
		
		#ifdef DEBUG
		// �e�X�g�p�̃_�~�[�e�L�X�g�ݒ�
		{
			HWND hList = GetDlgItem(IDC_CHLIST);
			::SetWindowLong(hList, GWL_STYLE,
				::GetWindowLong(hList, GWL_STYLE) & ~LVS_NOCOLUMNHEADER);

			CListViewCtrl	lv(hList);
			lv.InsertItem(0, _T("���݁[�Ă�����"));
			lv.SetItemText(0, 1, _T("���݁[�Ă�����"));
			lv.SetItemText(0, 2, _T("���݁[�Ă�����"));
		}
		#endif

		// �ĕ`��
		Invalidate();
	}


protected:

	CSize		m_OrgSize;
	CBitmap		m_Bmp;
	BOOL		m_bScanMenu;

	// WM_DESTROY
	void OnDestroy() {
		SetMsgHandled(FALSE);
		Uninit();
	}

	// WM_ERASEBKGND
	LRESULT OnEraseBkGnd(HDC hDC) {
		// �w�i��`��
		SetMsgHandled(TRUE);

		CDCHandle	dc(hDC);

		CRect	cr;
		GetClientRect(&cr);

		if (!m_Bmp.IsNull()) {
			// �r�b�g�}�b�v�����[�h�ł��Ă���`��
			CDC	tmp_dc;
			tmp_dc.CreateCompatibleDC();
			tmp_dc.SelectBitmap(m_Bmp);

			CSize	bmpSize;
			m_Bmp.GetSize(bmpSize);

			if (0) {
				// �P���Ȉ������΂�
				dc.StretchBlt(0, 0, cr.right, cr.bottom, 
						tmp_dc, 1, 1, bmpSize.cx-1, bmpSize.cy-1, SRCCOPY);
			}
			else {
				// �Â����������΂�

				// Rock�o�[�̃f�t�H���g�̉摜�ɂ����ԊO���̍������������O����
				CPoint	offset(1,1);
				bmpSize.cx -= offset.x * 2;
				bmpSize.cy -= offset.y * 2;

				bmpSize.cx--;
				bmpSize.cy--;

				// �������΂����̈�ƈ������΂��Ȃ��̈�𕪗����ĕ`��
				int	px[] = {20, 0, 9};	// �Œ�̈�̃T�C�Y ��,dummy,�E
				int	py[] = {9,  0, 9};	// �Œ�̈�̃T�C�Y ��,dummy,��

				int srcY = offset.y;
				int dstY = 0;
				for (int r=0; r<3; r++) {
					int srcHeight = (r == 1 ? (bmpSize.cy  - py[0] - py[2]) : py[r] );
					int dstHeight = (r == 1 ? (cr.Height() - py[0] - py[2]) : srcHeight);

					int srcX = offset.x;
					int dstX = 0;
					for (int c=0; c<3; c++) {
						int srcWidth = (c == 1 ? (bmpSize.cx - px[0] - px[2]) : px[c]);
						int dstWidth = (c == 1 ? (cr.Width() - px[0] - px[2]) : srcWidth);
						
						dc.StretchBlt(dstX, dstY, dstWidth, dstHeight,
							tmp_dc, srcX, srcY, srcWidth, srcHeight, SRCCOPY);

						srcX += srcWidth;
						dstX += dstWidth;
					}

					srcY += srcHeight;
					dstY += dstHeight;
				}
			}
		}
		else {
			// �r�b�g�}�b�v�����[�h�ł��ĂȂ��Ƃ��́A�̂��؂�
			dc.DrawEdge(cr, BDR_RAISED, BF_RECT | BF_MIDDLE);
		}

		return 1;
	}

	
	// WM_NCHITTEST
	LRESULT OnNcHitTest(CPoint pt) {
		// �E�[�Ɖ��[���h���b�O���ăE�B���h�E�T�C�Y��ύX�ł���悤�ɂ���
		const int	grip = 6;
		CRect		rc;

		GetWindowRect(&rc);

		DWORD idx = 
			 (pt.y <= rc.top    + grip ? 1 : 0)		// ��[
			+(pt.y >= rc.bottom - grip ? 2 : 0)		// ���[
			+(pt.x <= rc.left   + grip ? 4 : 0)		// ���[
			+(pt.x >= rc.right  - grip ? 8 : 0);	// �E�[
		if (idx > 0) {
			if (idx & 1) {
				if (idx & 4) return HTTOPLEFT;
				if (idx & 8) return HTTOPRIGHT;
				return HTTOP;
			}
			else if(idx & 2) {
				if (idx & 4) return HTBOTTOMLEFT;
				if (idx & 8) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}
			else {
				if (idx & 4) return HTLEFT;
				if (idx & 8) return HTRIGHT;
			}
		}

		SetMsgHandled(FALSE);
		return 0;
	}

	// ����
	void Uninit()  {
		// �ݒ�ۑ�
		SaveWindowSize();

		// �T�C�Y��߂�
		SetWindowPos(0, 0, 0, m_OrgSize.cx, m_OrgSize.cy, 
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

		// ���C�A�E�g��߂�
		{
			CRect	crc;
			GetClientRect(crc);
			DlgResize_UpdateLayout(crc.Width(), crc.Height());
		}

		// �T�u�N���X�����������čĕ`��
		HWND hRockBar = UnsubclassWindow();
		::InvalidateRect(hRockBar, NULL, TRUE);

		// ����炷
		/*
		for (int j=0; j<3; j++) {
			MessageBeep(MB_OK);
			Sleep(50);
		}*/
	}

	// WM_SIEZE
	void OnSize(UINT nType, CSize size) {

		// ���g�����T�C�Y(CHAIN_MSG_MAP�ŏ���ɂ���Ă����Ƃ��낾���A
		// ���̌�̏�����ǉ������������̂Ńn���h������)

		DlgResize_UpdateLayout(size.cx, size.cy);

		// ���X�g�r���[�̃J�����̕������X�g�̑傫���ɍ��킹��
		CListViewCtrl	lv(GetDlgItem(IDC_CHLIST));

		CRect	cr;
		lv.GetClientRect(cr);

		CRect	ir;
		lv.GetSubItemRect(0, 2, LVIR_BOUNDS, ir);
		lv.SetColumnWidth(2, cr.right - ir.left);

		// ���[�W�����ݒ�(�p���ۂ�����)
		if (1) {
			CRgnHandle 	rgn;
			rgn.CreateRoundRectRgn(0, 0, size.cx+1, size.cy+1, 6, 6);
			SetWindowRgn(rgn, TRUE);
		}

	}

	void SaveWindowSize() {
		CRect	rect;
		GetWindowRect(&rect);

		CRegKey	reg;
		if (reg.Create(HKEY_CURRENT_USER, _T(TVROCK_RESIZER_KEY)) == ERROR_SUCCESS) {
			// �E�B���h�E�T�C�Y�ۑ�
			reg.SetDWORDValue(_T("Width"), rect.Width());
			reg.SetDWORDValue(_T("Height"), rect.Height());
		}
	}

	LRESULT OnQueryEndSession(UINT wParam, UINT lParam) {
		SetMsgHandled(FALSE);
		SaveWindowSize();

		return 0;
	}

};
