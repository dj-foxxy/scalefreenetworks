#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <map>

#include "GraphNode.h"

struct Graph
{
  std::map<std::string, int> m_index_map;
  std::vector<GraphNode*> m_nodes;
};

#endif
