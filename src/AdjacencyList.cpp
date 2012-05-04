#include "AdjacencyList.h"


void AdjacencyList::push(std::string p_origin, std::string p_destination)
{
  AdjacentPair * pair = new AdjacentPair();
  pair->m_origin = p_origin;
  pair->m_destination = p_destination;
  m_adj_pair.push_back(*pair);
}

std::vector<AdjacentPair> AdjacencyList::pull(std::string p_node)
{
  std::vector<AdjacentPair> pair_vec;
  
  for(int i = 0; i < m_adj_pair.size(); i++)
    if(m_adj_pair[i].m_origin == p_node)
      pair_vec.push_back(m_adj_pair[i]);
  
  return pair_vec;
}

AdjacentPair *  AdjacencyList::pull(int index)
{
  return &m_adj_pair.at(index);
}

int AdjacencyList::count(std::string p_node)
{
  return 1;
//  return m_adj_pair.count(p_node);
}

int AdjacencyList::size()
{
  return m_adj_pair.size();
}
