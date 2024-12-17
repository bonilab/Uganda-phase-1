/*
 * TestTreatmentFailureEvent.cpp
 *
 * Defines the event that tests to see if the treatment given to a patient
 * failed or not.
 */
#ifndef TESTTREATMENTFAILUREEVENT_H
#define TESTTREATMENTFAILUREEVENT_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Event.h"

class ClonalParasitePopulation;

class Scheduler;

class Person;

class TestTreatmentFailureEvent : public Event {
  DELETE_COPY_AND_MOVE(TestTreatmentFailureEvent)
  OBJECTPOOL(TestTreatmentFailureEvent)
  POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite)
  PROPERTY_REF(int, therapyId)

public:
  TestTreatmentFailureEvent();
  ~TestTreatmentFailureEvent() override = default;

  static void schedule_event(Scheduler* scheduler, Person* p,
                             ClonalParasitePopulation* clinical_caused_parasite,
                             const int &time, const int &t_id = 0);

  std::string name() override { return "TestTreatmentFailureEvent"; }

private:
  void execute() override;
};

#endif
