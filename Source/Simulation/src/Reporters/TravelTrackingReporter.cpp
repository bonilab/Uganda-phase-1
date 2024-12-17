#ifdef ENABLE_TRAVEL_TRACKING
#include "TravelTrackingReporter.h"

#include "Core/Config/Config.h"
#include "Model.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexAll.h"

void TravelTrackingReporter::initialize(int job_number, const std::string &path) {
  output_file.open(fmt::format("{}travel_tracking_{}.csv", path, job_number));
}

void TravelTrackingReporter::before_run() {}

void TravelTrackingReporter::begin_time_step() {}

void TravelTrackingReporter::monthly_report() {}
/* Column Name	Data Type	Description */
/* district_id	Integer	Unique identifier for the district. */
/* population_size	Integer	Total number of people residing in the district. */
/* percentage_traveled_last_30_days	Float	Percentage of district population
 * that initiated any trip in the last 30 days. */
/* percentage_traveled_last_60_days	Float	Percentage of district population
 * that initiated any trip in the last 60 days. */
/* percentage_traveled_last_90_days	Float	Percentage of district population
 * that initiated any trip in the last 90 days. */
/* percentage_cross_district_last_30_days	Float	Percentage of district
 * population that initiated a trip outside their district in the last 30 days.
 */
/* percentage_cross_district_last_60_days	Float	Percentage of district
 * population that initiated a trip outside their district in the last 60 days.
 */
/* percentage_cross_district_last_90_days	Float	Percentage of district
 * population that initiated a trip outside their district in the last 90 days.
 */
void TravelTrackingReporter::after_run() {
  // output the travel tracking data
  // csv columns: district_id, population_size,
  // percentage_traveled_last_30_days, percentage_traveled_last_60_days
  // percentage_traveled_last_90_days
  // percentage_cross_district_last_30_days,
  // percentage_cross_district_last_60_days,
  // percentage_cross_district_last_90_days

  // collect data
  auto &district_lookup = SpatialData::get_instance().district_lookup();

  auto number_of_districts = SpatialData::get_instance().get_district_count();
  std::vector<int> population(number_of_districts, 0);
  std::vector<int> traveled_last_30_days(number_of_districts, 0);
  std::vector<int> traveled_last_60_days(number_of_districts, 0);
  std::vector<int> traveled_last_90_days(number_of_districts, 0);

  std::vector<int> cross_district_last_30_days(number_of_districts, 0);
  std::vector<int> cross_district_last_60_days(number_of_districts, 0);
  std::vector<int> cross_district_last_90_days(number_of_districts, 0);

  auto current_time = Model::SCHEDULER->current_time();
  auto* all_person_index =
      Model::POPULATION->get_person_index<PersonIndexAll>();
  for (auto* person : all_person_index->vPerson()) {
    auto district = district_lookup[person->location()];
    population[district]++;

    if (person->day_that_last_trip_was_initiated() > current_time - 30) {
      traveled_last_30_days[district]++;
    }
    if (person->day_that_last_trip_was_initiated() > current_time - 60) {
      traveled_last_60_days[district]++;
    }
    if (person->day_that_last_trip_was_initiated() > current_time - 90) {
      traveled_last_90_days[district]++;
    }
    if (person->day_that_last_trip_outside_district_was_initiated()
        > current_time - 30) {
      cross_district_last_30_days[district]++;
    }
    if (person->day_that_last_trip_outside_district_was_initiated()
        > current_time - 60) {
      cross_district_last_60_days[district]++;
    }
    if (person->day_that_last_trip_outside_district_was_initiated()
        > current_time - 90) {
      cross_district_last_90_days[district]++;
    }
  }

  // output
  /// output header
  output_file << fmt::format(
      "{},{},{},{},{},{},{},{}\n", "district_id", "population_size",
      "percentage_traveled_last_30_days", "percentage_traveled_last_60_days",
      "percentage_traveled_last_90_days",
      "percentage_cross_district_last_30_days",
      "percentage_cross_district_last_60_days",
      "percentage_cross_district_last_90_days");

  for (auto district = 0; district < number_of_districts; district++) {
    output_file << fmt::format(
        "{},{},{},{},{},{},{},{}\n",
        SpatialData::get_instance().adjust_simulation_district_to_raster_index(
            district),
        population[district],
        population[district] == 0
            ? 0
            : static_cast<float>(traveled_last_30_days[district])
                  / population[district],
        population[district] == 0
            ? 0
            : static_cast<float>(traveled_last_60_days[district])
                  / population[district],
        population[district] == 0
            ? 0
            : static_cast<float>(traveled_last_90_days[district])
                  / population[district],
        population[district] == 0
            ? 0
            : static_cast<float>(cross_district_last_30_days[district])
                  / population[district],
        population[district] == 0
            ? 0
            : static_cast<float>(cross_district_last_60_days[district])
                  / population[district],
        population[district] == 0
            ? 0
            : static_cast<float>(cross_district_last_90_days[district])
                  / population[district]);
  }

  // close the file
  if (output_file.is_open()) { output_file.close(); }
}

#endif  // ENABLE_TRAVEL_TRACKING
