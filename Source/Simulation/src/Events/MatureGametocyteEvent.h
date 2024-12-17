/*
 * File:   MatureGametocyteEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 11:38 PM
 */

#ifndef MATUREGAMETOCYTEEVENT_H
#define MATUREGAMETOCYTEEVENT_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Event.h"

class ClonalParasitePopulation;

class Scheduler;

class Person;

class MatureGametocyteEvent : public Event {
  DELETE_COPY_AND_MOVE(MatureGametocyteEvent);
  OBJECTPOOL(MatureGametocyteEvent)

  POINTER_PROPERTY(ClonalParasitePopulation, blood_parasite)

public:
  MatureGametocyteEvent();

  //    MatureGametocyteEvent(const MatureGametocyteEvent& orig);
  virtual ~MatureGametocyteEvent();

  static void schedule_event(Scheduler* scheduler, Person* p,
                             ClonalParasitePopulation* blood_parasite,
                             const int &time);

  std::string name() override { return "MatureGametocyteEvent"; }

private:
  void execute() override;
};

#endif /* MATUREGAMETOCYTEEVENT_H */
