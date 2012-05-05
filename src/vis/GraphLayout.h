#ifndef _GRAPH_LAYOUT_H_
#define _GRAPH_LAYOUT_H_


#include <string>
#include <stdlib.h>
#include <time.h>

#include "Graph.h"

struct State
{
  float m_damping;
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
    static float coulomb_repulsion( GraphNode* p_this_node, 
    				    GraphNode * p_other_node);
    static float hooke_attraction(  GraphNode* p_this_node,
				    Spring * p_spring);
};

#endif
