#ifndef TURNOFFMUTATIONEVENT_H
#define TURNOFFMUTATIONEVENT_H

#include <string>

#include "Core/PropertyMacro.h"
#include "Events/Event.h"

class TurnOffMutationEvent : public Event {
  DELETE_COPY_AND_MOVE(TurnOffMutationEvent)

public:
  explicit TurnOffMutationEvent(const int &at_time);

  virtual ~TurnOffMutationEvent() = default;

  std::string name() override { return "TurnOffMutationEvent"; }

private:
  void execute() override;
};

#endif  // TURNOFFMUTATIONEVENT_H
