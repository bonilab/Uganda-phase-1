/*
 * File:   UpdateByHavingDrugEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 12:16 PM
 */

#ifndef UPDATEWHENDRUGISPRESENTEVENT_H
#define UPDATEWHENDRUGISPRESENTEVENT_H

#include <string>

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Event.h"

class ClonalParasitePopulation;

class Scheduler;

class Person;

class UpdateWhenDrugIsPresentEvent : public Event {
  DELETE_COPY_AND_MOVE(UpdateWhenDrugIsPresentEvent)

  OBJECTPOOL(UpdateWhenDrugIsPresentEvent)

  POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite)

public:
  UpdateWhenDrugIsPresentEvent();

  //    UpdateByHavingDrugEvent(const UpdateByHavingDrugEvent& orig);
  virtual ~UpdateWhenDrugIsPresentEvent();

  static void schedule_event(Scheduler* scheduler, Person* p,
                             ClonalParasitePopulation* clinical_caused_parasite,
                             const int &time);

  std::string name() override { return "UpdateByHavingDrugEvent"; }

private:
  void execute() override;
};

#endif /* UPDATEWHENDRUGISPRESENTEVENT_H */
