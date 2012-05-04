#ifndef _GRAPH_FACTORY_H_
#define _GRAPH_FACTORY_H_

#include <string>
#include <iostream>

#include "Graph.h"
#include "GraphNode.h"
#include "AdjacencyList.h"

class GraphFactory
{
public:
    static Graph * createGraph(AdjacencyList * p_adj_list);
};

#endif
