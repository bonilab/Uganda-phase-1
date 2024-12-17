/*
 * SingleHostClonalParasitePopulations.h
 *
 * This class contains the points to all the clonal parasites that are
 * inflecting a single individual and defines the functions that are used to
 * calculate the force of infection.
 */
#ifndef SINGLE_HOST_CLONAL_PARASITE_POPULATIONS_H
#define SINGLE_HOST_CLONAL_PARASITE_POPULATIONS_H

#include <vector>

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"

class ClonalParasitePopulation;
class DrugsInBlood;
class DrugType;
class ParasiteDensityUpdateFunction;
class Person;

class SingleHostClonalParasitePopulations {
  OBJECTPOOL(SingleHostClonalParasitePopulations)

  DELETE_COPY_AND_MOVE(SingleHostClonalParasitePopulations)

  POINTER_PROPERTY(Person, person)

  POINTER_PROPERTY(std::vector<ClonalParasitePopulation*>, parasites)

  POINTER_PROPERTY(DoubleVector, relative_effective_parasite_density)

  // Total density of all parasites present in the host
  PROPERTY_REF(double, log10_total_relative_density);

private:
  int parasite_types = -1;

  void remove(const std::size_t &index);

public:
  explicit SingleHostClonalParasitePopulations(Person* person = nullptr);

  virtual ~SingleHostClonalParasitePopulations();

  void init();

  virtual int size();

  virtual void add(ClonalParasitePopulation* blood_parasite);

  virtual void add_all_infection_force();

  virtual void remove_all_infection_force();

  virtual void change_all_infection_force(const double &sign);

  [[nodiscard]] virtual int latest_update_time() const;

  virtual bool contain(ClonalParasitePopulation* blood_parasite);

  void change_all_parasite_update_function(
      ParasiteDensityUpdateFunction* from,
      ParasiteDensityUpdateFunction* to) const;

  void update() const;

  void clear_cured_parasites();

  void clear();

  void update_by_drugs(DrugsInBlood* drugs_in_blood) const;

  [[nodiscard]] bool has_detectable_parasite() const;

  void get_parasites_profiles(std::vector<double> &parasite_density,
                              double &log10_total_relative_density) const;

  void update_relative_effective_parasite_density_using_free_recombination();

  void update_relative_effective_parasite_density_without_free_recombination();

  [[nodiscard]] bool is_gametocytaemic() const;
};

#endif
