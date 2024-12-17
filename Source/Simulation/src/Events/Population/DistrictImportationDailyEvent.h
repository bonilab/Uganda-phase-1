#ifndef DISTRICTIMPORTATIONDAILYEVENT_H
#define DISTRICTIMPORTATIONDAILYEVENT_H

#include "Events/Event.h"
class DistrictImportationDailyEvent : public Event {
  VIRTUAL_PROPERTY_REF(int, district)
  VIRTUAL_PROPERTY_REF(int, locus)
  VIRTUAL_PROPERTY_REF(int, mutant_allele)
  VIRTUAL_PROPERTY_REF(double, daily_rate)
public:
  explicit DistrictImportationDailyEvent(int district = -1, int locus = -1,
                                         int mutantAllele = -1,
                                         double dailyRate = -1,
                                         int startDay = -1);

  DistrictImportationDailyEvent(const DistrictImportationDailyEvent &) = delete;
  DistrictImportationDailyEvent(DistrictImportationDailyEvent &&) = delete;
  DistrictImportationDailyEvent &operator=(
      const DistrictImportationDailyEvent &) = delete;
  DistrictImportationDailyEvent &operator=(DistrictImportationDailyEvent &&) =
      delete;
  ~DistrictImportationDailyEvent() override = default;

  inline static const std::string EventName =
      "district_importation_daily_event";

  static void schedule_event(Scheduler* scheduler, int district, int locus,
                             int mutantAllele, double dailyRate, int startDay);

  std::string name() override { return "DistrictImportationDailyEvent"; }

private:
  void execute() override;
};

#endif  // DISTRICTIMPORTATIONDAILYEVENT_H
