#ifndef _UTILS_H
#define _UTILS_H

#include <cmath>

template<typename T>
struct _Vector2
{
	T x;
	T y;

	_Vector2(const T& x, const T& y): x(x), y(y)
	{}

	_Vector2(const _Vector2<T>& other): x(other.x), y(other.y)
	{}

	
	double FastMagnitude() const
	{
		return static_cast<double>(x) * x +
			static_cast<double>(y) * y;
	}

	double magnitude() const { return std::sqrt(FastMagnitude()); }


	_Vector2& operator=(const _Vector2&) = default;
	/*_Vector2<T>& operator=(const _Vector2<T>& other)
	{
		if (this == &other) return *this;
		x = other.x; y = other.y;
		return *this;
	}*/

	bool operator==(const _Vector2& other) const
	{
		if constexpr (std::is_floating_point_v<T>)
		{
			constexpr T eps = static_cast<T>(0.0001);
			return std::abs(x - other.x) < eps &&
				std::abs(y - other.y) < eps;
		}
		else
		{
			return x == other.x && y == other.y;
		}
	}
	
};


using Vector2 = _Vector2<float>;
using Vector2Int = _Vector2<int>;
using Position = _Vector2<int>;



template<typename T>
struct _Transform2D
{
	T pos;
	T scale;
	float angle;

	_Transform2D(const T& pos, const T& scale, float angle = 0.0f) :
		pos(pos), scale(scale), angle(angle)
	{}
	_Transform2D(const _Transform2D<T>& other) : pos(other.pos), scale(other.scale), angle(other.angle)
	{}

};
using Transform2DINT = _Transform2D<Position>;
using Transform2DFLOAT = _Transform2D<Vector2>;


template<typename T>
struct _Rect
{
	T x1; T x2;
	T y1; T y2;

	_Rect(const T& x1, const T& y1, const T& x2, const T& y2):
		x1(x1), y1(y1), x2(x2), y2(y2)
	{}
	_Rect(const _Rect<T>& other) :
		x1(other.x1), y1(other.y1), x2(other.x2), y2(other.y2)
	{}

	bool IsIn(const _Vector2<T>& point)
	{
		return x1 < point.x && x2 > point.x && y1 < point.y && y2 > point.y;
	}

	bool IsOverlap(const _Rect<T>& rect)
	{
		return IsIn({ rect.x1, rect.y1 })
			|| IsIn({ rect.x1, rect.y2 })
			|| IsIn({ rect.x2, rect.y1 })
			|| IsIn({ rect.x2, rect.y2 });
	}

	T LengthX() const { return std::abs(x2 - x1); }
	T LengthY() const { return std::abs(y2 - y1); }

};
using RectINT = _Rect<int>;
using RectFLOAT = _Rect<float>;
#endif

