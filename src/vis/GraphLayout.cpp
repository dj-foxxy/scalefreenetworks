#include "GraphLayout.h"

#include <iostream>

#define CONSTANT 0.05

State * GraphLayout::generateLayout(Graph * p_graph, float damping)
{
  State * state = new State();
  state->m_damping = 0.2;//damping;
  state->m_kinetic_energy = 0.0f;
  state->m_convergence	  = 0.5f;
  state->m_time_step 	  = 0.01f;
  srand ( time(NULL) ); 
  
  int range = 250;
  int total = range * range * range;
  
  int * x = new int[total], 
      * y = new int[total], 
      * z = new int[total];
  
  int j = 0;
  for (int _x = 0; _x < range; _x++)
  {
    for(int _y = 0; _y < range; _y++)
    {
      for(int _z = 0; _z < range; _z++)
      {
        x[j] = _x; y[j] = _y; z[j] = _z;
        j++;
	
	//Loader::SetProgress( ((double)j / (double)total)/2.0 );
      }
    }
  }
  
  Loader::SetProgress(0.5f);
  /* Randomly Distribute Nodes */
  for(int i = 0; i < total; i++)
  {
    int _x, _y, _z; int rnd = rand() % (total);
    
    _x = x[i];
    _y = y[i];
    _z = z[i];
    
    x[i] = x[rnd];
    y[i] = y[rnd];
    z[i] = z[rnd];
    
    x[rnd] = _x;
    y[rnd] = _y;
    z[rnd] = _z;
    
   // Loader::SetProgress(10.0 + ((double)i / (double)total)/2.0);
  }  
  
  for(int i = 0; i < p_graph->m_nodes.size(); i++)
  {
    GraphNode * this_node = p_graph->m_nodes[i];
    
    this_node->m_velocity.X = 0.0f;
    this_node->m_velocity.Y = 0.0f;
    this_node->m_velocity.Z = 0.0f;
    
    double jitter = 1000.0 / (double)(rand () % 1000);
    
    this_node->m_position.X = x[i];
    this_node->m_position.Y = y[i];
    this_node->m_position.Z = z[i];
  }
  std::cout << std::endl;
  
  p_graph->m_x = x;
  p_graph->m_y = y;
  p_graph->m_z = z;
  
  return state;
}

bool GraphLayout::step(Graph * p_graph, State * &state, float time_step)
{
  float damping = state->m_damping;
  float total_kinetic_energy = 0.0f;
  time_step = 0.1;
  
  for(int i = 0; i < p_graph->m_nodes.size(); i++)
  {
    Vector3f net_force;
    GraphNode * this_node = p_graph->m_nodes[i];
    
    
    for(int j = 0; j < p_graph->m_nodes.size(); j++)
    {
      if(j != i)
      {
	net_force += coulomb_repulsion( this_node,
	    				p_graph->m_nodes[j]);
      }
    }    
    for(int j = 0; j < this_node->m_adjacent.size(); j++)
    {
      net_force += hooke_attraction( this_node, this_node->m_adjacent[j] );
    }
    
    float speed = this_node->m_velocity.magnitute();
    this_node->m_velocity = 
	(this_node->m_velocity + net_force * time_step) * damping;
    this_node->m_position += this_node->m_velocity * time_step;
    total_kinetic_energy += (speed * speed);
  }  
  
  state->m_kinetic_energy = total_kinetic_energy;
  
  return total_kinetic_energy > state->m_convergence;
}

Vector3f GraphLayout::coulomb_repulsion( GraphNode* p_this_node, 
        		  GraphNode * p_other_node)
{
  float q1q2 = CONSTANT;
  
  float k = CONSTANT;
  Vector3f d_ij = p_this_node->m_position - p_other_node->m_position;
  float d_ij_mag = d_ij.magnitute();
  
  d_ij.normalise();
  
  float scalar = k * (q1q2 / (d_ij_mag*d_ij_mag));
  return d_ij * scalar;
}

Vector3f GraphLayout::hooke_attraction( GraphNode* p_this_node,
				    GraphNode * p_adj_node)
{
  float k = CONSTANT;
  float r = CONSTANT;
  Vector3f d_ij = p_this_node->m_position - p_adj_node->m_position;
  float d_ij_mag = d_ij.magnitute();
  
  d_ij.normalise();
  float scalar = (-k * (d_ij_mag - r));
  
  return d_ij * scalar;
}
