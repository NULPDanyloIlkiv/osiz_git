#include "vec2.h"

vec2i::vec2i(void) : x(0x0), y(0x0) { /*Code...*/ }
vec2i::vec2i(int val) : x(val), y(val) { /*Code...*/ }
vec2i::vec2i(int _x, int _y) : x(_x), y(_y) { /*Code...*/ }

vec2i& vec2i::operator=(int val)
{ x = y = val; return (*this); }

vec2i::operator vec2f(void)
{ vec2f vec = { (float)x, (float)y }; return(vec); }



vec2i vec2i::operator+(vec2i const& vec)
{ return vec2i(x + vec.x, y + vec.y); }

vec2i vec2i::operator-(vec2i const& vec)
{ return vec2i(x - vec.x, y - vec.y); }

vec2i vec2i::operator*(vec2i const& vec)
{ return vec2i(x * vec.x, y * vec.y); }

vec2i vec2i::operator/(vec2i const& vec)
{ return vec2i(x / vec.x, y / vec.y); }



vec2i& vec2i::operator+=(vec2i const& vec)
{ x += vec.x, y += vec.y; return (*this); }

vec2i& vec2i::operator-=(vec2i const& vec)
{ x -= vec.x, y -= vec.y; return (*this); }

vec2i& vec2i::operator*=(vec2i const& vec)
{ x *= vec.x, y *= vec.y; return (*this); }

vec2i& vec2i::operator/=(vec2i const& vec)
{ x /= vec.x, y /= vec.y; return (*this); }

vec2i vec2i::operator-(void)
{ vec2i vec = { -x, -y }; return vec; }

bool vec2i::operator==(vec2i const& vec)
{ return(x == vec.x && y == vec.y); }
bool vec2i::operator!=(vec2i const& vec)
{ return(x != vec.x && y != vec.y); }



vec2f::vec2f(void) : x(0.f), y(0.f) { /*Code...*/ }
vec2f::vec2f(float val) : x(val), y(val) { /*Code...*/ }
vec2f::vec2f(float _x, float _y) : x(_x), y(_y) { /*Code...*/ }

vec2f& vec2f::operator=(float val)
{ x = y = val; return (*this); }

vec2f::operator vec2i(void)
{ vec2i vec = { (int)x, (int)y }; return vec; }



vec2f vec2f::operator+(vec2f const& vec)
{ return vec2f(x + vec.x, y + vec.y); }

vec2f vec2f::operator-(vec2f const& vec)
{ return vec2f(x - vec.x, y - vec.y); }

vec2f vec2f::operator*(vec2f const& vec)
{ return vec2f(x * vec.x, y * vec.y); }

vec2f vec2f::operator/(vec2f const& vec)
{ return vec2f(x / vec.x, y / vec.y); }



vec2f& vec2f::operator+=(vec2f const& vec)
{ x += vec.x, y += vec.y; return (*this); }

vec2f& vec2f::operator-=(vec2f const& vec)
{ x -= vec.x, y -= vec.y; return (*this); }

vec2f& vec2f::operator*=(vec2f const& vec)
{ x *= vec.x, y *= vec.y; return (*this); }

vec2f& vec2f::operator/=(vec2f const& vec)
{ x /= vec.x, y /= vec.y; return (*this); }

vec2f vec2f::operator-(void)
{ vec2f vec = { -x, -y }; return vec; }



bool vec2f::operator==(vec2f const& vec)
{ return(x == vec.x && y == vec.y); }
bool vec2f::operator!=(vec2f const& vec)
{ return(x != vec.x && y != vec.y); }
