/* 	
	Copyright Affonso Amendola 2019

	Distributed under GPLv3, use it to your hearts content,
	just remember the number one rule:

	Be Excellent to Each Other.
*/

#pragma once

//Forward Decs
class Point2;
class Point3;

class Vector2;
class Vector3;

//Vector2 is a (X,Y) pair of Double types
class Vector2
{
public:
	double x = 0.0f;
	double y = 0.0f;

	Vector2(double t_x = 0.0f,  double t_y = 0.0f);

	Vector2 operator+(const Vector2& v);
	void operator+=(const Vector2& v);
	
	Vector2 operator-();
	Vector2 operator-(const Vector2& v);
	void operator-=(const Vector2& v);

	Vector2 operator*(double s);
	void operator*=(double s);

	Vector2 operator/(double s);
	void operator/=(double s);

	explicit operator Point2();
	explicit operator Vector3();

	double cross(const Vector2& v);
	double dot(const Vector2& v);
	double abs();

	Vector2 unit();

	Vector2 rotate(double ang_rad);
};

//Vector3 is a (X,Y,Z) pairing of Double types
class Vector3
{
public:
	double x = 0.0f;
	double y = 0.0f;
	double z = 0.0f;

	Vector3(double t_x = 0.0f, double t_y = 0.0f, double t_z = 0.0f);

	Vector3 operator+(const Vector3& v);
	void operator+=(const Vector3& v);

	Vector3 operator-();
	Vector3 operator-(const Vector3& v);
	void operator-=(const Vector3& v);

	Vector3 operator*(double s);
	void operator*=(double s);

	Vector3 operator/(double s);
	void operator/=(double s);

	explicit operator Point3();
	
	Vector3 cross(const Vector3& v);
	double dot(const Vector3& v);
	double abs();

	Vector3 unit();
};
