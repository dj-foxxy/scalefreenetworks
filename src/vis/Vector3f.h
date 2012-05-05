#ifndef _VECTOR_3F_H_
#define _VECTOR_3F_H_

#include <cmath>

class Vector3f
{
public:
  float X, Y, Z;
  
  Vector3f() : X(0), Y(0), Z(0){}
  Vector3f(float x, float y, float z) : X(x), Y(y), Z(z) {}
 
  float magnitute()
  {
    return sqrt(X*X + Y*Y + Z*Z);
  }
  
  void normalise()
  {
    float mag = magnitute();
    X /= mag;
    Y /= mag;
    Z /= mag;
  }
  
  Vector3f operator-(const Vector3f &rhs)
  {
    return Vector3f(X - rhs.X, Y - rhs.Y, Z - rhs.Z);
  } 
 
  Vector3f operator+(const Vector3f &rhs)
  {
    return Vector3f(X + rhs.X, Y + rhs.Y, Z + rhs.Z);
  }
   
  Vector3f operator+(const float &rhs)
  {
    return Vector3f(X + rhs, Y + rhs, Z + rhs);
  }
  
  Vector3f operator*(const float &rhs)
  {
    return Vector3f(X * rhs, Y * rhs, Z * rhs);
  }
  
  Vector3f operator+=(const Vector3f &rhs)
  {
    X += rhs.X;
    Y += rhs.Y;
    Z += rhs.Z;
    
    return *this;
  }
  
  Vector3f operator+=(const float &rhs)
  {
    X += rhs;
    Y += rhs;
    Z += rhs;
    
    return *this;
  }
};

#endif
