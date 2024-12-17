/*
 * File:   UpdateEveryKDaysEvent.h
 * Author: Merlin
 *
 * Created on August 1, 2013, 12:02 AM
 */

#ifndef UPDATEEVERYKDAYSEVENT_H
#define UPDATEEVERYKDAYSEVENT_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Event.h"

class Person;

class UpdateEveryKDaysEvent : public Event {
  DELETE_COPY_AND_MOVE(UpdateEveryKDaysEvent)

  OBJECTPOOL(UpdateEveryKDaysEvent)

public:
  UpdateEveryKDaysEvent();

  //    UpdateEveryKDaysEvent(const UpdateEveryKDaysEvent& orig);
  virtual ~UpdateEveryKDaysEvent();

  static void schedule_event(Scheduler* scheduler, Person* p, const int &time);

  std::string name() override { return "UpdateEveryKDaysEvent"; }

private:
  void execute() override;
};

#endif /* UPDATEEVERYKDAYSEVENT_H */
