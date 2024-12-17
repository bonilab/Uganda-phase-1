#pragma once

#include <string>

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Events/Event.h"

class IntroduceLumefantrineMutantEvent : public Event {
  DELETE_COPY_AND_MOVE(IntroduceLumefantrineMutantEvent)

  OBJECTPOOL(IntroduceLumefantrineMutantEvent)

  VIRTUAL_PROPERTY_REF(int, location)

  VIRTUAL_PROPERTY_REF(double, fraction)

public:
  explicit IntroduceLumefantrineMutantEvent(const int &location = -1,
                                            const int &execute_at = -1,
                                            const double &fraction = 0);

  //    ImportationEvent(const ImportationEvent& orig);
  ~IntroduceLumefantrineMutantEvent() override;

  std::string name() override { return "IntroduceLumefantrineMutantEvent"; }

private:
  void execute() override;
};
