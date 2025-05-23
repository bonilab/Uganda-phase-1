/*
 * MainDataCollector.cpp
 *
 * Implementation of the model data collector class.
 */
#include "MainDataCollector.h"

#include <algorithm>
#include <cmath>
#include <numeric>

#include "Constants.h"
#include "Core/Config/Config.h"
#include "Model.h"
#include "Population/ImmuneSystem.h"
#include "Population/Person.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "Therapies/SCTherapy.h"

#define Vector_by_Locations(template) \
  template(Model::CONFIG->number_of_locations(), 0)
#define DoubleMatrix_Locations_by_AgeClasses()        \
  DoubleVector2(Model::CONFIG->number_of_locations(), \
                DoubleVector(Model::CONFIG->number_of_age_classes(), 0.0))
#define IntMatrix_Locations_by_AgeClasses()        \
  IntVector2(Model::CONFIG->number_of_locations(), \
             IntVector(Model::CONFIG->number_of_age_classes(), 0))
#define IntMatrix_Locations_by_Therapies()         \
  IntVector2(Model::CONFIG->number_of_locations(), \
             IntVector(Model::CONFIG->therapy_db().size(), 0))

// Fill the vector indicated with zeros, this should compile into a memset call
// which is faster than a loop
#define zero_fill(vector) std::fill(vector.begin(), vector.end(), 0)

MainDataCollector::MainDataCollector(Model* model)
    : model_(model),
      AMU_per_parasite_pop_(0),
      AMU_per_person_(0),
      AMU_for_clinical_caused_parasite_(0),
      AFU_(0),
      discounted_AMU_per_parasite_pop_(0),
      discounted_AMU_per_person_(0),
      discounted_AMU_for_clinical_caused_parasite_(0),
      discounted_AFU_(0),
      current_number_of_mutation_events_(0) {}

void MainDataCollector::initialize() {
  // TODO: Determine if the _age_group_by_5_ variables can be removed or not.

  // TODO: Determine if this can ever actually happen for the MDC
  if (model_ == nullptr) { return; }

  births_by_location_ = Vector_by_Locations(IntVector);
  deaths_by_location_ = Vector_by_Locations(IntVector);
  malaria_deaths_by_location_ = Vector_by_Locations(IntVector);
  malaria_deaths_by_location_age_class_ = IntMatrix_Locations_by_AgeClasses();

  popsize_residence_by_location_ = Vector_by_Locations(IntVector);

  blood_slide_prevalence_by_location_ = Vector_by_Locations(DoubleVector);
  blood_slide_prevalence_by_location_age_group_ =
      DoubleMatrix_Locations_by_AgeClasses();
  blood_slide_number_by_location_age_group_ =
      DoubleMatrix_Locations_by_AgeClasses();
  blood_slide_prevalence_by_location_age_group_by_5_ =
      DoubleMatrix_Locations_by_AgeClasses();
  blood_slide_number_by_location_age_group_by_5_ =
      DoubleMatrix_Locations_by_AgeClasses();
  fraction_of_positive_that_are_clinical_by_location_ =
      Vector_by_Locations(DoubleVector);
  popsize_by_location_hoststate_ =
      IntVector2(Model::CONFIG->number_of_locations(),
                 IntVector(Person::NUMBER_OF_STATE, 0));
  popsize_by_location_age_class_ = IntMatrix_Locations_by_AgeClasses();
  popsize_by_location_age_class_by_5_ = IntMatrix_Locations_by_AgeClasses();

  total_immune_by_location_ = Vector_by_Locations(DoubleVector);
  total_immune_by_location_age_class_ = DoubleMatrix_Locations_by_AgeClasses();

  total_number_of_bites_by_location_ = Vector_by_Locations(LongVector);
  total_number_of_bites_by_location_year_ = Vector_by_Locations(LongVector);
  person_days_by_location_year_ = Vector_by_Locations(LongVector);

  EIR_by_location_year_ =
      DoubleVector2(Model::CONFIG->number_of_locations(), DoubleVector());
  EIR_by_location_ = Vector_by_Locations(DoubleVector);

  cumulative_clinical_episodes_by_location_ = Vector_by_Locations(LongVector);

  average_number_biten_by_location_person_ =
      DoubleVector2(Model::CONFIG->number_of_locations(), DoubleVector());
  percentage_bites_on_top_20_by_location_ = Vector_by_Locations(DoubleVector);

  cumulative_mutants_by_location_ = Vector_by_Locations(IntVector);

  AMU_per_parasite_pop_ = 0;
  AMU_per_person_ = 0;
  AMU_for_clinical_caused_parasite_ = 0;

  AFU_ = 0;

  discounted_AMU_per_parasite_pop_ = 0;
  discounted_AMU_per_person_ = 0;
  discounted_AMU_for_clinical_caused_parasite_ = 0;

  discounted_AFU_ = 0;

  multiple_of_infection_by_location_ =
      IntVector2(Model::CONFIG->number_of_locations(),
                 IntVector(NUMBER_OF_REPORTED_MOI, 0));

  current_EIR_by_location_ = Vector_by_Locations(DoubleVector);
  last_update_total_number_of_bites_by_location_ =
      Vector_by_Locations(LongVector);

  last_10_blood_slide_prevalence_by_location_ = DoubleVector2(
      Model::CONFIG->number_of_locations(), DoubleVector(10, 0.0));
  last_10_fraction_positive_that_are_clinical_by_location_ = DoubleVector2(
      Model::CONFIG->number_of_locations(), DoubleVector(10, 0.0));
  last_10_fraction_positive_that_are_clinical_by_location_age_class_ =
      DoubleVector3(Model::CONFIG->number_of_locations(),
                    DoubleVector2(Model::CONFIG->number_of_age_classes(),
                                  DoubleVector(10, 0.0)));
  last_10_fraction_positive_that_are_clinical_by_location_age_class_by_5_ =
      DoubleVector3(Model::CONFIG->number_of_locations(),
                    DoubleVector2(Model::CONFIG->number_of_age_classes(),
                                  DoubleVector(10, 0.0)));

  total_parasite_population_by_location_ = Vector_by_Locations(IntVector);
  number_of_positive_by_location_ = Vector_by_Locations(IntVector);

  total_parasite_population_by_location_age_group_ =
      IntMatrix_Locations_by_AgeClasses();
  number_of_positive_by_location_age_group_ =
      IntMatrix_Locations_by_AgeClasses();
  number_of_clinical_by_location_age_group_ =
      IntMatrix_Locations_by_AgeClasses();
  number_of_clinical_by_location_age_group_by_5_ =
      IntMatrix_Locations_by_AgeClasses();
  number_of_death_by_location_age_group_ = IntMatrix_Locations_by_AgeClasses();

  monthly_number_of_treatment_by_location_ = Vector_by_Locations(IntVector);
  monthly_number_of_treatment_by_location_age_class_ =
      IntMatrix_Locations_by_AgeClasses();
  monthly_number_of_treatment_by_location_therapy_ =
      IntMatrix_Locations_by_Therapies();
  monthly_number_of_new_infections_by_location_ =
      Vector_by_Locations(IntVector);
  monthly_number_of_clinical_episode_by_location_ =
      Vector_by_Locations(IntVector);
  monthly_number_of_clinical_episode_by_location_age_class_ =
      IntMatrix_Locations_by_AgeClasses();
  monthly_nontreatment_by_location_ = Vector_by_Locations(IntVector);
  monthly_nontreatment_by_location_age_class_ =
      IntMatrix_Locations_by_AgeClasses();

  monthly_treatment_complete_by_location_therapy_ =
      IntMatrix_Locations_by_Therapies();
  monthly_treatment_failure_by_location_ = Vector_by_Locations(IntVector);
  monthly_treatment_failure_by_location_age_class_ =
      IntMatrix_Locations_by_AgeClasses();
  monthly_treatment_failure_by_location_therapy_ =
      IntMatrix_Locations_by_Therapies();
  monthly_treatment_success_by_location_ = Vector_by_Locations(IntVector);
  monthly_treatment_success_by_location_age_class_ =
      IntMatrix_Locations_by_AgeClasses();
  monthly_treatment_success_by_location_therapy_ =
      IntMatrix_Locations_by_Therapies();

  current_number_of_mutation_events_ = 0;
  number_of_mutation_events_by_year_ = LongVector();
}

void MainDataCollector::perform_population_statistic() {
  // Start by zeroing out the population statistics from the previous month
  zero_population_statistics();

  // Get the pointer to work with
  auto* pi =
      Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();

  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    // Calculate the basic statistics for the population in this location
    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      for (auto ac = 0ul; ac < Model::CONFIG->number_of_age_classes(); ac++) {
        std::size_t size = pi->vPerson()[loc][hs][ac].size();

        popsize_by_location_hoststate_[loc][hs] += static_cast<int>(size);
        popsize_by_location_age_class_[loc][ac] += static_cast<int>(size);

        for (std::size_t i = 0; i < size; i++) {
          Person* p = pi->vPerson()[loc][hs][ac][i];
          popsize_residence_by_location_[p->residence_location()]++;

          // this immune value will include maternal immunity value of the
          // infants
          double immune_value = p->immune_system()->get_latest_immune_value();
          total_immune_by_location_[loc] += immune_value;
          total_immune_by_location_age_class_[loc][ac] += immune_value;

          int ac1 = (p->age() > 70) ? 14 : p->age() / 5;
          popsize_by_location_age_class_by_5_[loc][ac1] += 1;

          if (hs == Person::ASYMPTOMATIC) {
            number_of_positive_by_location_[loc]++;
            number_of_positive_by_location_age_group_[loc][ac] += 1;

            if (p->has_detectable_parasite()) {
              blood_slide_prevalence_by_location_[loc] += 1;
              blood_slide_number_by_location_age_group_[loc][ac] += 1;
              blood_slide_number_by_location_age_group_by_5_[loc][ac1] += 1;
            }

          } else if (hs == Person::CLINICAL) {
            number_of_positive_by_location_[loc]++;
            number_of_positive_by_location_age_group_[loc][ac] += 1;
            blood_slide_prevalence_by_location_[loc] += 1;
            blood_slide_number_by_location_age_group_[loc][ac] += 1;
            blood_slide_number_by_location_age_group_by_5_[loc][ac1] += 1;
            number_of_clinical_by_location_age_group_[loc][ac] += 1;
            number_of_clinical_by_location_age_group_by_5_[loc][ac1] += 1;
          }

          // Calculate the multiple of infection (MOI)
          int moi = p->all_clonal_parasite_populations()->size();
          if (moi > 0) {
            total_parasite_population_by_location_[loc] += moi;
            total_parasite_population_by_location_age_group_[loc]
                                                            [p->age_class()] +=
                moi;
            if (moi <= NUMBER_OF_REPORTED_MOI) {
              multiple_of_infection_by_location_[loc][moi - 1]++;
            }
          }
        }
      }
    }

    fraction_of_positive_that_are_clinical_by_location_[loc] =
        (blood_slide_prevalence_by_location_[loc] == 0)
            ? 0
            : static_cast<double>(
                  popsize_by_location_hoststate_[loc][Person::CLINICAL])
                  / blood_slide_prevalence_by_location_[loc];

    auto location_population =
        static_cast<double>(Model::POPULATION->size(loc));
    const auto number_of_blood_slide_positive =
        blood_slide_prevalence_by_location_[loc];
    blood_slide_prevalence_by_location_[loc] =
        blood_slide_prevalence_by_location_[loc] / location_population;

    current_EIR_by_location_[loc] =
        static_cast<double>(
            total_number_of_bites_by_location_[loc]
            - last_update_total_number_of_bites_by_location_[loc])
        / location_population;

    last_update_total_number_of_bites_by_location_[loc] =
        total_number_of_bites_by_location_[loc];

    auto report_index =
        (Model::SCHEDULER->current_time() / Model::CONFIG->report_frequency())
        % 10;
    last_10_blood_slide_prevalence_by_location_[loc][report_index] =
        blood_slide_prevalence_by_location_[loc];
    last_10_fraction_positive_that_are_clinical_by_location_
        [loc][report_index] =
            fraction_of_positive_that_are_clinical_by_location_[loc];

    for (std::size_t ac = 0; ac < Model::CONFIG->number_of_age_classes();
         ac++) {
      last_10_fraction_positive_that_are_clinical_by_location_age_class_
          [loc][ac][report_index] =
              (blood_slide_prevalence_by_location_age_group_[loc][ac] == 0)
                  ? 0
                  : number_of_clinical_by_location_age_group_[loc][ac]
                        / static_cast<double>(
                            blood_slide_prevalence_by_location_age_group_[loc]
                                                                         [ac]);

      last_10_fraction_positive_that_are_clinical_by_location_age_class_by_5_
          [loc][ac][report_index] =
              (number_of_blood_slide_positive == 0)
                  ? 0
                  : number_of_clinical_by_location_age_group_by_5_[loc][ac]
                        / number_of_blood_slide_positive;

      blood_slide_prevalence_by_location_age_group_[loc][ac] =
          blood_slide_number_by_location_age_group_[loc][ac]
          / static_cast<double>(popsize_by_location_age_class_[loc][ac]);
      blood_slide_prevalence_by_location_age_group_by_5_[loc][ac] =
          blood_slide_number_by_location_age_group_by_5_[loc][ac]
          / static_cast<double>(popsize_by_location_age_class_by_5_[loc][ac]);
    }
  }
}

void MainDataCollector::collect_number_of_bites(const int &location,
                                                const int &number_of_bites) {
  if (!recording) { return; }
  total_number_of_bites_by_location_[location] += number_of_bites;
  total_number_of_bites_by_location_year_[location] += number_of_bites;
}

void MainDataCollector::yearly_update() {
  if (Model::SCHEDULER->current_time()
      == Model::CONFIG->start_collect_data_day()) {
    for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
      person_days_by_location_year_[loc] =
          Model::POPULATION->size(loc) * Constants::DAYS_IN_YEAR();
    }
  } else if (Model::SCHEDULER->current_time()
             > Model::CONFIG->start_collect_data_day()) {
    for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
      // Make sure the divisor is not zero before calculating the EIR
      auto eir =
          (person_days_by_location_year_[loc] == 0)
              ? 0
              : (static_cast<double>(
                     total_number_of_bites_by_location_year_[loc])
                 / static_cast<double>(person_days_by_location_year_[loc]))
                    * Constants::DAYS_IN_YEAR();
      EIR_by_location_year_[loc].push_back(eir);

      // this number will be changed whenever a birth or a death occurs
      //  and also when the individual change location
      person_days_by_location_year_[loc] =
          Model::POPULATION->size(loc) * Constants::DAYS_IN_YEAR();
      total_number_of_bites_by_location_year_[loc] = 0;
    }
    if (Model::SCHEDULER->current_time()
        >= Model::CONFIG->start_of_comparison_period()) {
      number_of_mutation_events_by_year_.push_back(
          current_number_of_mutation_events_);
      current_number_of_mutation_events_ = 0;
    }
  }
}

void MainDataCollector::update_person_days_by_years(const int &location,
                                                    const int &days) {
  if (!recording) { return; }
  person_days_by_location_year_[location] += days;
}

void MainDataCollector::calculate_eir() {
  // Check to see if we should be collecting data or not, this will help avoid
  // divide-by-zero errors when determining the total time in year
  if (Model::SCHEDULER->current_time()
      < Model::CONFIG->start_collect_data_day()) {
    zero_fill(EIR_by_location_);
    return;
  }

  for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    if (EIR_by_location_year_[loc].empty()) {
      // Collect data for less than 1 year, note that total_time_in_years maybe
      // <= 0 if the model time is still before collection should take place
      const auto total_time_in_years =
          (Model::SCHEDULER->current_time()
           - Model::CONFIG->start_collect_data_day())
          / Constants::DAYS_IN_YEAR();
      auto eir =
          (person_days_by_location_year_[loc] == 0)
              ? 0
              : (static_cast<double>(
                     total_number_of_bites_by_location_year_[loc])
                 / static_cast<double>(person_days_by_location_year_[loc]))
                    * Constants::DAYS_IN_YEAR();
      eir = eir / total_time_in_years;
      EIR_by_location_[loc] = eir;

    } else {
      double sum_eir = std::accumulate(EIR_by_location_year_[loc].begin(),
                                       EIR_by_location_year_[loc].end(), 0.0);
      auto number_of_0 = std::count(EIR_by_location_year_[loc].begin(),
                                    EIR_by_location_year_[loc].end(), 0);
      EIR_by_location_[loc] =
          (static_cast<double>(EIR_by_location_year_[loc].size() - number_of_0)
           == 0.0)
              ? 0.0
              : sum_eir
                    / static_cast<double>(EIR_by_location_year_[loc].size()
                                          - number_of_0);
    }
  }
}

void MainDataCollector::collect_1_clinical_episode(const int &location,
                                                   const int &age_class) {
  if (!recording) { return; }
  cumulative_clinical_episodes_by_location_[location]++;
  monthly_number_of_clinical_episode_by_location_[location]++;
  monthly_number_of_clinical_episode_by_location_age_class_[location]
                                                           [age_class]++;
}

void MainDataCollector::record_1_birth(const int &location) {
  births_by_location_[location]++;
}

void MainDataCollector::record_1_death(const int &location, const int &birthday,
                                       const int &number_of_times_bitten,
                                       const int &age_group) {
  if (!recording) { return; }
  deaths_by_location_[location]++;
  update_person_days_by_years(
      location,
      -(Constants::DAYS_IN_YEAR() - Model::SCHEDULER->current_day_in_year()));
  update_average_number_bitten(location, birthday, number_of_times_bitten);
  number_of_death_by_location_age_group_[location][age_group] += 1;
}

void MainDataCollector::record_1_infection(const int &location) {
  if (!recording) { return; }
  monthly_number_of_new_infections_by_location_[location]++;
}

void MainDataCollector::record_1_malaria_death(const int &location,
                                               const int &age_class) {
  if (!recording) { return; }
  malaria_deaths_by_location_[location]++;
  malaria_deaths_by_location_age_class_[location][age_class]++;
}

void MainDataCollector::update_average_number_bitten(
    const int &location, const int &birthday,
    const int &number_of_times_bitten) {
  const auto time_living_from_start_collect_data_day =
      (birthday < Model::CONFIG->start_collect_data_day())
          ? 1
          : Model::SCHEDULER->current_time() + 1
                - Model::CONFIG->start_collect_data_day();

  const auto average_bites =
      number_of_times_bitten
      / static_cast<double>(time_living_from_start_collect_data_day);
  average_number_biten_by_location_person_[location].push_back(average_bites);
}

void MainDataCollector::calculate_percentage_bites_on_top_20() {
  auto pi =
      Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      for (std::size_t ac = 0; ac < Model::CONFIG->number_of_age_classes();
           ac++) {
        for (auto p : pi->vPerson()[loc][hs][ac]) {
          // add to total average number bitten
          update_average_number_bitten(loc, p->birthday(),
                                       p->number_of_times_bitten());
        }
      }
    }
  }

  for (std::size_t location = 0;
       location < Model::CONFIG->number_of_locations(); location++) {
    std::sort(average_number_biten_by_location_person_[location].begin(),
              average_number_biten_by_location_person_[location].end(),
              std::greater<>());
    double total = 0;
    double t20 = 0;
    const auto size20 = static_cast<int>(std::round(
        static_cast<double>(
            average_number_biten_by_location_person_[location].size())
        / 100.0 * 20));

    for (std::size_t i = 0;
         i < average_number_biten_by_location_person_[location].size(); i++) {
      total += average_number_biten_by_location_person_[location][i];

      if (i <= size20) {
        t20 += average_number_biten_by_location_person_[location][i];
      }
    }
    percentage_bites_on_top_20_by_location_[location] =
        (total == 0) ? 0 : t20 / total;
  }
}

void MainDataCollector::record_1_non_treated_case(const int &location,
                                                  const int &age_class) {
  if (!recording) { return; }
  monthly_nontreatment_by_location_[location]++;
  monthly_nontreatment_by_location_age_class_[location][age_class]++;
}

void MainDataCollector::begin_time_step() {
  if (!recording
      && Model::SCHEDULER->current_time()
             >= Model::CONFIG->start_collect_data_day()) {
    recording = true;
  }
}

void MainDataCollector::record_1_treatment(const int &location,
                                           const int &age_class,
                                           const int &therapy_id) {
  if (!recording) { return; }
  monthly_number_of_treatment_by_location_[location] += 1;
  monthly_number_of_treatment_by_location_age_class_[location][age_class] += 1;
  monthly_number_of_treatment_by_location_therapy_[location][therapy_id] += 1;
}

void MainDataCollector::record_1_mutation(const int &location) {
  if (!recording) { return; }
  cumulative_mutants_by_location_[location] += 1;
  current_number_of_mutation_events_ += 1;
}

void MainDataCollector::record_1_treatment_failure_by_therapy(
    const int &location, const int &age_class, const int &therapy_id) {
  if (!recording) { return; }
  monthly_treatment_complete_by_location_therapy_[location][therapy_id]++;
  monthly_treatment_failure_by_location_[location]++;
  monthly_treatment_failure_by_location_age_class_[location][age_class]++;
  monthly_treatment_failure_by_location_therapy_[location][therapy_id]++;
}

void MainDataCollector::record_1_treatment_success_by_therapy(
    const int &location, const int &age_class, const int &therapy_id) {
  if (!recording) { return; }
  monthly_treatment_complete_by_location_therapy_[location][therapy_id]++;
  monthly_treatment_success_by_location_[location]++;
  monthly_treatment_success_by_location_age_class_[location][age_class]++;
  monthly_treatment_success_by_location_therapy_[location][therapy_id]++;
}

void MainDataCollector::update_after_run() {
  perform_population_statistic();

  calculate_eir();
  calculate_percentage_bites_on_top_20();
}

[[maybe_unused]] void MainDataCollector::record_AMU_AFU(
    Person* person, Therapy* therapy,
    ClonalParasitePopulation* clinical_caused_parasite) {
  if (Model::SCHEDULER->current_time()
      >= Model::CONFIG->start_of_comparison_period()) {
    auto sc_therapy = dynamic_cast<SCTherapy*>(therapy);
    if (sc_therapy != nullptr) {
      const auto art_id = sc_therapy->get_artemisinin_id();
      if (art_id != -1 && sc_therapy->drug_ids.size() > 1) {
        const auto number_of_drugs_in_therapy = sc_therapy->drug_ids.size();
        const auto discounted_fraction =
            exp(log(0.97)
                * floor(static_cast<double>(
                            Model::SCHEDULER->current_time()
                            - Model::CONFIG->start_of_comparison_period())
                        / Constants::DAYS_IN_YEAR()));

        // combine therapy
        for (std::size_t i = 0; i < number_of_drugs_in_therapy; i++) {
          int drug_id = sc_therapy->drug_ids[i];
          if (drug_id != art_id) {
            // only check for the remaining chemical drug != artemisinin
            const std::size_t parasite_population_size =
                person->all_clonal_parasite_populations()->size();

            auto found_amu = false;
            auto found_afu = false;
            for (std::size_t j = 0ul; j < parasite_population_size; j++) {
              ClonalParasitePopulation* bp =
                  person->all_clonal_parasite_populations()->parasites()->at(j);

              if (bp->resist_to(drug_id) && !bp->resist_to(art_id)) {
                found_amu = true;

                AMU_per_parasite_pop_ +=
                    sc_therapy->get_max_dosing_day()
                    / static_cast<double>(parasite_population_size);
                discounted_AMU_per_parasite_pop_ +=
                    discounted_fraction * sc_therapy->get_max_dosing_day()
                    / static_cast<double>(parasite_population_size);
                if (bp == clinical_caused_parasite) {
                  AMU_for_clinical_caused_parasite_ +=
                      sc_therapy->get_max_dosing_day();
                  discounted_AMU_for_clinical_caused_parasite_ +=
                      discounted_fraction * sc_therapy->get_max_dosing_day();
                }
              }

              if (bp->resist_to(drug_id) && bp->resist_to(art_id)) {
                found_afu = true;
              }
            }
            if (found_amu) {
              AMU_per_person_ += sc_therapy->get_max_dosing_day();
              discounted_AMU_per_person_ +=
                  discounted_fraction * sc_therapy->get_max_dosing_day();
            }

            if (found_afu) {
              AFU_ += sc_therapy->get_max_dosing_day();
              discounted_AFU_ +=
                  discounted_fraction * sc_therapy->get_max_dosing_day();
            }
          }
        }
      }
    }
  }
}

double MainDataCollector::get_blood_slide_prevalence(const int &location,
                                                     const int &age_from,
                                                     const int &age_to) {
  double blood_slide_numbers = 0;
  double pop_size = 0;

  if (age_from < 10) {
    if (age_to <= 10) {
      for (int ac = age_from; ac <= age_to; ac++) {
        blood_slide_numbers +=
            blood_slide_number_by_location_age_group_[location][ac];
        pop_size += popsize_by_location_age_class_[location][ac];
      }
    } else {
      for (int ac = age_from; ac <= 10; ac++) {
        blood_slide_numbers +=
            blood_slide_number_by_location_age_group_[location][ac];
        pop_size += popsize_by_location_age_class_[location][ac];
      }
      int ac = 10;
      while (ac < age_to) {
        blood_slide_numbers +=
            blood_slide_number_by_location_age_group_by_5_[location][ac / 5];
        pop_size += popsize_by_location_age_class_by_5_[location][ac / 5];
        ac += 5;
      }
    }
  } else {
    int ac = age_from;

    while (ac < age_to) {
      blood_slide_numbers +=
          blood_slide_number_by_location_age_group_by_5_[location][ac / 5];
      pop_size += popsize_by_location_age_class_by_5_[location][ac / 5];
      ac += 5;
    }
  }
  return (pop_size == 0) ? 0 : blood_slide_numbers / pop_size;
}

// this function is used to reset monthly variables
void MainDataCollector::monthly_update() {
  // Clear variables needed by reporters
  zero_fill(births_by_location_);
  zero_fill(deaths_by_location_);

  // Clear variables that start after the data collection day
  if (Model::SCHEDULER->current_time()
      > Model::CONFIG->start_collect_data_day()) {
    zero_fill(monthly_number_of_treatment_by_location_);
    zero_fill(monthly_number_of_new_infections_by_location_);
    zero_fill(monthly_number_of_clinical_episode_by_location_);
    zero_fill(monthly_nontreatment_by_location_);
    zero_fill(malaria_deaths_by_location_);
    zero_fill(monthly_treatment_failure_by_location_);
    zero_fill(monthly_treatment_success_by_location_);

    for (auto location = 0ul; location < Model::CONFIG->number_of_locations();
         location++) {
      zero_fill(monthly_nontreatment_by_location_age_class_[location]);
      zero_fill(malaria_deaths_by_location_age_class_[location]);
      zero_fill(
          monthly_number_of_clinical_episode_by_location_age_class_[location]);
      zero_fill(monthly_number_of_treatment_by_location_therapy_[location]);
      zero_fill(monthly_number_of_treatment_by_location_age_class_[location]);
      zero_fill(monthly_treatment_complete_by_location_therapy_[location]);
      zero_fill(monthly_treatment_failure_by_location_age_class_[location]);
      zero_fill(monthly_treatment_failure_by_location_therapy_[location]);
      zero_fill(monthly_treatment_success_by_location_age_class_[location]);
      zero_fill(monthly_treatment_success_by_location_therapy_[location]);
    }
  }
}

void MainDataCollector::zero_population_statistics() {
  // Vectors to be zeroed
  zero_fill(popsize_residence_by_location_);
  zero_fill(blood_slide_prevalence_by_location_);
  zero_fill(fraction_of_positive_that_are_clinical_by_location_);
  zero_fill(total_immune_by_location_);
  zero_fill(total_parasite_population_by_location_);
  zero_fill(number_of_positive_by_location_);

  // Matrices based on number of locations to be zeroed
  for (auto location = 0ul; location < Model::CONFIG->number_of_locations();
       location++) {
    zero_fill(popsize_by_location_hoststate_[location]);
    zero_fill(total_immune_by_location_age_class_[location]);
    zero_fill(total_parasite_population_by_location_age_group_[location]);
    zero_fill(number_of_positive_by_location_age_group_[location]);
    zero_fill(number_of_clinical_by_location_age_group_[location]);
    zero_fill(number_of_clinical_by_location_age_group_by_5_[location]);
    zero_fill(popsize_by_location_age_class_[location]);
    zero_fill(popsize_by_location_age_class_by_5_[location]);
    zero_fill(blood_slide_prevalence_by_location_age_group_[location]);
    zero_fill(blood_slide_number_by_location_age_group_[location]);
    zero_fill(blood_slide_prevalence_by_location_age_group_by_5_[location]);
    zero_fill(blood_slide_number_by_location_age_group_by_5_[location]);
    zero_fill(multiple_of_infection_by_location_[location]);
  }
}
