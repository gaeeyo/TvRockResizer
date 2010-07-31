#include "stdafx.h"
#include "TvRockUtil.h"

// TvRockのメインウィンドウを探す
HWND FindTvRockMainWindow()
{
	return FindWindow(_T("TVROCK"), _T("tvrock_window_class"));
}

// Rockバーらしきウィンドウを探す
HWND FindRockBar()
{
	HWND hMainWnd = FindTvRockMainWindow();
	if (hMainWnd) {
		HWND hWnd = NULL;
		HWND hPrevWnd = NULL;

		while (hWnd = FindWindowEx(NULL, hPrevWnd, _T("#32770"), _T(""))) {
			if (GetWindow(hWnd, GW_OWNER) == hMainWnd) {
				return hWnd;
			}
			hPrevWnd = hWnd;
		}
	}
	return NULL;
}