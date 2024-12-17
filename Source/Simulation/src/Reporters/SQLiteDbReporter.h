
/*
 * SQLiteDbReporter.h
 *
 * Override the base DbReporter and log genotype information at the district
 * level.
 */
#ifndef SQLITEDBREPORTER_H
#define SQLITEDBREPORTER_H

#include "Helpers/SQLiteDatabase.h"
#include "Reporter.h"

class PersonIndexByLocationStateAgeClass;

class SQLiteDbReporter : public Reporter {
private:
  const std::string insert_genotype_query_ =
      "INSERT INTO genotype (id, name) VALUES (?, ?);";

  const std::string insert_common_query_ = R""""(
  INSERT INTO MonthlyData (DaysElapsed, ModelTime, SeasonalFactor)
  VALUES (?, ?, ?)
  RETURNING id;
  )"""";

  const std::string insert_genotype_query_prefix_ = R"""(
    INSERT INTO MonthlyGenomeData 
    (MonthlyDataId, LocationId, GenomeId, Occurrences, 
    ClinicalOccurrences, Occurrences0to5, Occurrences2to10, 
    WeightedOccurrences) 
    VALUES 
  )""";
  // this query must be created in the initialize function (after the config is
  // initialized)
  std::string insert_site_query_prefix_;

protected:
  std::unique_ptr<SQLiteDatabase> db;

  void populate_genotype_table();

  void populate_db_schema();

  // Return the character code that indicates the level of genotype records (c:
  // cell, d: district)
  virtual char get_genotype_level() = 0;
  virtual void monthly_report_genome_data(int monthId) = 0;
  virtual void monthly_report_site_data(int monthId) = 0;

  void insert_monthly_site_data(const std::vector<std::string> &siteData);
  void insert_monthly_genome_data(const std::vector<std::string> &genomeData);

public:
  // Constructor and destructor
  SQLiteDbReporter() = default;
  SQLiteDbReporter(const SQLiteDbReporter &) = delete;
  SQLiteDbReporter(SQLiteDbReporter &&) = delete;
  SQLiteDbReporter &operator=(const SQLiteDbReporter &) = delete;
  SQLiteDbReporter &operator=(SQLiteDbReporter &&) = delete;
  ~SQLiteDbReporter() override = default;

  // Initialize the reporter with job number and path
  void initialize(int jobNumber, const std::string &path) override;

  // Basic declarations for before run and begin time step
  void before_run() override {}
  void begin_time_step() override {}

  // Overridden functions for monthly reporting and post-run cleanup
  void monthly_report() override;
  // With the new Database class, the database connection will automatically be
  // closed when the Database object is destroyed. Therefore, no explicit action
  // is required here. If there are other cleanup actions to perform, they can
  // be added here.
  void after_run() override {}
};

#endif
