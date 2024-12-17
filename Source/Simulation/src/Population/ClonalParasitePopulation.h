/*
 * ClonalParasitePopulation.h
 *
 * Define the ClonalParasitePopulation class which allows a single parasite
 * colony to be tracked in an individual.
 */
#ifndef CLONAL_PARASITE_POPULATION_H
#define CLONAL_PARASITE_POPULATION_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Helpers/UniqueId.hxx"
#include "Population/ParasiteDensity/ParasiteDensityUpdateFunction.h"
#include "Properties/IndexHandler.hxx"
#include "Therapies/DrugType.h"

class Genotype;
class SingleHostClonalParasitePopulations;
class Therapy;

class ClonalParasitePopulation : public IndexHandler {
  OBJECTPOOL(ClonalParasitePopulation);

  DELETE_COPY_AND_MOVE(ClonalParasitePopulation)

  PROPERTY_HEADER(double, last_update_log10_parasite_density)

  PROPERTY_HEADER(double, gametocyte_level)

  PROPERTY_REF(int, first_date_in_blood)

  POINTER_PROPERTY(SingleHostClonalParasitePopulations, parasite_population)

  POINTER_PROPERTY_HEADER(Genotype, genotype)

  POINTER_PROPERTY(ParasiteDensityUpdateFunction, update_function)

private:
  ul_uid _uid;

public:
  static constexpr double LOG_ZERO_PARASITE_DENSITY = -1000;

public:
  explicit ClonalParasitePopulation(Genotype* genotype = nullptr);

  ~ClonalParasitePopulation() override;

  double get_current_parasite_density(const int &current_time);

  [[nodiscard]] double get_log10_relative_density() const;

  [[nodiscard]] bool resist_to(const int &drug_id) const;

  void update();

  void perform_drug_action(const double &percent_parasite_remove);

  [[nodiscard]] ul_uid get_uid() const { return _uid; }
};

#endif
