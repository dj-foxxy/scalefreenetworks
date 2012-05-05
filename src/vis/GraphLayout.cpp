#include "GraphLayout.h"

State * GraphLayout::generateLayout(Graph * p_graph, float damping)
{
  State * state = new State();
  state->m_damping = damping;
  srand ( time(NULL) );
  
  for(int i = 0; i < p_graph->m_nodes.size(); i++)
  {
    GraphNode * this_node = p_graph->m_nodes[i];
    
    this_node->m_velocity.X = 0.0f;
    this_node->m_velocity.Y = 0.0f;
    this_node->m_velocity.Z = 0.0f;
    
    double jitter = 1000.0 / (double)(rand () % 1000);
    
    this_node->m_position.X = rand() % ( 1000 )
			     + jitter / (double)(2 + rand() % 4);
    this_node->m_position.Y = rand() % ( 1000 )
			     + jitter / (double)(2 + rand() % 4);
    this_node->m_position.Z = rand() %  ( 1000 )
			     + jitter / (double)(2 + rand() % 4);
  }
}

bool GraphLayout::step(Graph * p_graph, State * &state, float time_step)
{
  if(true) return false;
  
  float damping = state->m_damping;
  float total_kinetic_energy = 0.0f;
  
  for(int i = 0; i < p_graph->m_nodes.size(); i++)
  {
    Vector3f net_force;
    GraphNode * this_node = p_graph->m_nodes[i];
    
    for(int j = 0; j < p_graph->m_nodes.size(); j++)
    {
      if(j != i)
      {
	//net_force += coulomb_repulsion( this_node,
	  //  				p_graph->m_nodes[j]);
      }
    }    
    
    //For each spring connected to this node
    //net force += hooke_attraction( this_node, spring )
    
  //  this_node->m_velocity = 
	//(this_node->m_velocity + time_step * net_force) * damping;
    
  }  
  return false;
}
