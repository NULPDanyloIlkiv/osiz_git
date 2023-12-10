#pragma once
#ifndef _TRIVIAL_TEXTURE_H_
#define _TRIVIAL_TEXTURE_H_

#include "framework.h"

#ifndef BITS_PER_PIXEL
#define BITS_PER_PIXEL (32)
#endif

class RGBA;

class _cTexture_ {
    std::wstring m_wsFileName;

	struct _sTextureData_ {
		BITMAPINFO bmiTextureData;
		union {
			DWORD* dwMemory;
			BYTE* cbMemory;
		};
	} m_texture = { 0x0 };

	bool m_bIsValid = 0x0;

public:
	_cTexture_(
        UINT x, UINT y
    );
	_cTexture_(
        const std::wstring& sFileName
    );
	~_cTexture_(void);

	_cTexture_(const _cTexture_*);

    const wchar_t* GetFileName(void) const;



	bool IsValid(void) const;

	POINT GetSize(void) const;

	BYTE* cbGetMemoryData(void) const;
	DWORD* dwGetMemoryData(void) const;

	const BITMAPINFO* GetTextureData(void) const;

	RGBA GetColour(
		int x, int y
	) const;
	void SetColour(
		int x, int y, RGBA col
	);

	RGBA SampleColour(double x, double y) const;



	void ChangeFileName(
        const std::wstring& wsFileName
    );

private:
	DWORD Create32BppBitmap(
        UINT x, UINT y, _sTextureData_& textureData
    );
	DWORD Load32BppBitmap(
        const std::wstring& sFileName, _sTextureData_& textureData
    );
	DWORD Save32BppBitmap(
        const std::wstring& sFileName
    );
    DWORD Copy32BppBitmap(
        const _cTexture_* pT
    );
};

#endif //_TRIVIAL_TEXTURE_H_
