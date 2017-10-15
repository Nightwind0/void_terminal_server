#ifndef __PLANET_TOOLS_H_
#define __PLANET_TOOLS_H_
#include "ToolSet.h"

class PlanetTools : ToolSet {
 public:
  PlanetTools();
  virtual ~PlanetTools(){}

  double CalcPopulationAfter(double current_pop, double seconds_elapsed, double class_multiplier);
  double CalcPlasmaAfter(double population, double current, double seconds_elapsed, double class_multiplier);
  
};

#endif
