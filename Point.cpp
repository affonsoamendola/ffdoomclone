/* 	
	Copyright Affonso Amendola 2019

	Distributed under GPLv3, use it to your hearts content,
	just remember the number one rule:

	Be Excellent to Each Other.
*/

#include <cmath>

#include "Point.hpp"
#include "Vector.hpp"

Point2::Point2(	int t_x, int t_y)
				:x(t_x) , y(t_y) {}

//Basic operators Point2
//Sum
Point2 Point2::operator+(const Point2& v)
{
	Point2 new_p;

	new_p.x = this->x + v.x;
	new_p.y = this->y + v.y;

	return new_p;
}
void Point2::operator+=(const Point2& v)
{
	this->x += v.x;
	this->y += v.y;
}

//Sub / Neg
Point2 Point2::operator-()
{
	Point2 new_p;

	new_p.x = -(this->x);
	new_p.y = -(this->y);

	return new_p;
}
Point2 Point2::operator-(const Point2& v)
{
	Point2 new_p;

	new_p.x = this->x - v.x;
	new_p.y = this->y - v.y;

	return new_p; 
}
void Point2::operator-=(const Point2& v)
{
	this->x -= v.x;
	this->y -= v.y;
}

//Scalar Multiplication
Point2 Point2::operator*(int s)
{
	Point2 new_p;

	new_p.x = this->x * s;
	new_p.y = this->y * s;

	return new_p;
}
void Point2::operator*=(int s)
{
	this->x *= s;
	this->y *= s;
}

//Scalar Division
Point2 Point2::operator/(int s)
{
	Point2 new_p;

	new_p.x = this->x / s;
	new_p.y = this->y / s;

	return new_p;
}
void Point2::operator/=(int s)
{
	this->x /= s;
	this->y /= s;
}

//Point2 to Vector2 Conversion (Explicit)
Point2::operator Vector2()
{
	Vector2 new_v;

	new_v.x = static_cast<double>(this->x);
	new_v.y = static_cast<double>(this->y);

	return new_v;
} 

//Point2 to Point3 Conversion (Explicit)
//Z value is 0
Point2::operator Point3()
{
	Point3 new_p;

	new_p.x = this->x;
	new_p.y = this->y;
	new_p.z = 0;

	return new_p;
}

Point3::Point3(	int t_x, int t_y, int t_z)
					:x(t_x), y(t_y), z(t_z) {}

//Basic Operators Point3
//Sum
Point3 Point3::operator+(const Point3& v)
{
	Point3 new_p;

	new_p.x = this->x + v.x;
	new_p.y = this->y + v.y;
	new_p.z = this->z + v.z;

	return new_p;
}
void Point3::operator+=(const Point3& v)
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
}

//Sub / Neg
Point3 Point3::operator-()
{
	Point3 new_p;

	new_p.x = -(this->x);
	new_p.y = -(this->y);
	new_p.z = -(this->z);

	return new_p;
}
Point3 Point3::operator-(const Point3& v)
{
	Point3 new_p;

	new_p.x = this->x - v.x;
	new_p.y = this->y - v.y;
	new_p.z = this->z - v.z;

	return new_p; 
}
void Point3::operator-=(const Point3& v)
{
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
}

//Scalar Multiply
Point3 Point3::operator*(int s)
{
	Point3 new_p;

	new_p.x = this->x * s;
	new_p.y = this->y * s;
	new_p.z = this->z * s;

	return new_p;
}
void Point3::operator*=(int s)
{
	this->x *= s;
	this->y *= s;
	this->z *= s;
}

//Scalar Divide
Point3 Point3::operator/(int s)
{
	Point3 new_p;

	new_p.x = this->x / s;
	new_p.y = this->y / s;
	new_p.z = this->z / s;

	return new_p;
}
void Point3::operator/=(int s)
{
	this->x /= s;
	this->y /= s;
	this->z /= s;
}

//Point3 to Vector3 Conversion (Explicit)
Point3::operator Vector3()
{
	Vector3 new_v;

	new_v.x = static_cast<double>(this->x);
	new_v.y = static_cast<double>(this->y);
	new_v.z = static_cast<double>(this->z);

	return new_v;
} 