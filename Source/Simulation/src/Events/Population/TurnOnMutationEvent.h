#ifndef TURNONMUTATIONEVENT_H
#define TURNONMUTATIONEVENT_H

#include <string>

#include "Core/PropertyMacro.h"
#include "Events/Event.h"

class TurnOnMutationEvent : public Event {
  DELETE_COPY_AND_MOVE(TurnOnMutationEvent)

  double mutation_probability = 0.0;
  int drug_id = -1;

public:
  explicit TurnOnMutationEvent(const int &at_time,
                               const double &mutation_probability,
                               const int &drug_id);

  ~TurnOnMutationEvent() override = default;

  std::string name() override { return "TurnOnMutationEvent"; }

private:
  void execute() override;
};

#endif  // TURNONMUTATIONEVENT_H
