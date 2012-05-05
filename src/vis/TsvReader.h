#ifndef _TSV_READER_H_
#define _TSV_READER_H_

#include <iostream>
#include <fstream>
#include <sstream>

#include "FileReader.h"

class TsvReader : public FileReader
{
public:
  virtual AdjacencyList * readGraph(std::string file);
};

#endif
