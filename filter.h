#ifndef _TRIVIAL_FILTER_H_
#define _TRIVIAL_FILTER_H_

#include "framework.h"



RGBA Quantise_RGB_NBit(
    RGBA const& in, size_t _Nbit_
)
{
    float fLevel = (0x1 << _Nbit_) - 0x1;

    uint8_t cr = uint8_t(
        clamp(std::round(float(in.Red) / 255.f * fLevel) / fLevel * 255.f, 0.f, 255.f)
    );
    uint8_t cg = uint8_t(
        clamp(std::round(float(in.Green) / 255.f * fLevel) / fLevel * 255.f, 0.f, 255.f)
    );
    uint8_t cb = uint8_t(
        clamp(std::round(float(in.Blue) / 255.f * fLevel) / fLevel * 255.f, 0.f, 255.f)
    );

    return(
        RGBA(cr, cg, cb)
    );
}



void _QUANTISE_(
    const _cTexture_* const pSource, _cTexture_* pDest, size_t _Nbit_
)
{
    wchar_t wcFileName[256] = { 0x0 };

    wsprintf(wcFileName,
        L"Q.%ibit.bmp", _Nbit_
    );

    pDest->ChangeFileName(wcFileName);

    DWORD* dwMemory = pDest->dwGetMemoryData();

    size_t nSize = pDest->GetSize().x * pDest->GetSize().y;

    for (
        int i = 0x0; i < nSize; i += 0x1
    )
    {
        dwMemory[i] = (COLORREF)Quantise_RGB_NBit(
            (COLORREF)dwMemory[i], _Nbit_
        );
    }
}



void _DITHER_FLOYD_STEINBERG_(
    _cTexture_* pDest, size_t _Nbit_
);

void _QUANTISE_AND_DITHERISE_(
    const _cTexture_* const pSource, _cTexture_* pDest, size_t _Nbit_
)
{
    wchar_t wcFileName[256] = { 0x0 };

    wsprintf(wcFileName,
        L"QandD.%ibit.bmp", _Nbit_
    );

    pDest->ChangeFileName(wcFileName);



    DWORD* dwMemory = pDest->dwGetMemoryData();

    size_t nSize = pDest->GetSize().x * pDest->GetSize().y;

    _DITHER_FLOYD_STEINBERG_(
        pDest, _Nbit_
    );
}

void _DITHER_FLOYD_STEINBERG_(
    _cTexture_* pDest, size_t _Nbit_
)
{
    if (!pDest) { return; }

    DWORD* dwMemory = pDest->dwGetMemoryData();

    POINT pPixel = {
        0x0, 0x0
    }, pSize = pDest->GetSize();

    for(pPixel.y = 0x0; pPixel.y < pSize.y; pPixel.y += 0x1) {
        for(pPixel.x = 0x0; pPixel.x < pSize.x; pPixel.x += 0x1) {
            RGBA op = pDest->GetColour(
                pPixel.x, pPixel.y
            );

            RGBA qp = Quantise_RGB_NBit(op, _Nbit_);

            int32_t error[0x3] = {
                op.Red - qp.Red, op.Green - qp.Green, op.Blue - qp.Blue
            };

            pDest->SetColour(
                pPixel.x, pPixel.y, qp
            );

            auto UpdatePixel = [&pDest, &pPixel, &error](
                vec2i const& vOffset, float fErrorBias
            )
            {
                RGBA p = pDest->GetColour(
                    pPixel.x + vOffset.x, pPixel.y + vOffset.y
                );
                int32_t k[0x3] = {
                    p.Red, p.Green, p.Blue
                };
                k[0x0] += (int32_t)(error[0x0] * fErrorBias);
                k[0x1] += (int32_t)(error[0x1] * fErrorBias);
                k[0x2] += (int32_t)(error[0x2] * fErrorBias);

                pDest->SetColour(pPixel.x + vOffset.x, pPixel.y + vOffset.y,
                    RGBA(
                        clamp(k[0x0], 0x0, 0xFF),
                        clamp(k[0x1], 0x0, 0xFF),
                        clamp(k[0x2], 0x0, 0xFF)
                    )
                );
            };

            UpdatePixel({ +0x1, 0x0 }, 7.f / 16.f);
            UpdatePixel({ -0x1, +0x1 }, 3.f / 16.f);
            UpdatePixel({ 0x0, +0x1 }, 5.f / 16.f);
            UpdatePixel({ +0x1, +0x1 }, 1.f / 16.f);
        }
    }
}

#endif // _TRIVIAL_FILTER_H_
