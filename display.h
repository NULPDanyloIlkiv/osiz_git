#pragma once
#ifndef _TRIVIAL_DISPLAY_H_
#define _TRIVIAL_DISPLAY_H_

#include "framework.h"

#ifndef GDI_32b
#define GDI_32b (32)
#endif

#ifndef GDI_24b
#define GDI_24b (24)
#endif

#ifndef GDI_COLOR_DEPTH
#define GDI_COLOR_DEPTH (GDI_32b)
#endif

#ifndef GDI_INVERT_Y_AXIS
#define GDI_INVERT_Y_AXIS
#endif

class _cDisplay_ {

protected:
	HDC m_hDC = NULL, m_hDrawDC = NULL;
	POINT m_resolution = { 0x0 };

	struct sDisplayData {
		HBITMAP hFrameBuf;
		BITMAPINFO bmiFrameBufInfo;
		union {
			DWORD* dwMemory;
			BYTE* cbMemory;
		};
	} m_display = { 0x0 };

private:
	DWORD Create32BppDisplay
		(sDisplayData& screenData);

public:
	_cDisplay_(HDC hDC, POINT pRes);
	~_cDisplay_(void);

	void Show(RECT rSize);

	HDC GetDrawDC(void) const;
	POINT GetResolution(void) const;

	DWORD* dwGetDisplayData(void) const;
	BYTE* cbGetDisplayData(void) const;
};

#endif //_TRIVIAL_DISPLAY_H_
