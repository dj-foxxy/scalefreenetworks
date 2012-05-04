#ifndef _VECTOR_3F_H_
#define _VECTOR_3F_H_

class Vector3f
{
public:
  float X, Y, Z;
  
  Vector3f() : X(0), Y(0), Z(0){}
  Vector3f(float x, float y, float z) : X(x), Y(y), Z(z) {}
};

#endif
