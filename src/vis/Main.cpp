
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>  

#include "TsvReader.h"
#include "GraphFactory.h"
#include "GraphLayout.h"
#include "Camera.h"
#include "Animation.h"

const int WIN_HEIGHT = 1024;
const int WIN_WIDTH  = 1280;

bool pre_defined_graph = false;
Graph 	  * m_graph;
State 	  * m_state;
Animation * m_animation;
bool    m_stepping;
float   m_last_update;
bool    m_show_edges = true;
bool    m_has_animation = true;

std::vector<Anim_State*> m_current_color_states;
std::vector<Anim_State*> m_current_edge_states;

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
  RGB * color = NULL;
  for(int i = 0; i < m_current_edge_states.size(); i++)
  {
     Anim_State * anim_state = m_current_edge_states[i];
     if(p_origin->m_name == anim_state->m_origin && p_destination->m_name == anim_state->m_destination
	 || p_origin->m_name == anim_state->m_destination && p_destination->m_name == anim_state->m_origin)
     {
        color = anim_state->m_edge_color;
     }
  }
  
  if(color != NULL)
  {
    glColor3f( color->R, color->G, color->B);
  }
  else if(!m_show_edges)
    return;
  else
    glColor3f( 0.3,0.2,1.0 );
  glBegin(GL_LINES);
    glVertex3f(p_origin->m_position.X,
	       p_origin->m_position.Y,
	       p_origin->m_position.Z);
    
    glVertex3f(p_destination->m_position.X,
	       p_destination->m_position.Y,
	       p_destination->m_position.Z);
  glEnd();
  glColor3f(0.0,1.0,1.0);
}

void RenderNode(GraphNode * node)
{
  glBegin(GL_POINTS);
    glVertex3f(node->m_position.X, 
		 node->m_position.Y, 
		 node->m_position.Z);
  glEnd();
  
  if(true)
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
  if(m_stepping)
    m_stepping = GraphLayout::step(graph, m_state,0.01f);
  
  for(int i = 0; i < graph->m_nodes.size(); i++)
  {
    RGB * color = NULL;
    for(int j = 0; j < m_current_color_states.size(); j++)
      if(m_current_color_states[j]->m_node == graph->m_nodes[i]->m_name)
	color = m_current_color_states[j]->m_node_color;
    
    if(color != NULL)
    {
      glColor3f((double)color->R / 255.0, (double)color->G / 255.0, (double)color->B /255.0);
    }
    else
      glColor3f(1.0,1.0,1.0);
    RenderNode(graph->m_nodes[i]);
  }
  
  std::stringstream oss;
  
  oss << "Time Step: "	    << m_state->m_time_step 	 << "\n"
      << "Damping:   " 	    << m_state->m_damping	 << "\n"
      << "Kinetic Energy: " << m_state->m_kinetic_energy << "\n"
      << "Converges At: "   << m_state->m_convergence    << "\n";
  
  RenderText(oss.str(), 10, 250);
//  RenderText("Hello2", 10, 280);
  
}

void NextAnimation(Graph * &p_graph, Animation * &anim)
{
  Anim_State * anim_state;
  
  //m_current_color_states.clear();
  m_current_edge_states.clear();
  std::stringstream oss;
  
  while((anim_state = anim->nextState()) != NULL && anim_state->m_action != FRAME)
  {
    if(anim_state->m_action == INITIALISE)
    {
      Anim_State * init_state = anim_state;
      for(int i = 0; i < init_state->m_number_nodes; i++)
      {
	GraphNode * node = new GraphNode();
	oss << p_graph->m_nodes.size();
	
	node->m_name = oss.str();
	node->m_position.X = p_graph->m_x[p_graph->m_nodes.size()];
	node->m_position.Y = p_graph->m_y[p_graph->m_nodes.size()];
	node->m_position.Z = p_graph->m_z[p_graph->m_nodes.size()];
	
	node->m_mass = 1.0f;
	
	p_graph->m_index_map[oss.str()] = p_graph->m_nodes.size();
        p_graph->m_nodes.push_back(node);
	    
	oss.str("");
      }
    } 
    else if(anim_state->m_action == ADD)
    {
	GraphNode * node = new GraphNode();
	oss << p_graph->m_nodes.size();
	
	node->m_name = oss.str();
	node->m_position.X = p_graph->m_x[p_graph->m_nodes.size()];
	node->m_position.Y = p_graph->m_y[p_graph->m_nodes.size()];
	node->m_position.Z = p_graph->m_z[p_graph->m_nodes.size()];
	
	
	node->m_mass = 1.0f;
	
	p_graph->m_index_map[oss.str()] = p_graph->m_nodes.size();
        p_graph->m_nodes.push_back(node);
	    
	oss.str("");
      m_stepping = true;
      
    }
    else if(anim_state->m_action == LINK)
    {
      Anim_State * link_state = anim_state;
       
      int origin = p_graph->m_index_map[link_state->m_origin];
      int destination = p_graph->m_index_map[link_state->m_destination];
      
      
      GraphNode * orig_node = p_graph->m_nodes[origin];
      GraphNode * dest_node = p_graph->m_nodes[destination];
      
      orig_node->m_adjacent.push_back(dest_node);
      dest_node->m_adjacent.push_back(orig_node);
    }
    else if(anim_state->m_action == COLOR)
    {
      m_current_color_states.push_back(anim_state);
      /*
      std::cout << "Found Color State " << std::endl;
      Color_State * clr_state = reinterpret_cast<Color_State*>(anim_state);
      std::cout << "BLAH" <<std::endl;
      std::cout << "ColorNode: ";
      std::cout << clr_state->m_node << std::endl;
      std::cout << "Found Color State " << std::endl;
      m_current_color_states.push_back(clr_state);
      std::cout << "Found Color State " << std::endl;
      */
    }
    else if(anim_state->m_action == EDGE)
    {
      m_current_edge_states.push_back(anim_state);
    }
  }
}

void Display()
{
  long current_time = GetTimeMillis();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  
    Camera::get()->update();
    
    if(!pre_defined_graph && current_time - m_last_update > 250)
    {
      NextAnimation(m_graph, m_animation);
      m_last_update = current_time;
    }
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
  AdjacencyList * list;
  
  
  m_animation = Animation::Read("animation/real.anim");
  
  
  
  if(pre_defined_graph)
  {
    std::cout << "Animation Read... Reading Graph" << std::endl;
    if( argc == 1 )
      list = myReader.readGraph("datasets/marvel_social_2.tsv");
    else
      list = myReader.readGraph(std::string(argv[1]));

    std::cout << "Graph Read... Converting Graph" << std::endl;

    /* Convert the graph */
    m_graph = GraphFactory::createGraph(list);
    std::cout << "Graph Converted... Initialising Layout" << std::endl;
  }
  else
  {
    bool m_blah = true;
    if(m_blah)
    {
      std::cout << "BLAH" << std::endl;
      list = myReader.readGraph("animation/edges");
      std::cout << "BLAH" << std::endl;
      m_graph = GraphFactory::createGraph(list);
      std::cout << "BLAH" << std::endl;
    }
    else
    {
      m_graph = new Graph();
      m_graph->m_size = 100;
    }
    std::cout << "Animation Read... Initialising Layout" << std::endl;
  }
  
  m_state = GraphLayout::generateLayout(m_graph, 0.5f);
  std::cout << "Layout Initialised... Initialising OpenGL" << std::endl;
  
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
  glPointSize(10.0f);
  
  
  glutCreateMenu(Menu);
  glutAddMenuEntry("Toggle Edges", 1);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  /* */
  std::cout << "OpenGL Initialised... Starting Simulation" << std::endl;
  
  m_last_update = GetTimeMillis();
  m_stepping = true;
  
  glutMainLoop();
  
  return 0;
}
