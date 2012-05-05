
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>

#include "TsvReader.h"
#include "GraphFactory.h"
#include "GraphLayout.h"
#include "Camera.h"

const int WIN_HEIGHT = 1024;
const int WIN_WIDTH  = 1280;

Graph * m_graph;
State * m_state;
bool    m_stepping;
float   m_last_frame;
bool    m_show_edges = false;

long GetTimeMillis()
{
    struct timeval start;

    long mtime, seconds, useconds;    

    gettimeofday(&start, NULL);
    
    seconds  = start.tv_sec;
    useconds = start.tv_usec;
    
    return ((seconds) * 1000 + useconds/1000.0) + 0.5;
}

void RenderEdge(GraphNode * p_origin, GraphNode * p_destination)
{
  glColor3f(0.0,1.0,0.0);
  glBegin(GL_LINES);
    glVertex3f(p_origin->m_position.X,
	       p_origin->m_position.Y,
	       p_origin->m_position.Z);
    
    glVertex3f(p_destination->m_position.X,
	       p_destination->m_position.Y,
	       p_destination->m_position.Z);
  glEnd();
  glColor3f(1.0,1.0,1.0);
}

void RenderNode(GraphNode * node)
{
  glBegin(GL_POINTS);
    glVertex3f(node->m_position.X, 
		 node->m_position.Y, 
		 node->m_position.Z);
  glEnd();
  
  if(m_show_edges)
  {
    for(int i = 0; i < node->m_adjacent.size(); i++)
    {
      RenderEdge(node, node->m_adjacent[i]);
    } 
  }  
}

void RenderText(std::string string, int x, int y)
{
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0.0, 800, 0.0, 800);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   
      glColor3f(0.1f, 0.2f, 1.0f); 
      glRasterPos2f(x, WIN_HEIGHT - y);

      glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)string.c_str());
    
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   
}

void RenderGraph(Graph * graph)
{
  long current_time = GetTimeMillis();
  if(m_stepping)
    m_stepping = GraphLayout::step(graph, m_state,
				   current_time - m_last_frame);
  
  for(int i = 0; i < graph->m_nodes.size(); i++)
    RenderNode(graph->m_nodes[i]);
  
  std::stringstream oss;
  
  oss << "Time Step: "	    << m_state->m_time_step 	 << "\n"
      << "Damping:   " 	    << m_state->m_damping	 << "\n"
      << "Kinetic Energy: " << m_state->m_kinetic_energy << "\n"
      << "Converges At: "   << m_state->m_convergence    << "\n";
  
  RenderText(oss.str(), 10, 250);
//  RenderText("Hello2", 10, 280);
  
  m_last_frame = current_time;
}

void Display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  
    Camera::get()->update();
//    gluLookAt(300,300,300,
 // 	     -4000, -3000, 400,
//	     0.0, 1.0, 0.0);
    RenderGraph(m_graph);
    
  glColor3f(1.0,1.0,1.0);
  glutSwapBuffers();
  
  //glFlush();
  glutPostRedisplay();
}



void Reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(80, 1, 0.001, 10000 );
   glMatrixMode(GL_MODELVIEW);
}

void Menu( int entry )
{
   if(entry == 1)
     m_show_edges = !m_show_edges;
}

int main(int argc, char ** argv)
{
  TsvReader myReader;
  
  /* Read the graph from a file */
  AdjacencyList * list = myReader.readGraph("datasets/marvel_social_2.tsv");
  std::cout << "Graph Read... Converting" << std::endl;
  
  /* Convert the graph */
  m_graph = GraphFactory::createGraph(list);
  std::cout << "Graph Converted... Initialising OpenGL" << std::endl;
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);   // Set the initial Window's width and height
  glutInitWindowPosition(50, 50); // Position the initial Window's top-left corner
  glutCreateWindow("Meatballs, Spaghetti and Cheese Sauce");  // Create window with the given title
  glutDisplayFunc(Display); 
  glutReshapeFunc(Reshape);
  
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  Camera::get()->init();
  glPointSize(3.0f);
  
  
  glutCreateMenu(Menu);
  glutAddMenuEntry("Toggle Edges", 1);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  /* */
  std::cout << "OpenGL Initialised... Starting Simulation" << std::endl;
  m_state = GraphLayout::generateLayout(m_graph, 0.5f);
  
  m_last_frame = GetTimeMillis();
  m_stepping = true;
  
  glutMainLoop();
  
  return 0;
}
