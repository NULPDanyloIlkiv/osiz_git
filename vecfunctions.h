#pragma once
#include <cmath>
#include <windows.h>

#include "vec2.h"

float clamp(float value, float min, float max);
float sign(float a);
float step(float edge, float x);

float length(vec2f const& v);

vec2i tovec(POINT const& p);
POINT topoint(vec2i const& v);
