#ifndef SINGLEROUNDMDAEVENT_H
#define SINGLEROUNDMDAEVENT_H

#include <vector>

#include "Core/PropertyMacro.h"
#include "Events/Event.h"

class SingleRoundMDAEvent : public Event {
  DELETE_COPY_AND_MOVE(SingleRoundMDAEvent)

public:
  std::vector<double> fraction_population_targeted;
  int days_to_complete_all_treatments{14};

  explicit SingleRoundMDAEvent(const int &execute_at);

  virtual ~SingleRoundMDAEvent() = default;

  std::string name() override { return "SingleRoundMDAEvent"; }

private:
  void execute() override;
};

#endif  // SINGLEROUNDMDAEVENT_H
