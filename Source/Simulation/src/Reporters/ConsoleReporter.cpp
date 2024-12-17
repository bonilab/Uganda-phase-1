/*
 * File:   ConsoleReporter.cpp
 * Author: Merlin
 *
 * Created on August 1, 2013, 12:15 PM
 */

#include "ConsoleReporter.h"

#include <iostream>

#include "Constants.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "MDC/MainDataCollector.h"
#include "Model.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "ReporterUtils.h"
#include "fmt/printf.h"

ConsoleReporter::ConsoleReporter() {}

ConsoleReporter::~ConsoleReporter() {}

void ConsoleReporter::before_run() {
  std::cout << "Seed:" << Model::RANDOM->seed() << std::endl;
}

void report_number_by_state(const int &location,
                            PersonIndexByLocationStateAgeClass* pi) {
  //    std::cout << std::setw(10) << std::setprecision(3);
  for (int hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
    //        int sum = 0;
    //        for (int ac = 0; ac < Model::CONFIG->number_of_age_classes();
    //        ac++) {
    //            sum += pi->vPerson()[location][hs][ac].size();
    //        }
    double v = Model::MAIN_DATA_COLLECTOR
                   ->popsize_by_location_hoststate()[location][hs]
               * 100 / (double)Model::POPULATION->size(location);
    //        double v = sum;

    fmt::printf("%.3f\t", v);
  }
}

void ConsoleReporter::after_run() {
  std::cout << "==============================================================="
               "==========="
            << std::endl;

  // total time
  double total_time_in_years = (Model::SCHEDULER->current_time()
                                - Model::CONFIG->start_collect_data_day())
                               / Constants::DAYS_IN_YEAR();
  std::cout << "Total time (from equilibrium) : " << total_time_in_years
            << " years" << std::endl;

  // report EIR
  std::cout << "EIR by location:" << std::endl;
  for (std::size_t location = 0;
       location < Model::CONFIG->number_of_locations(); location++) {
    std::cout << Model::MAIN_DATA_COLLECTOR->EIR_by_location()[location]
              << "\t";
  }
  std::cout << std::endl;

  // total number of bites
  std::cout << "Number of infectious bites:" << std::endl;
  for (std::size_t location = 0;
       location < Model::CONFIG->number_of_locations(); location++) {
    std::cout << Model::MAIN_DATA_COLLECTOR
                     ->total_number_of_bites_by_location()[location]
              << "\t";
  }
  std::cout << std::endl;

  std::cout << "Number of clinical episodes:" << std::endl;
  for (std::size_t location = 0;
       location < Model::CONFIG->number_of_locations(); location++) {
    std::cout << Model::MAIN_DATA_COLLECTOR
                     ->cumulative_clinical_episodes_by_location()[location]
              << "\t";
  }
  std::cout << std::endl;

  std::cout << "Percentage of bites on top 20% bitten" << std::endl;
  for (std::size_t location = 0;
       location < Model::CONFIG->number_of_locations(); location++) {
    std::cout << Model::MAIN_DATA_COLLECTOR
                         ->percentage_bites_on_top_20_by_location()[location]
                     * 100
              << "%"
              << "\t";
  }
  std::cout << std::endl;

  std::cout << "Number of mutations by location: " << std::endl;
  for (std::size_t location = 0;
       location < Model::CONFIG->number_of_locations(); location++) {
    std::cout << Model::MAIN_DATA_COLLECTOR
                     ->cumulative_mutants_by_location()[location]
              << "\t";
  }
  std::cout << std::endl;

  std::cout << "AMU per parasite population: "
            << Model::MAIN_DATA_COLLECTOR->AMU_per_parasite_pop() << std::endl;
  std::cout << "AMU per per: " << Model::MAIN_DATA_COLLECTOR->AMU_per_person()
            << std::endl;
  std::cout << "EAMU count only clinical caused parasite: "
            << Model::MAIN_DATA_COLLECTOR->AMU_for_clinical_caused_parasite()
            << std::endl;
}

void ConsoleReporter::begin_time_step() {}

void ConsoleReporter::monthly_report() {
  if (Model::SCHEDULER->current_time() % Model::CONFIG->report_frequency()
      == 0) {
    //        Model::DATA_COLLECTOR->perform_population_statistic();

    std::cout << Model::SCHEDULER->current_time() << "\t";

    auto* pi = Model::POPULATION
                   ->get_person_index<PersonIndexByLocationStateAgeClass>();

    for (std::size_t location = 0;
         location < Model::CONFIG->number_of_locations(); location++) {
      std::cout << "||\t";
      report_number_by_state(location, pi);
      std::cout << Model::MAIN_DATA_COLLECTOR
                           ->blood_slide_prevalence_by_location()[location]
                       * 100
                << "\t";
      std::cout
          << Model::MAIN_DATA_COLLECTOR->total_immune_by_location()[location]
                 / Model::POPULATION->size(location)
          << "\t";
    }
    std::cout << std::endl;
  }
}
