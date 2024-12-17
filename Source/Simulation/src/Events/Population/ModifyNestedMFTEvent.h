#ifndef MODIFYNESTEDMFTEVENT_H
#define MODIFYNESTEDMFTEVENT_H

#include <string>

#include "Core/PropertyMacro.h"
#include "Events/Event.h"

class ModifyNestedMFTEvent : public Event {
  DELETE_COPY_AND_MOVE(ModifyNestedMFTEvent)

public:
  int strategy_id{-1};

  ModifyNestedMFTEvent(const int &at_time, const int &strategy_id);

  virtual ~ModifyNestedMFTEvent() = default;

  std::string name() override { return "ChangeStrategyEvent"; }

private:
  void execute() override;
};

#endif  // MODIFYNESTEDMFTEVENT_H
