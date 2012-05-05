#include "GraphFactory.h"

Graph * GraphFactory::createGraph(AdjacencyList * p_adj_list)
{
  Graph * graph = new Graph();
  
  for(int i = 0; i < p_adj_list->size(); i++)
  {
    /* Get the pair that we're currently looking at */
    AdjacentPair * pair = p_adj_list->pull(i);
    /* We'll have an origin and a destination node */
    GraphNode * orig_node = NULL;
    GraphNode * dest_node = NULL;
    
    /* Create a new node if we don't have one yet */
    if(graph->m_index_map.count(pair->m_origin) == 0)
    {
      orig_node = new GraphNode();
      orig_node->m_name = pair->m_origin;
      graph->m_nodes.push_back(orig_node);
      graph->m_index_map[pair->m_origin] = graph->m_nodes.size()-1;
     }
    else
      orig_node = graph->m_nodes[ graph->m_index_map[pair->m_origin] ];
    
    if(graph->m_index_map.count(pair->m_destination) == 0)
    {
      dest_node = new GraphNode();
      dest_node->m_name = pair->m_destination;
      graph->m_nodes.push_back(dest_node);
      graph->m_index_map[pair->m_destination] = graph->m_nodes.size()-1;
    }
    else
      dest_node = graph->m_nodes[ graph->m_index_map[pair->m_destination] ];
      
    /* At this point we're guaranteed to have the correct references */
    orig_node->m_adjacent.push_back(dest_node);
    dest_node->m_adjacent.push_back(orig_node);
  }  
    
  return graph;
}
