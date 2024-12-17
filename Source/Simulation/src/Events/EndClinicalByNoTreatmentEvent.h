/*
 * File:   EndClinicalByNoTreatmentEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 12:28 PM
 */

#ifndef ENDCLINICALBYNOTREATMENTEVENT_H
#define ENDCLINICALBYNOTREATMENTEVENT_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Event.h"

class ClonalParasitePopulation;

class Scheduler;

class Person;

class EndClinicalByNoTreatmentEvent : public Event {
  DELETE_COPY_AND_MOVE(EndClinicalByNoTreatmentEvent)

  OBJECTPOOL(EndClinicalByNoTreatmentEvent)

  POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite)

public:
  EndClinicalByNoTreatmentEvent();

  //    EndClinicalByNoTreatmentEvent(const EndClinicalByNoTreatmentEvent&
  //    orig);
  virtual ~EndClinicalByNoTreatmentEvent();

  static void schedule_event(Scheduler* scheduler, Person* p,
                             ClonalParasitePopulation* clinical_caused_parasite,
                             const int &time);

  std::string name() override { return "EndClinicalByNoTreatmentEvent"; }

private:
  void execute() override;
};

#endif /* ENDCLINICALBYNOTREATMENTEVENT_H */
