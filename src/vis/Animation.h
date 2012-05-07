#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

enum Action
{
  INITIALISE,
  FRAME,
  ADD,
  LINK,
  EDGE,
  COLOR
};

struct RGB
{
  RGB() : R(0), G(0), B(0) {}
  RGB(char r, char g, char b) : R(r), G(g), B(b) {}
  char R;
  char G;
  char B;
};

struct Anim_State
{
  Anim_State(Action act) : m_action(act) {}
  
  Action m_action;
  
  /*Init State*/
  int m_number_nodes;
  
  /* Color State */
  std::string m_node;
  RGB *       m_node_color;
  
  /* Link State */
  std::string m_origin;
  std::string m_destination;
  
  RGB *       m_edge_color;
};

class Animation
{
private:
    std::vector<Anim_State> m_state_vector;
    int m_current_state;    

    static RGB * ColorToRGB(std::string p_color);
    Animation(std::vector<Anim_State> * p_vec);
    
public:
    Anim_State * nextState();
    static Animation * Read(std::string file);

    
};

#endif
