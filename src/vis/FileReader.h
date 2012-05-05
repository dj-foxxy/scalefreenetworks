#ifndef _FILE_READER_H_
#define _FILE_READER_H_

#include "AdjacencyList.h"

/* Interface file reader classes */
class FileReader
{
public:
  virtual AdjacencyList * readGraph(std::string file) = 0;
};

#endif
