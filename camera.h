#ifndef _TRIVIAL_CAMERA_H_
#define _TRIVIAL_CAMERA_H_

class _cCamera_ {

public:
	_cCamera_(void) {
	    vecOffset = {
	        0.f, 0.f
        };
	    fScale = 1.f;
    }
	_cCamera_(
		float fScale
	) : fScale(fScale)
	{
        vecOffset = {
	        0.f, 0.f
        };
	}
	~_cCamera_(void) { /*Code...*/ }

	vec2f vecOffset{};
	float fScale = 1.f;

	vec2i WorldToDisplay(float fWorldX, float fWorldY)
	{ return(vec2f{ fWorldX - vecOffset.x, fWorldY - vecOffset.y } *vec2f{ fScale, fScale }); }
	vec2f DisplayToWorld(float fDisplayX, float fDisplayY)
	{ return(vec2f{ fDisplayX, fDisplayY } / vec2f{ fScale, fScale } + vecOffset); }
};

#endif //_TRIVIAL_CAMERA_H_
