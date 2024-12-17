/*
 * ParasiteUpdateFunction.h
 *
 * Define the parasite update function interface.
 */
#ifndef PARASITEDENSITYUPDATEFUNCTION_H
#define PARASITEDENSITYUPDATEFUNCTION_H

#include "Core/PropertyMacro.h"

class ClonalParasitePopulation;

class ParasiteDensityUpdateFunction {
  DELETE_COPY_AND_MOVE(ParasiteDensityUpdateFunction)

public:
  ParasiteDensityUpdateFunction() = default;

  virtual ~ParasiteDensityUpdateFunction() = default;

  virtual double get_current_parasite_density(
      ClonalParasitePopulation* parasite, int duration) = 0;
};

#endif
