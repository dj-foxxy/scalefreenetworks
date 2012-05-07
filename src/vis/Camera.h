
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <cstdlib>
#include <cmath>
#include <GL/freeglut.h>

class Camera
{
private:
  GLdouble lat,     lon;              /* View angles (degrees)    */
  GLdouble mlat,    mlon;             /* Mouse look offset angles */   
  GLfloat  eyex,    eyey,    eyez;    /* Eye point                */
  GLfloat  centerx, centery, centerz; /* Look point               */
  static const int WIN_HEIGHT = 1280;
  static const int WIN_WIDTH  = 1024;
  bool m_next_state;
  
  Camera();
  
public:
  static Camera * get();
  
  bool NextState(void);
  void init(void);
  void calculate_lookpoint(void);
  void update(void);
  void mouse_motion(int x, int y);
  void keyboard(unsigned char key, int x, int y);
  
  static void Keyboard(unsigned char key, int x, int y);
  static void MouseMotion(int,int);
};

#endif
