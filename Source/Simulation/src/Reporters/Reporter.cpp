/*
 * Reporter.cpp
 *
 * Implements a factory pattern to generate the various reporter types.
 */

#include "Reporter.h"

#include "ConsoleReporter.h"
#include "DbReporter.h"
#include "DbReporterDistrict.h"
#include "MMCReporter.h"
#include "Model.h"
#include "MonthlyReporter.h"
#include "Reporters/SQLitePixelReporter.h"
#include "SQLiteDistrictReporter.h"
#include "Specialist/AgeBandReporter.h"
#include "Specialist/CellularReporter.h"
#include "Specialist/GenotypeCarriersReporter.h"
#include "Specialist/MovementReporter.h"
#include "Specialist/NullReporter.hxx"
#include "Specialist/PopulationReporter.h"
#include "Specialist/SeasonalImmunity.h"
#include "Specialist/TherapyRecordReporter.h"
#include "easylogging++.h"

#ifdef ENABLE_TRAVEL_TRACKING
#include "Reporters/TravelTrackingReporter.h"
#endif

std::map<std::string, Reporter::ReportType> Reporter::ReportTypeMap{
    {"Console", CONSOLE},
    {"MonthlyReporter", MONTHLY_REPORTER},
    {"MMC", MMC_REPORTER},
    {"DbReporter", DB_REPORTER},
    {"DbReporterDistrict", DB_REPORTER_DISTRICT},
    {"MovementReporter", MOVEMENT_REPORTER},
    {"PopulationReporter", POPULATION_REPORTER},
    {"CellularReporter", CELLULAR_REPORTER},
    {"GenotypeCarriers", GENOTYPE_CARRIERS},
    {"SeasonalImmunity", SEASONAL_IMMUNITY},
    {"AgeBand", AGE_BAND_REPORTER},
    {"TherapyRecord", THERAPY_RECORD_REPORTER},
    {"SQLiteDistrictReporter", SQLITE_DISTRICT_REPORTER},
    {"SQLitePixelReporter", SQLITE_PIXEL_REPORTER},
#ifdef ENABLE_TRAVEL_TRACKING
    {"TravelTrackingReporter", TRAVEL_TRACKING_REPORTER},
#endif
    {"Null", NULL_REPORTER}};

Reporter* Reporter::MakeReport(ReportType report_type) {
  switch (report_type) {
    case CONSOLE:
      return new ConsoleReporter();
    case MONTHLY_REPORTER:
      return new MonthlyReporter();
    case MMC_REPORTER:
      return new MMCReporter();
    case DB_REPORTER:
      return new DbReporter();
    case DB_REPORTER_DISTRICT:
      return new DbReporterDistrict();
    case MOVEMENT_REPORTER:
      return new MovementReporter();
    case POPULATION_REPORTER:
      return new PopulationReporter();
    case CELLULAR_REPORTER:
      return new CellularReporter();
    case GENOTYPE_CARRIERS:
      return new GenotypeCarriersReporter();
    case SEASONAL_IMMUNITY:
      return new SeasonalImmunity();
    case AGE_BAND_REPORTER:
      return new AgeBandReporter();
    case THERAPY_RECORD_REPORTER:
      return new TherapyRecordReporter();
    case SQLITE_DISTRICT_REPORTER:
      return new SQLiteDistrictReporter();
    case SQLITE_PIXEL_REPORTER:
      return new SQLitePixelReporter();
#ifdef ENABLE_TRAVEL_TRACKING
    case TRAVEL_TRACKING_REPORTER:
      return new TravelTrackingReporter();
#endif
    case NULL_REPORTER:
      return new NullReporter();
    default:
      LOG(ERROR) << "No reporter type supplied";
      throw std::runtime_error("No reporter type supplied");
  }
}
