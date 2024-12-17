/*
 * File:   BirthdayEvent.h
 * Author: nguyentran
 *
 * Created on May 9, 2013, 2:42 PM
 */

#ifndef BIRTHDAYEVENT_H
#define BIRTHDAYEVENT_H

#include <string>

#include "Core/ObjectPool.h"
#include "Event.h"

class Person;

class BirthdayEvent : public Event {
  OBJECTPOOL(BirthdayEvent)

  DELETE_COPY_AND_MOVE(BirthdayEvent)

public:
  BirthdayEvent();

  //    BirthdayEvent(const BirthdayEvent& orig);
  virtual ~BirthdayEvent();

  static void schedule_event(Scheduler* scheduler, Person* p, const int &time);

  std::string name() override;

private:
  void execute() override;
};

#endif /* BIRTHDAYEVENT_H */
