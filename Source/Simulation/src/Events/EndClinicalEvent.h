/*
 * File:   EndClinicalEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 12:27 PM
 */

#ifndef ENDCLINICALEVENT_H
#define ENDCLINICALEVENT_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Event.h"

class ClonalParasitePopulation;

class Scheduler;

class Person;

class EndClinicalEvent : public Event {
  DELETE_COPY_AND_MOVE(EndClinicalEvent);
  OBJECTPOOL(EndClinicalEvent)

  POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite)

public:
  EndClinicalEvent();

  //    EndClinicalEvent(const EndClinicalEvent& orig);
  virtual ~EndClinicalEvent();

  static void schedule_event(Scheduler* scheduler, Person* p,
                             ClonalParasitePopulation* clinical_caused_parasite,
                             const int &time);

  std::string name() override { return "EndClinicalEvent"; }

private:
  void execute() override;
};

#endif /* ENDCLINICALEVENT_H */
