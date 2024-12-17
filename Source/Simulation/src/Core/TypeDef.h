/*
 * File:   TypeDef.h
 * Author: nguyentran
 *
 * Created on April 17, 2013, 10:17 AM
 */

#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <list>
#include <map>
#include <ostream>
#include <string>
#include <vector>

class Person;

class PersonIndex;

class Event;

class Reporter;

class Drug;

class IStrategy;

class Therapy;

using ul = unsigned long;

using DoubleVector = std::vector<double>;
using DoubleVector2 = std::vector<DoubleVector>;
using DoubleVector3 = std::vector<DoubleVector2>;
using IntVector = std::vector<int>;
using IntVectorPtr = std::vector<int>*;
using IntVector2 = std::vector<IntVector>;
using IntVector3 = std::vector<IntVector2>;
using IntVectorPtrVector = std::vector<IntVector*>;
using IntVector2Ptr = std::vector<IntVector>*;
using UIntVector = std::vector<unsigned int>;

using LongVector = std::vector<ul>;
using LongVector2 = std::vector<LongVector>;

using StringVector = std::vector<std::string>;
using StringVector2 = std::vector<StringVector>;

using IntIntMap = std::map<int, int>;

using PersonPtrVector = std::vector<Person*>;
using PersonPtrVectorIterator = PersonPtrVector::iterator;

using PersonPtrVector2 = std::vector<PersonPtrVector>;
using PersonPtrVector3 = std::vector<PersonPtrVector2>;
using PersonPtrVector4 = std::vector<PersonPtrVector3>;

using EventPtrVector = std::vector<Event*>;
using EventPtrVector2 = std::vector<EventPtrVector>;

using ReporterPtrVector = std::vector<Reporter*>;

using PersonIndexPtrList = std::list<PersonIndex*>;

using DrugPtrMap = std::map<int, Drug*>;

using TherapyPtrVector = std::vector<Therapy*>;
using StrategyPtrVector = std::vector<IStrategy*>;

struct ImmuneSystemInformation {
  double acquire_rate{-1};
  std::vector<double> acquire_rate_by_age;
  double decay_rate{-1};

  double duration_for_fully_immune{-1};
  double duration_for_naive{-1};

  double immune_inflation_rate{-1};

  double max_clinical_probability{-1};

  // Slope of the sigmoidal prob-v-immunity function, z-value
  double immune_effect_on_progression_to_clinical{-1};

  // The midpoint of the sigmoidal prob-v-immunity function, recommended default
  // 0.4
  double midpoint{-1};

  double c_min{-1};
  double c_max{-1};

  double alpha_immune{-1};
  double beta_immune{-1};

  double age_mature_immunity{-1};

  // Parameter kappa in supplement of 2015 LGH paper
  double factor_effect_age_mature_immunity{-1};
};

struct ParasiteDensityLevel {
  double log_parasite_density_cured;
  double log_parasite_density_from_liver;
  double log_parasite_density_asymptomatic;
  double log_parasite_density_clinical;
  double log_parasite_density_clinical_from;
  double log_parasite_density_clinical_to;
  double log_parasite_density_detectable;
  double log_parasite_density_detectable_pfpr;
  double log_parasite_density_pyrogenic;

  friend std::ostream &operator<<(std::ostream &os,
                                  const ParasiteDensityLevel &pdl) {
    os << "[" << pdl.log_parasite_density_cured << ","
       << pdl.log_parasite_density_from_liver << ","
       << pdl.log_parasite_density_asymptomatic << ","
       << pdl.log_parasite_density_clinical << ","
       << pdl.log_parasite_density_clinical_from << ","
       << pdl.log_parasite_density_clinical_to << ","
       << pdl.log_parasite_density_detectable << ","
       << pdl.log_parasite_density_detectable_pfpr << ","
       << pdl.log_parasite_density_pyrogenic << "]";
    return os;
  }
};

// This structure is a hook so we can convert rasters to location_db
struct RasterDb {
  friend std::ostream &operator<<(std::ostream &os, const RasterDb &rdb) {
    os << "raster_db: ";
    return os;
  }
};

struct RelativeBittingInformation {
  double max_relative_biting_value;
  int number_of_biting_levels;

  //  biting_level_distribution:
  //  #  distribution: Exponential
  //    distribution: Gamma
  //    Exponential:
  double scale;

  double mean;
  double sd;
  DoubleVector v_biting_level_value;
  DoubleVector v_biting_level_density;
};

struct RelativeMovingInformation {
  double max_relative_moving_value;
  int number_of_moving_levels;

  //  biting_level_distribution:
  //  #  distribution: Exponential
  //    distribution: Gamma
  //    Exponential:
  double scale;

  double mean;
  double sd;
  DoubleVector v_moving_level_value;
  DoubleVector v_moving_level_density;

  double circulation_percent;
  double length_of_stay_mean;
  double length_of_stay_sd;
  double length_of_stay_theta;
  double length_of_stay_k;
  double relative_probability_that_child_travels_compared_to_adult{1.0};
  double relative_probability_for_clinical_to_travel{1.0};
};

struct InitialParasiteInfo {
  int location;
  int parasite_type_id;
  double prevalence;

  InitialParasiteInfo()
      : location(-1), parasite_type_id(-1), prevalence(-1.0){};

  InitialParasiteInfo(const int loc, const int p_type, const double pre)
      : location(loc), parasite_type_id(p_type), prevalence(pre){};
};

struct RelativeInfectivity {
  double sigma;
  double ro_star;

  friend std::ostream &operator<<(std::ostream &os,
                                  const RelativeInfectivity &e) {
    os << "[" << e.sigma << "," << e.ro_star << "]";
    return os;
  }
};

struct Allele {
  int value;  // we can do char later or map from char to int
  std::string name;
  std::string short_name;
  IntVector mutation_values;
  int mutation_level;
  double daily_cost_of_resistance;

  friend std::ostream &operator<<(std::ostream &os, const Allele &e) {
    os << e.short_name;
    return os;
  }
};

struct Locus {
  std::vector<Allele> alleles;
  int position{};

  friend std::ostream &operator<<(std::ostream &os, const Locus &l) {
    for (const auto &allele : l.alleles) { os << allele; }
    return os;
  }
};

struct GenotypeInfo {
  std::vector<Locus> loci_vector;

  friend std::ostream &operator<<(std::ostream &os,
                                  const GenotypeInfo &genotype_info) {
    for (const auto &loci : genotype_info.loci_vector) { os << loci; }
    return os;
  }
};

#endif /* TYPEDEF_H */
