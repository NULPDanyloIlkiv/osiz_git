#ifndef _TRIVIAL_IMAGE_H_
#define _TRIVIAL_IMAGE_H_

#include "framework.h"

#include "texture.h"

#include "filter.h"

enum _eData_ {
    _SOURCE_ = 0x0
};

class _cImage_ {
    struct _sData_ {
        std::unique_ptr
            <_cTexture_> _T_;

        size_t _S_ = NULL;
    };

    std::vector<
        _sData_
    > _DATA_;

    size_t _INDEX_ = _SOURCE_;

public:
    _cImage_(
        const std::wstring wsFileName
    )
    {
        std::unique_ptr<_cTexture_> i = std::make_unique
            <_cTexture_>(wsFileName);

        size_t s = i->GetTextureData()->bmiHeader.biSizeImage;

        _DATA_.push_back(
            { std::move(i), s }
        );
    }



    size_t _SelectData_(size_t i) {
        if (
            i < _DATA_.size()
        ) { std::swap(_INDEX_, i); }
            return(i);        }

    _cTexture_* _Data_(void) {
        if (
            _DATA_.empty()
        ) { return(nullptr); }

        return(
            _DATA_[_INDEX_]._T_.get()
        );
    }
    _cTexture_* _Source_(void) {
        if (
            _DATA_.empty()
        ) { return(nullptr); }

        return(
            _DATA_[_SOURCE_]._T_.get()
        );
    }

    size_t _DataSize_(void) const {
        if (
            _DATA_.empty()
        ) { return(NULL); }

        return(
            _DATA_[_INDEX_]._S_
        );
    }
    size_t _SourceSize_(void) const {
        if (
            _DATA_.empty()
        ) { return(NULL); }

        return(
            _DATA_[_SOURCE_]._S_
        );
    }



    size_t _Index_(void) const {
        return(_INDEX_);
    }

    size_t _Size_(void) const {
        return(
            _DATA_.size()
        );
    }



    size_t _Dithering_RGB_(
        size_t _Nbit_
    )
    {
        if (
            _DATA_.empty()
        ) { return(_SOURCE_); }

        _Nbit_ = clamp(_Nbit_, 0x1, 0x8);

        std::unique_ptr<_cTexture_> i = std::make_unique
            <_cTexture_>(_DATA_[_SOURCE_]._T_.get());

        _QUANTISE_AND_DITHERISE_(
            _DATA_[_SOURCE_]._T_.get(), i.get(), _Nbit_
        );

        const BITMAPINFO* pBMI = i->GetTextureData();

        size_t s = pBMI->bmiHeader.biSizeImage;

        _DATA_.push_back(
            { std::move(i),
                s * (_Nbit_ / 8.f)
            }
        );

        return(
            _DATA_.size() - 0x1
        );
    }
    size_t _Quantisation_RGB_(
        size_t _Nbit_
    )
    {
        if (
            _DATA_.empty()
        ) { return(_SOURCE_); }

        _Nbit_ = clamp(_Nbit_, 0x1, 0x8);

        std::unique_ptr<_cTexture_> i = std::make_unique
            <_cTexture_>(_DATA_[_SOURCE_]._T_.get());

        _QUANTISE_(
            _DATA_[_SOURCE_]._T_.get(), i.get(), _Nbit_
        );

        const BITMAPINFO* pBMI = i->GetTextureData();

        size_t s = pBMI->bmiHeader.biSizeImage;

        _DATA_.push_back(
            { std::move(i),
                s * (_Nbit_ / 8.f)
            }
        );

        return(
            _DATA_.size() - 0x1
        );
    }
};

#endif // _TRIVIAL_IMAGE_H_
