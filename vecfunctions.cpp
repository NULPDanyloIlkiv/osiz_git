#include "vecfunctions.h"

float clamp(float value, float min, float max)
{ return fmax(fmin(value, max), min); }

float sign(float a)
{ return (float)((0.f < a) - (a < 0.f)); }

float step(float edge, float x)
{ return (float)(x > edge); }



float length(vec2f const& v)
{ return (float)sqrtf((float)v.x * (float)v.x + (float)v.y * (float)v.y); }

vec2i tovec(POINT const& p)
{ return { p.x, p.y }; }

POINT topoint(vec2i const& v)
{ return { v.x, v.y }; }
