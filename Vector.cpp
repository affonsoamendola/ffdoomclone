/* 	
	Copyright Affonso Amendola 2019

	Distributed under GPLv3, use it to your hearts content,
	just remember the number one rule:

	Be Excellent to Each Other.
*/

#include <cmath>

#include "Vector.hpp"
#include "Point.hpp"

Vector2::Vector2(	double t_x, double t_y)
					:x(t_x) , y(t_y) {}

//Basic Operators
//Sum
Vector2 Vector2::operator+(const Vector2& v)
{
	Vector2 new_v;

	new_v.x = this->x + v.x;
	new_v.y = this->y + v.y;

	return new_v;
}
void Vector2::operator+=(const Vector2& v)
{
	this->x += v.x;
	this->y += v.y;
}

//Sub / Neg
Vector2 Vector2::operator-()
{
	Vector2 new_v;

	new_v.x = -(this->x);
	new_v.y = -(this->y);

	return new_v;
}
Vector2 Vector2::operator-(const Vector2& v)
{
	Vector2 new_v;

	new_v.x = this->x - v.x;
	new_v.y = this->y - v.y;

	return new_v; 
}
void Vector2::operator-=(const Vector2& v)
{
	this->x -= v.x;
	this->y -= v.y;
}

//Scalar Multiply
Vector2 Vector2::operator*(double s)
{
	Vector2 new_v;

	new_v.x = this->x * s;
	new_v.y = this->y * s;

	return new_v;
}
void Vector2::operator*=(double s)
{
	this->x *= s;
	this->y *= s;
}

//Scalar Divide
Vector2 Vector2::operator/(double s)
{
	Vector2 new_v;

	new_v.x = this->x / s;
	new_v.y = this->y / s;

	return new_v;
}
void Vector2::operator/=(double s)
{
	this->x /= s;
	this->y /= s;
}

//Vector2 -> Point2 Conversion (Explicit)
Vector2::operator Point2()
{
	Point2 new_p;

	new_p.x = static_cast<int>(this->x);
	new_p.y = static_cast<int>(this->y);

	return new_p;
} 

//Vector2 -> Vector3 Conversion (Explicit)
Vector2::operator Vector3()
{
	Vector3 new_v;

	new_v.x = this->x;
	new_v.y = this->y;
	new_v.z = 0.0f;

	return new_v;
} 

//Cross Product, since the cross product isn't really defined for 2 dimensional vectors,
//(It is supposed to be a Vect3 -> Vect3 operation that gives something orthogonal to both vectors)
//(Not really possible in 2 dimensions, but I digress.)
//We're using the standard extension of the definition of cross product to allow this.
//Which basically returns the length of the Vector3 that *would* exist if they were Vector3's 
double Vector2::cross(const Vector2& v) {return (this->x * v.y) - (this->y * v.x);}

//Dot product
double Vector2::dot(const Vector2& v) {return (this->x * v.x) + (this->y * v.y);}

//Abs / Length of the vector, using square root of dot product, Could use the sqr x*x+y*y, but,
//Its really the same thing.
double Vector2::abs()	{return sqrt(this->dot(*this));}

//Returns the Unitary vector version of this vector
Vector2 Vector2::unit()
{
	Vector2 new_v;

	new_v = (*this) / this->abs();

	return new_v;
}

//Rotates this vector around the origin by ang_rad degrees radian.
Vector2 Vector2::rotate(double ang_rad)
{
	Vector2 new_v;

	new_v.x = this->x * cos(ang_rad) - this->y * sin(ang_rad);
	new_v.y = this->x * sin(ang_rad) + this->y * cos(ang_rad);

	return new_v;
}

Vector3::Vector3(	double t_x, double t_y, double t_z)
					:x(t_x), y(t_y), z(t_z) {}

//Basic Operators
//Sum
Vector3 Vector3::operator+(const Vector3& v)
{
	Vector3 new_v;

	new_v.x = this->x + v.x;
	new_v.y = this->y + v.y;
	new_v.z = this->z + v.z;

	return new_v;
}
void Vector3::operator+=(const Vector3& v)
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
}

//Sub / Neg
Vector3 Vector3::operator-()
{
	Vector3 new_v;

	new_v.x = -(this->x);
	new_v.y = -(this->y);
	new_v.z = -(this->z);

	return new_v;
}
Vector3 Vector3::operator-(const Vector3& v)
{
	Vector3 new_v;

	new_v.x = this->x - v.x;
	new_v.y = this->y - v.y;
	new_v.z = this->z - v.z;

	return new_v; 
}
void Vector3::operator-=(const Vector3& v)
{
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
}

//Scalar Multiply
Vector3 Vector3::operator*(double s)
{
	Vector3 new_v;

	new_v.x = this->x * s;
	new_v.y = this->y * s;
	new_v.z = this->z * s;

	return new_v;
}
void Vector3::operator*=(double s)
{
	this->x *= s;
	this->y *= s;
	this->z *= s;
}

//Scalar Divide
Vector3 Vector3::operator/(double s)
{
	Vector3 new_v;

	new_v.x = this->x / s;
	new_v.y = this->y / s;
	new_v.z = this->z / s;

	return new_v;
}
void Vector3::operator/=(double s)
{
	this->x /= s;
	this->y /= s;
	this->z /= s;
}

//Cross Product
Vector3 Vector3::cross(const Vector3& v)
{
	Vector3 new_v;

	new_v.x = (this->y*v.z) - (this->z*v.y);
	new_v.y = (this->z*v.x) - (this->x*v.z);
	new_v.z = (this->x*v.y) - (this->y*v.x);

	return new_v;
}

//Dot Product
double Vector3::dot(const Vector3& v) {return (this->x * v.x) + (this->y * v.y) + (this->z * v.z);}

//Abs / Length of the vector
double Vector3::abs()	{return sqrt(this->dot(*this));}

//Returns the Unitary vector version of this vector
Vector3 Vector3::unit()
{
	Vector3 new_v;

	new_v = (*this) / this->abs();

	return new_v;
}

//Vector3 -> Point3 Conversion (Explicit)
Vector3::operator Point3()
{
	Point3 new_p;

	new_p.x = static_cast<int>(this->x);
	new_p.y = static_cast<int>(this->y);
	new_p.z = static_cast<int>(this->z);

	return new_p;
} 