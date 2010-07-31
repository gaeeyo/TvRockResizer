// TvRockResizerDll.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "TvRockResizerDll.h"
#include "TvRockUtil.h"
#include "KeywordReserveDlgResizer.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// 前のHHOOK
static HHOOK	_org_hhook = NULL;

static CRockBarResizer	_rockBar;

// prototype
LRESULT CALLBACK MyCallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam);

// DllMain
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			// 自分の参照カウントを増やして、TvRockの判断でアンロードされないようにする
			TCHAR	path[MAX_PATH+1] = {0};
			GetModuleFileName(hModule, path, MAX_PATH);
			LoadLibrary(path);

			_org_hhook = SetWindowsHookEx(WH_CALLWNDPROCRET, MyCallWndRetProc, hModule, GetCurrentThreadId());

		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


// MyCallWndProc
LRESULT CALLBACK MyCallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) {
		// ダイアログの作成を監視して、
		// Rockバーが作成されたらサブクラス化する
		CWPRETSTRUCT 	*p = reinterpret_cast<CWPRETSTRUCT *>(lParam);

		if (p->message == WM_INITDIALOG) {
			static BOOL bSubclassed = FALSE;
			if (!bSubclassed) {
				if (p->hwnd == FindRockBar()) {
					bSubclassed = TRUE;
					// サブクラス化
					_rockBar.Init(p->hwnd);
				}
			}
			if (CKeywordReserveDlgResizer::IsTargetWindow(p->hwnd)) {
				// キーワード検索予約の改造
				LRESULT lr = CallNextHookEx(_org_hhook, nCode, wParam, lParam);

				CKeywordReserveDlgResizer::Apply(p->hwnd);
				return lr;
			}
		}
	}
	return CallNextHookEx(_org_hhook, nCode, wParam, lParam);
}

