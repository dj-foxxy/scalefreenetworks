#include "Animation.h"

Animation::Animation(std::vector<Anim_State> * p_vec) : m_state_vector(*p_vec),
					     		m_current_state(0)
{
}

Anim_State * Animation::nextState()
{
  if(m_current_state >= m_state_vector.size())
    return NULL;
  else
  {
     return &m_state_vector[m_current_state++];
  }
}

RGB * Animation::ColorToRGB(std::string p_color)
{
  if(p_color == "red")
    return new RGB(255,50,10);
  if(p_color == "white")
    return new RGB(255,255,255);
  if(p_color == "black")
    return new RGB(100,100,100);
  if(p_color == "green")
    return new RGB(0,255,0);
  if(p_color == "powderblue")
    return new RGB(100,100,255);
  if(p_color == "plum1")
    return new RGB(100,50,150);
  if(p_color == "grey")
    return new RGB(100,150,150);
  if(p_color == "yellow")
    return new RGB(170,0,170);
  
  return new RGB(255,255,0);
}

Animation * Animation::Read(std::string p_file)
{
  std::ifstream in;
  
  in.open(p_file.c_str(), std::ifstream::in);
  
  std::stringstream oss;
  char buffer;
  std::vector<Anim_State> * state_vector = 
       new std::vector<Anim_State>();
  
  while(!in.eof())
  {
    in >> buffer;
    if( 'I' == buffer )
    {
      Anim_State * init_state = new Anim_State(INITIALISE);
      in >> init_state->m_number_nodes;
  
 
      state_vector->push_back(*init_state);
    } 
    else if( 'L' == buffer )
    {
      Anim_State * link_state = new Anim_State(LINK);
      in >> link_state->m_origin >> link_state->m_destination;
      state_vector->push_back(*link_state);
    }
    else if( 'E' == buffer )
    {
      Anim_State * edge_state = new Anim_State(EDGE);
      std::string color;
      in >> edge_state->m_origin >> edge_state->m_destination >> color;
      edge_state->m_edge_color = ColorToRGB(color);
      state_vector->push_back(*edge_state);
    }
    else if( 'C' == buffer )
    {
      Anim_State * color_state = new Anim_State(COLOR);
      std::string color;
      in >> color_state->m_node >> color;
      color_state->m_node_color = ColorToRGB(color);
      state_vector->push_back(*color_state);
    }
    else if( 'F' == buffer )
      state_vector->push_back(Anim_State(FRAME));
    else if( 'A' == buffer )
      state_vector->push_back(Anim_State(ADD));
    else
      std::cout << "Not recognised: " << buffer << std::endl;
  }  
  
  return new Animation(state_vector);
  
}
