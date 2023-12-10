#pragma once
#ifndef _TRIVIAL_RGBA_H_
#define _TRIVIAL_RGBA_H_

#include "framework.h"

enum class eColour {
	BLACK = RGB(0xF, 0xF, 0xF),

	DARK_RED = RGB(0x80, 0x0, 0x0),
	LIGHT_RED = RGB(0xFF, 0x0, 0x0),

	DARK_GREEN = RGB(0x0, 0x80, 0x0),
	LIGHT_GREEN = RGB(0x0, 0xFF, 0x0),

	DARK_BLUE = RGB(0x0, 0x0, 0x80),
	LIGHT_BLUE = RGB(0x0, 0x0, 0xFF),

	DARK_YELLOW = RGB(0x80, 0x80, 0x0),
	LIGHT_YELLOW = RGB(0xFF, 0xFF, 0x0),

	DARK_TURQUOICE = RGB(0x0, 0x80, 0x80),
	LIGHT_TURQUOICE = RGB(0x0, 0xFF, 0xFF),

	DARK_LILAC = RGB(0x80, 0x0, 0x80),
	LIGHT_LILAC = RGB(0xFF, 0x0, 0xFF),

	DARK_GRAY = RGB(0x80, 0x80, 0x80),
	LIGHT_GRAY = RGB(0xC0, 0xC0, 0xC0),

	WHITE = RGB(0xFF, 0xFF, 0xFF)
};

class RGBA {

public:
	RGBA(void);
	RGBA(
		BYTE R, BYTE G, BYTE B, BYTE A
	);
	RGBA(
		BYTE R, BYTE G, BYTE B
	);

	RGBA(BYTE col);

	RGBA(COLORREF col);
	RGBA(eColour col);

	RGBA& operator=(BYTE col);

	bool operator == (RGBA const& col);
	bool operator != (RGBA const& col);

	RGBA operator+(RGBA const& col);
	RGBA operator-(RGBA const& col);

	RGBA operator+(BYTE cbCol);
	RGBA operator-(BYTE cbCol);

	union {
		struct {
			BYTE Blue, Green, Red, Alpha;
		};
		COLORREF Col;
	};

	operator COLORREF(void);
};

#endif //_TRIVIAL_RGBA_H_
