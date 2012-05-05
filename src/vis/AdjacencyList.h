#ifndef _ADJACENCY_LIST_H_
#define _ADJACENCY_LIST_H_

#include <vector>
#include <string>

struct AdjacentPair
{
  std::string m_origin;
  std::string m_destination;
};

class AdjacencyList
{
  public:
       void push(std::string p_origin, std::string p_destination);
       std::vector<AdjacentPair> pull(std::string p_node);
       AdjacentPair * pull(int index);
       
       int count(std::string p_node);
       int size();
 
  private:
       std::vector<AdjacentPair> m_adj_pair;
};

#endif
