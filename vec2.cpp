#include <math.h>
#include "vec2.h"

Vec2::Vec2()
{
}

Vec2::Vec2(float xin, float yin)
	:x(xin), y(yin)
{
}

bool Vec2::operator==(const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
	
}

bool Vec2::operator!=(const Vec2& rhs) const
{
	return (x != rhs.x || y != rhs.y);
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator/(const Vec2& rhs) const
{
	return Vec2(x / rhs.x, y - rhs.y);
}

Vec2 Vec2::operator*(const float val) const
{
	return Vec2(x * val, y * val);
}

Vec2 Vec2::operator+=(const Vec2& rhs)
{
	return Vec2(x += rhs.x, y += rhs.y);
}

Vec2 Vec2::operator-=(const Vec2& rhs)
{
	return Vec2(x -= rhs.x, y -= rhs.y);
}

Vec2 Vec2::operator*=(const Vec2& rhs)
{
	return Vec2(x *= rhs.x, y *= rhs.y);
}

void Vec2::operator/=(const float val)
{
}

float Vec2::dist(const Vec2& rhs) const
{
	Vec2 d = Vec2(rhs.x - x, rhs.y - y);
	return sqrtf(d.x*d.x + d.y*d.y);
}

float Vec2::distSq(const Vec2& rhs) const
{
	Vec2 d = Vec2(rhs.x - x, rhs.y - y);
	return  (d.x * d.x + d.y * d.y);
}

float Vec2::length() const
{
	return sqrtf(x*x + y*y);
}

Vec2 Vec2::normalize() const
{ 
	float l = sqrt(x * x + y * y);

	return Vec2(x / l, y / l);
}

float Vec2::dot(Vec2 & rhs)
{
	return (x * rhs.x + y * rhs.y);
}
