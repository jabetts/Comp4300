#pragma once

struct Vec2
{
	float x = 0;
	float y = 0;

	Vec2();

	Vec2(float xin, float yin);

	bool operator == (const Vec2& rhs) const;
	bool operator != (const Vec2& rhs) const;

	Vec2 operator + (const Vec2& rhs) const;
	Vec2 operator - (const Vec2& rhs) const;
	Vec2 operator / (const Vec2& rhs) const;
	Vec2 operator * (const float val) const;

	Vec2 operator += (const Vec2& rhs);
	Vec2 operator -= (const Vec2& rhs);
	Vec2 operator *= (const Vec2& rhs);
	void operator /= (const float val);

	Vec2 normalize() const;
	float distSq(const Vec2& rhs) const;
	float dist(const Vec2& rhs) const;
	float length() const;
	float dot(Vec2 & rhs);
};