#include "texture.h"

#include "rgba.h"

#define _32b_ (32)
#define _24b_ (24)

/* Bitmap file format
 *
 * SECTION
 * Address:Bytes	Name
 *
 * HEADER:
 *	  0:	2		"BM" magic number
 *	  2:	4		File size
 *	  6:	4		Junk
 *	 10:	4		Starting address of image data
 * BITMAP HEADER:
 *	 14:	4		Header size
 *	 18:	4		Width  (signed)
 *	 22:	4		Height (signed)
 *	 26:	2		Number of color planes
 *	 28:	2		Bits per pixel
 *	[...]
 * [OPTIONAL COLOR PALETTE, NOT PRESENT IN 32 BIT BITMAPS]
 * BITMAP DATA:
 *	DATA:	X	Pixels
 */

_cTexture_::_cTexture_(UINT x, UINT y) {
    m_wsFileName = L"NULL";

	DWORD dwResult = Create32BppBitmap(
        x, y, m_texture
    );
	if (dwResult != ERROR_SUCCESS) {
        Helper::ShowFormatError(
            L"Create32BppBitmap", dwResult
        );
    }
	m_bIsValid = (bool)!dwResult;
}
_cTexture_::_cTexture_(
    const std::wstring& wsFileName
) : m_wsFileName(wsFileName)
{
    DWORD dwResult = Load32BppBitmap(
        wsFileName, m_texture
    );
	if (dwResult != ERROR_SUCCESS) {
        wchar_t wcError[256]{};

        wsprintf(
            wcError, L"Failed to Load : %s", wsFileName.data()
        );

		Helper::ShowFormatError(
			wcError, dwResult
		);
	}
	m_bIsValid = (bool)!dwResult;
}
_cTexture_::~_cTexture_(void) {
	if (
        m_bIsValid && !HeapFree(GetProcessHeap(), 0x0, m_texture.dwMemory)
    ) {
        Helper::ShowLastError(
            L"HeapFree(GetProcessHeap(), 0x0, m_texture.nMemory) - Failed!"
        );
	}
}

_cTexture_::_cTexture_(
    const _cTexture_* pT
)
{
    m_wsFileName = pT->m_wsFileName;

	DWORD dwResult = Copy32BppBitmap(pT);

	if (dwResult != ERROR_SUCCESS) {
        Helper::ShowFormatError(
            L"Copy32BppBitmap", dwResult
        );
    }
	m_bIsValid = (bool)!dwResult;
}

bool _cTexture_::IsValid(void) const
{ return(m_bIsValid); }

const wchar_t* _cTexture_::GetFileName(void) const
{ return(m_wsFileName.data()); }

POINT _cTexture_::GetSize(void) const
{ return { m_texture.bmiTextureData.bmiHeader.biWidth,
	m_texture.bmiTextureData.bmiHeader.biHeight };
}

BYTE* _cTexture_::cbGetMemoryData(void) const
{ return(m_texture.cbMemory); }

DWORD* _cTexture_::dwGetMemoryData(void) const
{ return(m_texture.dwMemory); }

const BITMAPINFO* _cTexture_::GetTextureData(void) const
{ return(&m_texture.bmiTextureData); }



RGBA _cTexture_::GetColour(int x, int y) const {
    if (
        x < 0x0 || x >= m_texture.bmiTextureData.bmiHeader.biWidth || y < 0x0 || y >= m_texture.bmiTextureData.bmiHeader.biHeight
    ) { return(RGBA(0xFF, 0xFF, 0xFF, 0xFF)); }

	RGBA col = { 0x0, 0x0, 0x0 };
	switch (m_texture.bmiTextureData.bmiHeader.biBitCount) {
	case(_32b_): {
		memcpy(&col, m_texture.dwMemory + (m_texture.bmiTextureData.bmiHeader.biHeight - y - 0x1) * m_texture.bmiTextureData.bmiHeader.biWidth + x, sizeof(DWORD));
	} break;
	case(_24b_): {
		BYTE cbCount = m_texture.bmiTextureData.bmiHeader.biBitCount / 0x8;
		memcpy(&col, m_texture.cbMemory + cbCount * ((m_texture.bmiTextureData.bmiHeader.biHeight - y - 0x1) * m_texture.bmiTextureData.bmiHeader.biWidth + x), cbCount);
	} break;

	default:
		break;
	}
	return(col);
}
void _cTexture_::SetColour(int x, int y, RGBA col) {
    if (
        x < 0x0 || x >= m_texture.bmiTextureData.bmiHeader.biWidth || y < 0x0 || y >= m_texture.bmiTextureData.bmiHeader.biHeight
    ) { return; }

	switch (m_texture.bmiTextureData.bmiHeader.biBitCount) {
	case(_32b_): {
		memcpy(m_texture.dwMemory + (m_texture.bmiTextureData.bmiHeader.biHeight - y - 0x1) * m_texture.bmiTextureData.bmiHeader.biWidth + x,
			&col, sizeof(DWORD)
        );
	} break;
	case(_24b_): {
		BYTE cbCount = m_texture.bmiTextureData.bmiHeader.biBitCount / 0x8;
		memcpy(m_texture.cbMemory + cbCount * ((m_texture.bmiTextureData.bmiHeader.biHeight - y - 0x1) * m_texture.bmiTextureData.bmiHeader.biWidth + x),
			&col, cbCount
        );
	} break;

	default:
		break;
	}
}

RGBA _cTexture_::SampleColour(double x, double y) const {
	int sx = (int)(x * (double)m_texture.bmiTextureData.bmiHeader.biWidth - (x != 0x0 ? 1.f : 0x0));
	int sy = (int)(y * (double)m_texture.bmiTextureData.bmiHeader.biHeight - (y != 0x0 ? 1.f : 0x0));
	if (
        sx < 0x0 || sx >= m_texture.bmiTextureData.bmiHeader.biWidth || sy < 0x0 || sy >= m_texture.bmiTextureData.bmiHeader.biHeight
    ) {
        return(
            RGBA(0xFF, 0xFF, 0xFF, 0xFF)
        );
    }

	return(
        GetColour(sx, sy)
    );
}

DWORD _cTexture_::Save32BppBitmap(const std::wstring& sFileName) {
	DWORD dwError = ERROR_SUCCESS;

	goto linkExit;

linkExit:
	return(dwError);
}

DWORD _cTexture_::Create32BppBitmap(
    UINT x, UINT y, _sTextureData_& textureData
)
{
	DWORD dwError = ERROR_SUCCESS;

	if (x == NULL || y == NULL)
	{ dwError = ERROR_INVALID_DATA; goto linkExit; }

	if((textureData.dwMemory = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
		sizeof(DWORD*) * (x * y))) == NULL
    ) { dwError = ERROR_NOT_ENOUGH_MEMORY; goto linkExit; }

	textureData.bmiTextureData.bmiHeader.biWidth = x;
	textureData.bmiTextureData.bmiHeader.biHeight = y;

	textureData.bmiTextureData.bmiHeader.biSize = sizeof(textureData.bmiTextureData.bmiHeader);
	textureData.bmiTextureData.bmiHeader.biPlanes = 0x1;
	textureData.bmiTextureData.bmiHeader.biBitCount = BITS_PER_PIXEL;
	textureData.bmiTextureData.bmiHeader.biCompression = BI_RGB;

linkExit:
	return(dwError);
}

DWORD _cTexture_::Load32BppBitmap(
    const std::wstring& sFileName, _sTextureData_& textureData
)
{
	DWORD dwError = ERROR_SUCCESS;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	WORD wBitmapHeader = 0x0; DWORD dwPixelData = 0x0, dwNumberOfBytes = 0x0;

	WCHAR wcFullPath[MAX_PATH]{};
	GetFullPathName(sFileName.data(), MAX_PATH, wcFullPath, NULL);

	if ((hFile = CreateFile(wcFullPath, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
	{ dwError = GetLastError(); goto linkExit; }

	if (!ReadFile(hFile, &wBitmapHeader, 0x2, &dwNumberOfBytes, NULL))
	{ dwError = GetLastError(); goto linkExit; }

	if(wBitmapHeader != ('M' << 0x8 | 'B'))
	{ dwError = ERROR_FILE_INVALID; goto linkExit; }

	if(SetFilePointer(hFile, 0xA, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{ dwError = GetLastError(); goto linkExit; }

	if(!ReadFile(hFile, &dwPixelData, sizeof(DWORD), &dwNumberOfBytes, NULL))
	{ dwError = GetLastError(); goto linkExit; }

	if(SetFilePointer(hFile, 0xE, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{ dwError = GetLastError(); goto linkExit; }

	if(!ReadFile(hFile, &textureData.bmiTextureData.bmiHeader,
		sizeof(BITMAPINFOHEADER), &dwNumberOfBytes, NULL))
	{ dwError = GetLastError(); goto linkExit; }

	if((textureData.dwMemory = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
		textureData.bmiTextureData.bmiHeader.biSizeImage)) == NULL
	) { dwError = ERROR_NOT_ENOUGH_MEMORY; goto linkExit; }

	if(SetFilePointer(hFile, dwPixelData, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{ dwError = GetLastError(); goto linkExit; }

	if(!ReadFile(hFile, textureData.dwMemory,
		textureData.bmiTextureData.bmiHeader.biSizeImage, &dwNumberOfBytes, NULL))
	{ dwError = GetLastError(); goto linkExit; }

	textureData.bmiTextureData.bmiHeader.biCompression = BI_RGB;

linkExit:
	if (!CloseHandle(hFile))
	{ dwError = GetLastError(); }
	return(dwError);
}

DWORD _cTexture_::Copy32BppBitmap(
    const _cTexture_* pT
)
{
    DWORD dwError = ERROR_SUCCESS;

    int x = pT->m_texture.bmiTextureData.bmiHeader.biWidth;
    int y = pT->m_texture.bmiTextureData.bmiHeader.biHeight;

    if (x == NULL || y == NULL)
	{ dwError = ERROR_INVALID_DATA; goto linkExit; }

	if((m_texture.dwMemory = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
		sizeof(DWORD*) * (x * y))) == NULL
    ) { dwError = ERROR_NOT_ENOUGH_MEMORY; goto linkExit; }

    memcpy(
        m_texture.dwMemory, pT->m_texture.dwMemory, sizeof(DWORD) * x * y
    );

    m_texture.bmiTextureData = pT->m_texture.bmiTextureData;

linkExit:
	return(dwError);
}

void _cTexture_::ChangeFileName(
    const std::wstring& wsFileName
)
{
    if (
        !wsFileName.empty()
    ) { m_wsFileName = wsFileName; }
}
