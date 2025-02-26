/*
Copyright (C) 2008  Lukas Kropatschek, Mathias Gottschlag

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "core/Vector2D.h"

#include <math.h>

namespace peak
{
	
	Vector2D::Vector2D()
	{
		x = 0;
		y = 0;
	}
	
	Vector2D::Vector2D(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
	
	void Vector2D::rotate(float rotation)
	{
		float oldx = x;
		x = x * cos(rotation) + y * sin(rotation);
		y = y * cos(rotation) - oldx * sin(rotation);
	}
	
	Vector2D &Vector2D::operator*=(float scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}
	Vector2D &Vector2D::operator/=(float scale)
	{
		x /= scale;
		y /= scale;
		return *this;
	}
	Vector2D Vector2D::operator+(const Vector2D &v)
	{
		return Vector2D(x + v.x, y + v.y);
	}
	Vector2D &Vector2D::operator+=(Vector2D &v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector2D &Vector2D::operator-=(Vector2D &v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
}
