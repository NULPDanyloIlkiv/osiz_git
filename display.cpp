#include "display.h"

_cDisplay_::_cDisplay_(HDC hDC, POINT pRes) :
	m_resolution(pRes)
{
	if (hDC == NULL)
	{ exit(Helper::ShowFormatError
		(L"m_hDC is [NULL]", ERROR_INVALID_HANDLE));
	}

	m_hDC = hDC;
	m_hDrawDC = CreateCompatibleDC(m_hDC);

	DWORD dwResult = Create32BppDisplay(m_display);
	if (dwResult != ERROR_SUCCESS)
	{ exit(Helper::ShowFormatError(L"Create32BppVirtualDisplay", dwResult)); }
}
_cDisplay_::~_cDisplay_(void) {
	if (!DeleteBitmap(m_display.hFrameBuf))
	{ exit(Helper::ShowLastError(L"DeleteBitmap(m_display.hFrameBuf) - Failed!")); }
	if (!DeleteDC(m_hDrawDC))
	{ exit(Helper::ShowLastError(L"DeleteDC(m_hDrawDC) - Failed!")); }
}

DWORD _cDisplay_::Create32BppDisplay(sDisplayData& displayData) {
	DWORD dwError = ERROR_SUCCESS;

	bool _bInvertion_ = 0x0;

	if (m_resolution.x == NULL || m_resolution.y == NULL)
	{ dwError = ERROR_INVALID_DATA; goto linkExit; }

#ifdef GDI_INVERT_Y_AXIS
	_bInvertion_ = 0x1;
#endif

	displayData.bmiFrameBufInfo.
		bmiHeader.biHeight = m_resolution.y * (_bInvertion_ ? -(0x1) : 0x1);
	displayData.bmiFrameBufInfo.bmiHeader.biWidth = m_resolution.x;

	displayData.bmiFrameBufInfo.bmiHeader.biSize = sizeof(displayData.bmiFrameBufInfo.bmiHeader);
	displayData.bmiFrameBufInfo.bmiHeader.biPlanes = 0x1;
	displayData.bmiFrameBufInfo.bmiHeader.biBitCount = GDI_COLOR_DEPTH;
	displayData.bmiFrameBufInfo.bmiHeader.biCompression = BI_RGB;

	if((displayData.hFrameBuf = CreateDIBSection(m_hDC,
		&displayData.bmiFrameBufInfo, DIB_RGB_COLORS, (void**)&displayData.dwMemory, 0x0, 0x0)
	) == NULL)
	{ dwError = GetLastError(); goto linkExit; }

	SelectBitmap(m_hDrawDC, displayData.hFrameBuf);

linkExit:
	return(dwError);
}

void _cDisplay_::Show(RECT rSize) {
    DWORD dwMode = SetStretchBltMode(
        m_hDC, STRETCH_DELETESCANS
    );

	StretchDIBits(m_hDC, rSize.left, rSize.top, rSize.right - rSize.left, rSize.bottom - rSize.top, 0x0, 0x0,
		m_resolution.x, m_resolution.y, m_display.dwMemory, &m_display.bmiFrameBufInfo, DIB_RGB_COLORS, SRCCOPY
	);

	SetStretchBltMode(
        m_hDC, dwMode
    );
}

HDC _cDisplay_::GetDrawDC(void) const
{ return(m_hDrawDC); }
POINT _cDisplay_::GetResolution(void) const
{ return(m_resolution); }

DWORD* _cDisplay_::dwGetDisplayData(void) const
{ return(m_display.dwMemory); }
BYTE* _cDisplay_::cbGetDisplayData(void) const
{ return(m_display.cbMemory); }
