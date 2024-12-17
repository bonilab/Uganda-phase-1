#ifndef REPOTERS_TRAVELTRACKINGREPORTER_H
#define REPOTERS_TRAVELTRACKINGREPORTER_H

#ifdef ENABLE_TRAVEL_TRACKING

#include <fstream>

#include "Reporters/Reporter.h"
class TravelTrackingReporter : public Reporter {
  DELETE_COPY_AND_MOVE(TravelTrackingReporter)
private:
  std::ofstream output_file;

public:
  TravelTrackingReporter() = default;
  ~TravelTrackingReporter() override = default;

  void initialize(int job_number, const std::string &path) override;
  void before_run() override;
  void after_run() override;
  void begin_time_step() override;
  void monthly_report() override;
};

#endif  // ENABLE_TRAVEL_TRACKING
#endif  // REPOTERS_TRAVELTRACKINGREPORTER_H
