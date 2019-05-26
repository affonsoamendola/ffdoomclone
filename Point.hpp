/* 	
	Copyright Affonso Amendola 2019

	Distributed under GPLv3, use it to your hearts content,
	just remember the number one rule:

	Be Excellent to Each Other.
*/

#pragma once

//Forward Decs
class Vector2;
class Vector3;

class Point2;
class Point3;

//Point2 is a (X,Y) pair of Integer types
class Point2
{
public:
	int x = 0;
	int y = 0;

	Point2(int t_x = 0, int t_y = 0);

	Point2 operator+(const Point2& p);
	void operator+=(const Point2& p);

	Point2 operator-();
	Point2 operator-(const Point2& p);
	void operator-=(const Point2& p);

	Point2 operator*(int s);
	void operator*=(int s);

	Point2 operator/(int s);
	void operator/=(int s);

	explicit operator Point3();
	explicit operator Vector2();
};

//Point3 is a (X, Y, Z) pairing of Integer type
class Point3
{
public:
	int x = 0;
	int y = 0;
	int z = 0;

	Point3(int t_x = 0, int t_y = 0, int t_z = 0);

	Point3 operator+(const Point3& p);
	void operator+=(const Point3& p);

	Point3 operator-();
	Point3 operator-(const Point3& p);
	void operator-=(const Point3& p);

	Point3 operator*(int s);
	void operator*=(int s);

	Point3 operator/(int s);
	void operator/=(int s);

	explicit operator Vector3();
};