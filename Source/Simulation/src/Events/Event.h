/*
 * File:   Event.h
 * Author: nguyentran
 *
 * Created on May 3, 2013, 3:13 PM
 */

#ifndef EVENT_H
#define EVENT_H

#include <string>

#include "Core/PropertyMacro.h"
#include "Population/Properties/IndexHandler.hxx"

class Dispatcher;

class Scheduler;

class Event : public IndexHandler {
  DELETE_COPY_AND_MOVE(Event)

public:
  Scheduler* scheduler{nullptr};
  Dispatcher* dispatcher{nullptr};
  bool executable{false};
  int time{-1};

  Event();

  //    Event(const Event& orig);
  virtual ~Event();

  void perform_execute();

  virtual std::string name() = 0;

private:
  virtual void execute() = 0;
};

#endif /* EVENT_H */
