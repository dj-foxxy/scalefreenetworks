#ifndef _GRAPH_LAYOUT_H_
#define _GRAPH_LAYOUT_H_


#include <string>
#include <stdlib.h>
#include <time.h>

#include "Graph.h"
#include "Loader.h"

struct State
{
  float m_damping;
  float m_kinetic_energy;
  float m_time_step;
  float m_convergence;
};

struct Spring
{
};

class GraphLayout
{
public:
    static State * generateLayout(Graph * p_graph, float damping);
    static bool step(Graph * p_graph, State * &state, float time_step);

private:
    static Vector3f coulomb_repulsion( GraphNode* p_this_node, 
    				    GraphNode * p_other_node);
    static Vector3f hooke_attraction(  GraphNode* p_this_node,
				    GraphNode * p_adj_node);
};

#endif
