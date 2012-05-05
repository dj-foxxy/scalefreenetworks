#ifndef _GRAPH_NODE_H_
#define _GRAPH_NODE_H_

#include "Vector3f.h"

struct GraphNode
{
  std::string m_name;
  std::vector<GraphNode*> m_adjacent;

  Vector3f m_position;
  Vector3f m_velocity;
  float    m_mass;
};

#endif
