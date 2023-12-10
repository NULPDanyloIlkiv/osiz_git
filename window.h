#pragma once
#ifndef _TRIVIAL_WINDOW_H_
#define _TRIVIAL_WINDOW_H_

#include "framework.h"

class _cWindow_ {
	static LRESULT CALLBACK WindowProc(_In_ HWND hWnd,
		_In_ UINT uMessage,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam
	);

public:
	_cWindow_(void);
	_cWindow_(const _In_ HINSTANCE& hInst,
		_In_ int nCmdShow = SW_SHOWNORMAL);
	virtual ~_cWindow_(void);

	HINSTANCE GetInstance(void) const;

	HWND GetWnd(void) const;
	POINT GetWndSize(void) const;

	BOOL CreateC(
		LPWSTR lpszClassName = NULL,
		HCURSOR hCursor = LoadCursor(GetModuleHandle(NULL), IDC_HAND),
		HBRUSH hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION),
		HICON hIconSm = LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION),
		LPCWSTR lpszMenuName = NULL,
		int cbClsExtra = NULL, int cbWndExtra = NULL
	);
	BOOL CreateW(
		LPWSTR lpszWindowName = NULL,
		int nWidth = CW_USEDEFAULT,
		int nHeight = CW_USEDEFAULT,
		DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW,
		HMENU hMenu = NULL
	);

	virtual DWORD Start(void);

protected:
	virtual LRESULT HandleMessage(
        UINT _In_ uMsg, WPARAM _In_ wParam, LPARAM _In_ lParam
    ) = 0;

	DWORD m_nCmdShow = NULL;

	std::atomic<HINSTANCE> m_hInstance;

	std::atomic<HWND> m_hWnd;
	std::atomic<POINT> m_wSize{};

	HACCEL m_hAccelTable = NULL;

#ifndef MAX_LOADSTRING
#define MAX_LOADSTRING 128
#endif

	WCHAR m_szTitle[MAX_LOADSTRING]{};
	WCHAR m_szWindowClass[MAX_LOADSTRING]{};
};

#endif //_TRIVIAL_WINDOW_H_
