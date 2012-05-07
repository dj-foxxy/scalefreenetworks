#include "Loader.h"

void Loader::StartLoading()
{
}

void Loader::SetProgress(float p_fraction)
{
  std::cout << "Loading " << p_fraction * 100.0 << "%\r" << std::flush;
}
