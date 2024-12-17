//
// Created by Nguyen Tran on 3/5/2018.
//

#ifndef POMS_BFREPORTER_H
#define POMS_BFREPORTER_H

#include <sstream>

#include "Reporter.h"

class MonthlyReporter : public Reporter {
  DELETE_COPY_AND_MOVE(MonthlyReporter)

public:
  MonthlyReporter();

  ~MonthlyReporter() override;

  void before_run() override {}

  void begin_time_step() override {}

  void initialize(int job_number, const std::string &path) override;

  void after_run() override;

  void monthly_report() override;

  void print_EIR_PfPR_by_location();

  //  void print_monthly_incidence_by_location();
};

#endif  // POMS_BFREPORTER_H
