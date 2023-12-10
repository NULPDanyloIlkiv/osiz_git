#include "window.h"

_cWindow_::_cWindow_(void) :
	_cWindow_(GetModuleHandle(NULL), SW_SHOWNORMAL)
{ /*Code...*/ }

_cWindow_::_cWindow_(const _In_ HINSTANCE& hInst, _In_ int nCmdShow) : m_hWnd(NULL) {
	m_hInstance = hInst; m_nCmdShow = nCmdShow;

	HANDLE hProcess = GetCurrentProcess();
	if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
		exit(Helper::ShowLastError(L"SetPriorityClass"));
	HANDLE hThread = GetCurrentThread();
	if (!SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST))
		exit(Helper::ShowLastError(L"SetThreadPriority"));
}
_cWindow_::~_cWindow_(void) {
	/*Code...*/
}

BOOL _cWindow_::CreateC(
	LPWSTR lpszClassName,
	HCURSOR hCursor,
	HBRUSH hbrBackground,
	HICON hIcon,
	HICON hIconSm,
	LPCWSTR lpszMenuName,
	int cbClsExtra, int cbWndExtra
)
{
	if (lpszClassName != NULL)
		wcscpy_s(
			m_szWindowClass, lpszClassName
		);
	else { return(0x0); }

	WNDCLASSEX wCex;
	ZeroMemory(
		&wCex, sizeof(wCex)
	);

	wCex.lpszClassName = m_szWindowClass;

	wCex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wCex.cbClsExtra = cbClsExtra;
	wCex.cbWndExtra = cbWndExtra;
	wCex.hIcon = hIcon;
	wCex.hCursor = hCursor;
	wCex.hbrBackground = hbrBackground;
	wCex.lpszMenuName = lpszMenuName;
	wCex.hIconSm = hIconSm;

	wCex.lpfnWndProc = _cWindow_::WindowProc;
	wCex.hInstance = m_hInstance;
	wCex.cbSize = sizeof(WNDCLASSEX);

	return(
		RegisterClassEx(&wCex) ? TRUE : FALSE
	);
}

BOOL _cWindow_::CreateW(
	LPWSTR lpszWindowName,
	int nWidth,
	int nHeight,
	DWORD dwStyle,
	DWORD dwExStyle,
	HMENU hMenu
)
{
	if (lpszWindowName != NULL)
		wcscpy_s(
			m_szTitle, lpszWindowName
		);
	else { return(0x0); }

	RECT _r = { 0x0, 0x0, nWidth, nHeight };
	if(!AdjustWindowRectEx(&_r, dwStyle, TRUE, dwExStyle))
	{ exit(Helper::ShowLastError
		(L"AdjustWindowRectEx(&r, dwStyle, hMenu != NULL, dwExStyle)"));
	}

	(void)CreateWindowEx(
		dwExStyle, m_szWindowClass, m_szTitle, dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, _r.right - _r.left, _r.bottom - _r.top,
		NULL, hMenu, m_hInstance, (LPVOID)(this)
	);

	return(
		m_hWnd ? TRUE : FALSE
	);
}



LRESULT CALLBACK _cWindow_::WindowProc(_In_ HWND hWnd,
	_In_ UINT uMessage,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam)
{
	_cWindow_* pThis = nullptr;

	if (uMessage == WM_NCCREATE) {
		CREATESTRUCT* pCreate = reinterpret_cast
			<CREATESTRUCT*>(lParam);
		pThis = reinterpret_cast
			<_cWindow_*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

		pThis->m_hWnd = hWnd;

		pThis->m_wSize.store
			(POINT{ (LONG)pCreate->cx, (LONG)pCreate->cy } );
	}
	else
	{ pThis = reinterpret_cast<_cWindow_*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)); }
	if (pThis)
	{ return pThis->HandleMessage(uMessage, wParam, lParam); }
	else
	{ return DefWindowProc(hWnd, uMessage, wParam, lParam); }
}

DWORD _cWindow_::Start(void) {
	if (m_hWnd == NULL) {
		return(ERROR_INVALID_HANDLE);
	}

	(void)ShowWindow(
		m_hWnd, m_nCmdShow
	);
	(void)UpdateWindow(m_hWnd);

	MSG msg = { 0x0 };
	while (GetMessage(&msg, NULL, 0x0, 0x0) > 0x0)
		if (!TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
		{ TranslateMessage(&msg); DispatchMessage(&msg); }

	return((int)msg.wParam);
}

HINSTANCE _cWindow_::GetInstance(void) const
{ return(m_hInstance); }

HWND _cWindow_::GetWnd(void) const
{ return(m_hWnd); }
POINT _cWindow_::GetWndSize(void) const
{ return(m_wSize); }
