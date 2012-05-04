
#include <GL/glut.h>
#include <sys/time.h>
#include <iostream>
#include <unistd.h>

#include "TsvReader.h"
#include "GraphFactory.h"
#include "GraphLayout.h"

Graph * m_graph;
State * m_state;
bool    m_stepping;
float   m_last_frame;

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
  glPushMatrix();
    glTranslatef(node->m_position.X, 
		 node->m_position.Y, 
		 node->m_position.Z);
    glutSolidCube(1.0f);   
  glPopMatrix();
  
  for(int i = 0; i < node->m_adjacent.size(); i++)
  {
  //  RenderEdge(node, node->m_adjacent[i]);
  }  
}

void RenderGraph(Graph * graph)
{
  long current_time = GetTimeMillis();
  if(m_stepping)
    m_stepping = GraphLayout::step(graph, m_state,
				   current_time - m_last_frame);
  
  for(int i = 0; i < graph->m_nodes.size(); i++)
    RenderNode(graph->m_nodes[i]);
  
  m_last_frame = current_time;
}

void Display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  
    gluLookAt(1000, 1000, 1000,
  	     0, 0, 0,
	     0.0, 1.0, 0.0);
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

int main(int argc, char ** argv)
{
  TsvReader myReader;
  
  /* Read the graph from a file */
  AdjacencyList * list = myReader.readGraph("datasets/marvel_social.tsv");
  std::cout << "Graph Read... Converting" << std::endl;
  
  /* Convert the graph */
  m_graph = GraphFactory::createGraph(list);
  std::cout << "Graph Converted... Initialising OpenGL" << std::endl;
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(800, 600);   // Set the initial Window's width and height
  glutInitWindowPosition(50, 50); // Position the initial Window's top-left corner
  glutCreateWindow("Meatballs, Spaghetti and Cheese Sauce");  // Create window with the given title
  glutDisplayFunc(Display); 
  glutReshapeFunc(Reshape);
  
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  
  /* */
  std::cout << "OpenGL Initialised... Starting Simulation" << std::endl;
  m_state = GraphLayout::generateLayout(m_graph, 0.5f);
  
  m_last_frame = GetTimeMillis();
  
  glutMainLoop();
  
  return 0;
}
