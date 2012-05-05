#include "TsvReader.h"

AdjacencyList * TsvReader::readGraph(std::string file)
{
  /* Need an Adjacency list */
  AdjacencyList * adjList = new AdjacencyList();
  
  /* Open the file stream */
  std::ifstream in;
  in.open(file.c_str(), std::ifstream::in);
  in.unsetf(std::ios_base::skipws);
  /* Create a buffer character and a string stream*/
  std::stringstream oss[2];
  int  cur_stream = 0;
  char buffer;
  
  while(!in.eof())
  {
    in >> buffer;
    if(buffer == '\t')
    {
      cur_stream = 1;
    }
    else if(buffer == '\n')
    {
      if(oss[0].str() != "")
      {
        /* Push the strings into the adjacency list */
        adjList->push(oss[0].str(), oss[1].str());

      //  std::cout << oss[0].str() << " <--> " << oss[1].str() << std::endl;

        /* Reset the string streams */
        oss[0].str("");
        oss[1].str("");

        /* Set the current stream to be the source again */
        cur_stream = 0;
      }    
    }
    else
    {
      oss[cur_stream] << buffer;
    }    
  }
  
  in.close();
  
  return adjList;  
}
