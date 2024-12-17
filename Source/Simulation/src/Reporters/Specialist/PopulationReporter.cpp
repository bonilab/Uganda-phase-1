/*
 * PopulationReporter.cpp
 *
 * Implement the PopulationReporter class.
 */
#include "PopulationReporter.h"

#include "Core/Config/Config.h"
#include "MDC/MainDataCollector.h"
#include "Model.h"
#include "Population/Population.h"
#include "easylogging++.h"

void PopulationReporter::initialize(int job_number, const std::string &path) {
  el::Configurations reporter;
  reporter.setToDefault();
  reporter.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
  reporter.setGlobally(el::ConfigurationType::ToFile, "true");
  reporter.setGlobally(
      el::ConfigurationType::Filename,
      fmt::format("{}population_data_{}.{}", path, job_number, Csv::extension));
  reporter.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
  reporter.setGlobally(el::ConfigurationType::LogFlushThreshold, "10");
  el::Loggers::reconfigureLogger("population_reporter", reporter);

  // Log the headers
  ss << "dayselapsed" << Csv::sep << "population" << Csv::sep << "births"
     << Csv::sep << "deaths" << Csv::sep << "malariadeaths" << Csv::sep
     << "cases" << Csv::sep << "treatments" << Csv::sep << Csv::end_line;
}

void PopulationReporter::monthly_report() {
  long population = 0, births = 0, deaths = 0, malariaDeaths = 0, cases = 0,
       treatments = 0;

  // Calculate the summary data
  for (std::size_t location = 0;
       location < Model::CONFIG->number_of_locations(); location++) {
    population += Model::POPULATION->size(location);
    births += Model::MAIN_DATA_COLLECTOR->births_by_location()[location];
    deaths += Model::MAIN_DATA_COLLECTOR->deaths_by_location()[location];
    malariaDeaths +=
        Model::MAIN_DATA_COLLECTOR->malaria_deaths_by_location()[location];
    cases += Model::MAIN_DATA_COLLECTOR
                 ->monthly_number_of_clinical_episode_by_location()[location];
    treatments += Model::MAIN_DATA_COLLECTOR
                      ->monthly_number_of_treatment_by_location()[location];
  }

  // Log the entry
  ss << Model::SCHEDULER->current_time() << Csv::sep << population << Csv::sep
     << births << Csv::sep << deaths << Csv::sep << malariaDeaths << Csv::sep
     << cases << Csv::sep << treatments << Csv::sep;
  CLOG(INFO, "population_reporter") << ss.str();
  ss.str("");
}
