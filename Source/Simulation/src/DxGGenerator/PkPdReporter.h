/*
 * File:   PkPdReporter.h
 * Author: Merlin
 *
 * Created on October 29, 2014, 12:56 PM
 */

#ifndef PKPDREPORTER_H
#define PKPDREPORTER_H

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "Reporters/Reporter.h"

class PkPdReporter : public Reporter {
  DELETE_COPY_AND_MOVE(PkPdReporter)

  PROPERTY_REF(DoubleVector, yesterday_density)

public:
  PkPdReporter();

  //    PkPdReporter(const PkPdReporter& orig);
  ~PkPdReporter() override;

  void initialize(int job_number, const std::string &path) override;

  void before_run() override;

  void after_run() override;

  void begin_time_step() override;

  virtual void after_time_step();

  void monthly_report() override;

private:
};

#endif /* PKPDREPORTER_H */
