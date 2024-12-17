/*
 * AgeBandReporter.h
 *
 * This reporter is intended to be used during model calibration and validation
 * and reports the age-banded PfPR during the last year of the given simulation
 * to a CSV  file.
 */
#ifndef AGEBANDREPORTER_H
#define AGEBANDREPORTER_H

#include <vector>

#include "Reporters/Reporter.h"

class AgeBandReporter : public Reporter {
  DELETE_COPY_AND_MOVE(AgeBandReporter)

private:
  // When to start logging the data
  int start_recording = -1;

  // Mapping of the locations to their districts
  std::vector<int> district_lookup;

  // String streams to use when working with the loggers
  std::stringstream pfpr;
  std::stringstream cases;

public:
  AgeBandReporter() = default;

  ~AgeBandReporter() override = default;

  void before_run() override {}

  void begin_time_step() override {}

  void initialize(int job_number, const std::string &path) override;

  void after_run() override {}

  void monthly_report() override;
};

#endif
