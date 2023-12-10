#include "rgba.h"

RGBA::RGBA(void) { Red = 0x0, Green = 0x0, Blue = 0x0, Alpha = 0x0; }
RGBA::RGBA(BYTE R, BYTE G, BYTE B, BYTE A)
{ Red = R, Green = G, Blue = B, Alpha = A; }
RGBA::RGBA(BYTE R, BYTE G, BYTE B)
{ Red = R, Green = G, Blue = B, Alpha = 0x0; }

RGBA::RGBA(BYTE col)
{ Red = Green = Blue = Alpha = col; }

RGBA::RGBA(COLORREF col)
{ Red = GetRValue(col), Green = GetGValue(col), Blue = GetBValue(col);
	Alpha = col >> 24;
}
RGBA::RGBA(eColour col) :
	RGBA::RGBA((COLORREF)col) { /*Code...*/ }

RGBA& RGBA::operator=(BYTE col)
{ Red = Blue = Green = col; return (*this); }

bool RGBA::operator == (RGBA const& col)
{ return (Red == col.Red) && (Green == col.Green) && (Blue == col.Blue); }
bool RGBA::operator != (RGBA const& col)
{ return (Red != col.Red) || (Green != col.Green) || (Blue != col.Blue); }



RGBA RGBA::operator+(RGBA const& col)
{ return RGBA(std::min(Red + col.Red, 0xFF), std::min(Green + col.Green, 0xFF), std::min(Blue + col.Blue, 0xFF), std::min(Alpha + col.Alpha, 0xFF)); }
RGBA RGBA::operator-(RGBA const& col)
{ return RGBA(std::max(Red - col.Red, 0x0), std::max(Green - col.Green, 0x0), std::max(Blue - col.Blue, 0x0), std::max(Alpha - col.Alpha, 0x0)); }


RGBA RGBA::operator+(BYTE cbCol)
{ return RGBA(std::min(Red + cbCol, 0xFF), std::min(Green + cbCol, 0xFF), std::min(Blue + cbCol, 0xFF), std::min(Alpha + cbCol, 0xFF)); }
RGBA RGBA::operator-(BYTE cbCol)
{ return RGBA(std::max(Red - cbCol, 0x0), std::max(Green - cbCol, 0x0), std::max(Blue - cbCol, 0x0), std::max(Alpha - cbCol, 0x0)); }



RGBA::operator COLORREF(void)
{ return(RGB(Red, Green, Blue) | (Alpha << 24)); }
