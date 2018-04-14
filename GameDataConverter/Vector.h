#pragma once
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

template <class T>
class Vector
{
public:
  Vector() :x(0), y(0) {}
  Vector(T X, T Y) :x(X), y(Y) {}
  Vector(const Vector<T> &other) :x(other.x), y(other.y) {}

  Vector<T> operator-() const { return Vector<T>(-x, -y); }

  Vector<T> &operator=(const Vector<T> &other) { x = other.x; y = other.y; return *this; }

  Vector<T> operator+(const Vector<T> &other) const { return Vector<T>(x + other.x, y + other.y); }
  Vector<T> &operator+=(const Vector<T> &other) { x += other.x; y += other.y; return *this; }
  Vector<T> operator+(const T &v) const { return Vector<T>(x + v, y + v); }
  Vector<T> &operator+=(const T &v) { x += v; y += v; return *this; }

  Vector<T> operator-(const Vector<T> &other) const { return Vector<T>(x - other.x, y - other.y); }
  Vector<T> &operator-=(const Vector<T> &other) { x -= other.x; y -= other.y; return *this; }
  Vector<T> operator-(const T &v) const { return Vector<T>(x - v, y - v); }
  Vector<T> &operator-=(const T &v) { x -= v; y -= v; return *this; }

  T operator*(const Vector<T> &other) const { return x * other.x + y * other.y; }
  Vector<T> operator*(const T &v) const { return Vector<T>(x * v, y * v); }
  Vector<T> &operator*=(const T &v) { x *= v; y *= v; return *this; }

  Vector<T> operator/(const T &v) const { return Vector<T>(x / v, y / v); }
  Vector<T> &operator/=(const T &v) { x /= v; y /= v; return *this; }

  bool operator==(const Vector<T>& other) const { return x == other.x && y == other.y; }
  bool operator!=(const Vector<T>& other) const { return x != other.x || y != other.y; }

  Vector<T> &set(T X, T Y) { x = X; y = Y; return *this; }

  T getLength() const { return std::sqrt(x*x + y*y); }
  T getLengthSQ() const { return x*x + y*y; }

  Vector<T>& normalize()
  {
    double length = (double)(x*x + y*y);
    if (length == 0.0) return *this;
    length = 1.0 / sqrt(length);
    x = (T)(x * length);
    y = (T)(y * length);
    return *this;
  }
  Vector<T>& rotate(const double &angle)
  {
    const double sr = sin(angle * M_PI / 180.0);
    const double cr = cos(angle * M_PI / 180.0);
    T temp = (T)(x*cr - y*sr);
    y = (T)(x*sr + y*cr);
    x = temp;
    return *this;
  }

  T x, y;
};