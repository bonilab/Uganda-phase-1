/*
 * ClinicalUpdateFunction.h
 *
 * Define the update function for clinical infections.
 */
#ifndef CLINICALUPDATEFUNCTION_H
#define CLINICALUPDATEFUNCTION_H

#include "Core/PropertyMacro.h"
#include "ParasiteDensityUpdateFunction.h"

class Model;

class ClinicalUpdateFunction : public ParasiteDensityUpdateFunction {
  DELETE_COPY_AND_MOVE(ClinicalUpdateFunction)

  POINTER_PROPERTY(Model, model)

public:
  explicit ClinicalUpdateFunction(Model* model = nullptr);

  ~ClinicalUpdateFunction() override = default;

  double get_current_parasite_density(ClonalParasitePopulation* parasite,
                                      int duration) override;
};

#endif
