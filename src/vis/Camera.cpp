
#include "Camera.h"

#define INVERT_MOUSE 0

#define RUN_SPEED  1.0
#define TURN_ANGLE 4.0
#define DEG_TO_RAD 0.017453293

Camera::Camera(void)
{
}

Camera * Camera::get(void)
{
  static Camera instance;
  return &instance;
}

void Camera::init(void)
{
  glutKeyboardFunc(Keyboard);
  //glutMotionFunc(MouseMotion);
  
  lat = lon = 0.0f;
  mlat = mlon = 0.0f;
  eyex = eyey = eyez = 300.0f;
  centerx = centery = centerz = 1500.0f;
}

void Camera::calculate_lookpoint(void) { 
/*   Given an eyepoint and latitude and longitude angles, will
     compute a look point one unit away */

  GLdouble lonRad = (lon + mlon)*DEG_TO_RAD;
  GLdouble latRad = (lat + mlat)*DEG_TO_RAD;
  
  /*First calculate view direction*/
  GLdouble dir_x = cos(latRad) * sin(lonRad);
  GLdouble dir_y = sin(latRad);
  GLdouble dir_z = cos(latRad) * cos(lonRad);
  
  /*Now calculate the look point*/
  centerx = eyex + dir_x;
  centery = eyey + dir_y;
  centerz = eyez + dir_z;
} // calculate_lookpoint()

void Camera::update()
{
   calculate_lookpoint(); /* Compute the centre of interest   */
   gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, 0, 1, 0);
}

void Camera::mouse_motion(int x, int y) {

  mlon = -(1.0/WIN_WIDTH*100.0)*x + 50.0;
  mlat = -(1.0/WIN_HEIGHT*100.0)*y + 50.0;

} // mouse_motion()

void Camera::keyboard(unsigned char key, int x, int y) {
  
  GLfloat rad;//To be used by several methods
  
  switch (key) { 
    case ',': /*Comma Key*/
      rad = (lon+mlon+90) * DEG_TO_RAD;
      eyex += RUN_SPEED * sin(rad);
      eyez += RUN_SPEED * cos(rad);
      break;
    
    case '.': /*Full-Stop Key*/
      rad = (lon+mlon-90) * DEG_TO_RAD;
      eyex += RUN_SPEED * sin(rad);
      eyez += RUN_SPEED * cos(rad);
      break;
     
    /*Movement*/
    case 'w':
      rad = (mlon+lon) * DEG_TO_RAD;
      eyex += RUN_SPEED * sin(rad);
      eyey += RUN_SPEED * sin(rad);
      eyez += RUN_SPEED * cos(rad);
      break;
      
    case 's':
      rad = (mlon+lon) * DEG_TO_RAD;
      eyex -= RUN_SPEED * sin(rad);
      eyey -= RUN_SPEED * sin(rad);
      eyez -= RUN_SPEED * cos(rad);
      break; 
      
    case 'a':
      lon += TURN_ANGLE;
      break;
      
    case 'd':
      lon -= TURN_ANGLE;
      break;
      
    case 'r':
      lat += TURN_ANGLE;
      if(lat > 90)
	lat = 90;
      break;
      
    case 'f':
      lat -= TURN_ANGLE;
      if(lat < -90)
	lat = -90;
      break;
      
    case 27:  /* Escape key */
      exit(0);
      break;
 

   }
  
  switch (key) {
  }
} // keyboard()

void Camera::Keyboard(unsigned char c, int x, int y)
{
  Camera::get()->keyboard(c,x,y);
}

void Camera::MouseMotion(int x, int y)
{
  Camera::get()->mouse_motion(x,y);
}
