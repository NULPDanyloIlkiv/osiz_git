#pragma once
#ifndef _TRIVIAL_GDI_H_
#define _TRIVIAL_GDI_H_

#include "framework.h"

#include "display.h"
#include "rgba.h"

#include "texture.h"

class _cGDI_ :
	public _cDisplay_
{
	HPEN m_hPen = NULL,
		m_hOldPen = NULL;
	HBRUSH m_hBrush = NULL,
		m_hOldBrush = NULL;
	HBRUSH m_hBrBackground = NULL;

public:
	_cGDI_(HDC hDC, POINT wRes) :
		_cDisplay_(hDC, wRes)
	{
		m_hPen = GetStockPen(NULL_PEN);		m_hOldPen = GetStockPen(NULL_PEN);

		m_hBrush = GetStockBrush(NULL_BRUSH);
		m_hOldBrush = GetStockBrush(NULL_BRUSH);

		m_hBrBackground = GetStockBrush(
			WHITE_BRUSH
		);
	}
	~_cGDI_(void) { /*Code...*/ }

	void Clear(void) {
		RECT wRect = { 0x0, 0x0, GetResolution().x, GetResolution().y };
		FillRect(GetDrawDC(), &wRect, m_hBrBackground);
	}
	void SetBGColor(COLORREF col) {
		(void)DeleteBrush(m_hBrBackground);
		m_hBrBackground = CreateSolidBrush(col);
	}

	void SelectNewPen(BYTE cbStyle, BYTE cbWidth, COLORREF col) {
		(void)DeletePen(m_hOldPen);
		m_hOldPen = SelectPen(GetDrawDC(), m_hPen = CreatePen
			(cbStyle, cbWidth, col));
}
	void SelectDefaultPen(BYTE cbParam) {
		(void)DeletePen(m_hOldPen);
		m_hOldPen = SelectPen(GetDrawDC(),
			m_hPen = GetStockPen(cbParam));
	}
	void RestorePen(void)
		{ SelectPen(GetDrawDC(),
			(std::swap(m_hPen, m_hOldPen), m_hPen));
	}



	void SelectNewBrush(BYTE cbStyle, COLORREF col) {
		(void)DeleteBrush(m_hOldBrush);
		m_hOldBrush = SelectBrush
			(GetDrawDC(), m_hBrush = CreateSolidBrush(col));
	}
	void SelectDefaultBrush(BYTE cbParam) {
		(void)DeleteBrush(m_hOldBrush);
		m_hOldBrush = SelectBrush(GetDrawDC(),
			m_hBrush = GetStockBrush(cbParam));
	}
	void RestoreBrush(void)
	{ SelectBrush(GetDrawDC(),
		(std::swap(m_hBrush, m_hOldBrush), m_hBrush));
	}

	void DrawPixel(
		int x, int y, RGBA col = eColour::WHITE
	) {
        if (x >= 0x0 && x < m_resolution.x && y >= 0x0 && y < m_resolution.y) {
#if GDI_COLOR_DEPTH == GDI_32b
            memcpy_s(m_display.dwMemory + (y * m_resolution.x + x),
                sizeof(DWORD), &col, sizeof(DWORD)
            );
#elif GDI_COLOR_DEPTH == GDI_24b
            BYTE cbCount = GDI_COLOR_DEPTH / 0x8;
            memcpy_s(m_display.cbMemory) + cbCount * (y * m_resolution.x + x),
                cbCount, &col, cbCount
            );
#endif
        }
    }

    void DrawTexture(
        int x, int y, const _cTexture_* pT, std::function<RGBA(RGBA)> fFilter = nullptr
    ) {
        if (!pT) return;

        const int nMax[0x2] = {
            x + pT->GetSize().x < m_resolution.x ? pT->GetSize().x : m_resolution.x,
            y + pT->GetSize().y < m_resolution.y ? pT->GetSize().y : m_resolution.y
        };

        for (int iy = y > 0x0 ? y : 0x0; iy < nMax[0x1]; iy++)
            for (int ix = x > 0x0 ? x : 0x0; ix < nMax[0x0]; ix++) {
                if (fFilter)
                { DrawPixel(ix + x, iy + y, fFilter(pT->GetColour(ix, iy))); }
                else
                { DrawPixel(ix + x, iy + y, pT->GetColour(ix, iy)); }
            }
    }
    void DrawSampleTexture(
        int x0, int y0, int x1, int y1, const _cTexture_* pT, std::function<RGBA(RGBA)> fFilter = nullptr
    ) {
        if (!pT) return;

        const int dxy[] = {
            x1 - x0,
            y1 - y0
        };
        const int nMax[] = {
            x1 < m_resolution.x ? dxy[0x0] : dxy[0x0] - (x1 - m_resolution.x),
            y1 < m_resolution.y ? dxy[0x1] : dxy[0x1] - (y1 - m_resolution.y)
        };
        for (signed long y = y0 > 0x0 ? 0x0 : -(y0); y <= nMax[0x1]; y++)
            for (signed long x = x0 > 0x0 ? 0x0 : -(x0); x <= nMax[0x0]; x++) {
                if (fFilter)
                { DrawPixel(x + x0, y + y0,
                    fFilter(pT->SampleColour(x / (double)dxy[0x0], y / (double)dxy[0x1])));
                }
                else
                { DrawPixel(x + x0, y + y0,
                    pT->SampleColour(x / (double)dxy[0x0], y / (double)dxy[0x1]));
                }
            }
    }

	void gdiDrawLine(vec2i p, vec2i dp) {
		if (!MoveToEx(GetDrawDC(), p.x, p.y, NULL))
		{ exit(Helper::ShowLastError(L"gdiDrawLine")); }
		if (!LineTo(GetDrawDC(), dp.x, dp.y))
		{ exit(Helper::ShowLastError(L"gdiDrawLine")); }
	}

	void gdiDrawRectangle(vec2i p1, vec2i p2) {
		if (!Rectangle(GetDrawDC(), p1.x, p1.y, p2.x + 0x1, p2.y + 0x1))
		{ exit(Helper::ShowLastError(L"gdiFillRectangle")); }
	}

	void gdiDrawTriangle(vec2i p1, vec2i p2, vec2i p3) {
		POINT pModel[0x3] = { {p1.x, p1.y}, {p2.x, p2.y}, {p3.x, p3.y} };
		if (!Polygon(GetDrawDC(), pModel, 0x3))
		{ exit(Helper::ShowLastError(L"gdiFillTriangle")); }
	}

	void gdiDrawCircle(vec2i p, INT r) {
		if (!Ellipse(GetDrawDC(), (p.x-r), (p.y-r), (p.x+r) + 0x1, (p.y+r) + 0x1))
		{ exit(Helper::ShowLastError(L"gdiFillRectangle")); }
	}

    void gdiDrawSampleTexture(
        int x0, int y0, int x1, int y1, const _cTexture_* pT
    )
    {
        if (!pT) return;

        DWORD dwMode = SetStretchBltMode(
            GetDrawDC(), STRETCH_DELETESCANS
        );

        StretchDIBits(GetDrawDC(), x0, y0, x1 - x0, y1 - y0, 0x0, 0x0,
            pT->GetSize().x, pT->GetSize().y, pT->dwGetMemoryData(), pT->GetTextureData(), DIB_RGB_COLORS, SRCCOPY
        );

        SetStretchBltMode(
            GetDrawDC(), dwMode
        );
    }

	void gdiDrawWireFrameModel(const std::vector<vec2f>& vecPoints,
		float x, float y, float s = 1.f, float r = 0.f)
	{
		int verts = vecPoints.size();

		std::vector<vec2f>
			vecTransformedPoints;
		vecTransformedPoints.resize(verts);

		for (int i = 0x0; i < verts; i++) {
			vecTransformedPoints[i].x = vecPoints[i].x * cosf(r) - vecPoints[i].y * sinf(r);
			vecTransformedPoints[i].y = vecPoints[i].x * sinf(r) + vecPoints[i].y * cosf(r);
		}
		for (int i = 0x0; i < verts; i++) {
			vecTransformedPoints[i].x = vecTransformedPoints[i].x * s;
			vecTransformedPoints[i].y = vecTransformedPoints[i].y * s;
		}
		for (int i = 0x0; i < verts; i++) {
			vecTransformedPoints[i].x = vecTransformedPoints[i].x + x;
			vecTransformedPoints[i].y = vecTransformedPoints[i].y + y;
		}
		for (int i = 0x0; i < verts; i++) {
			int j = (i + 0x1);
			gdiDrawLine({ (int)vecTransformedPoints[i % verts].x, (int)vecTransformedPoints[i % verts].y },
				{ (int)vecTransformedPoints[j % verts].x, (int)vecTransformedPoints[j % verts].y });
		}
	}
	void gdiDrawPolygonModel(const std::vector<vec2f>& vecPoints,
		float x, float y, float s = 1.f, float r = 0.f)
	{
		int verts = vecPoints.size();

		std::vector<vec2f>
			vecTransformedPoints;
		vecTransformedPoints.resize(verts);

		for (int i = 0x0; i < verts; i++) {
			vecTransformedPoints[i].x = vecPoints[i].x * cosf(r) - vecPoints[i].y * sinf(r);
			vecTransformedPoints[i].y = vecPoints[i].x * sinf(r) + vecPoints[i].y * cosf(r);
		}
		for (int i = 0x0; i < verts; i++) {
			vecTransformedPoints[i].x = vecTransformedPoints[i].x * s;
			vecTransformedPoints[i].y = vecTransformedPoints[i].y * s;
		}
		for (int i = 0x0; i < verts; i++) {
			vecTransformedPoints[i].x = vecTransformedPoints[i].x + x;
			vecTransformedPoints[i].y = vecTransformedPoints[i].y + y;
		}

		std::vector<POINT>
			vecPolygonPoints;
		vecPolygonPoints.resize(verts);
		for(int i = 0x0; i < verts; i++)
			vecPolygonPoints[i] = POINT{
				(LONG)vecTransformedPoints[i].x,
				(LONG)vecTransformedPoints[i].y
			};

		Polygon(GetDrawDC(),
			vecPolygonPoints.data(), verts
		);
	}

	void gdiDrawString(int x, int y,
		const std::wstring& s, UINT format, COLORREF col
    )
	{
		HFONT hFont = CreateFont(16, 9, 0x0, 0x0, 300, 0x0, 0x0, 0x0,
			ANSI_CHARSET, OUT_RASTER_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, NULL
        );
		(void)DeleteFont(
            reinterpret_cast<HFONT>(SelectFont(GetDrawDC(), hFont))
        );
		SetTextColor(
            GetDrawDC(), col), SetBkMode(GetDrawDC(), TRANSPARENT
        );
		RECT rText = { x, y,
            GetResolution().x, GetResolution().y
		};
		DrawTextW(
            GetDrawDC(), s.data(), s.size(), &rText, format
        );
	}

	void ClearRect(
        const RECT& r
    ) { FillRect(m_hDC, &r, WHITE_BRUSH); }
};

#endif //_TRIVIAL_GDI_H_
