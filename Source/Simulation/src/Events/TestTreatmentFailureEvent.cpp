/*
 * TestTreatmentFailureEvent.cpp
 *
 * Implement event that tests to see if the treatment given to a patient failed
 * or not.
 */
#include "TestTreatmentFailureEvent.h"

#include "Core/Config/Config.h"
#include "Core/Scheduler.h"
#include "MDC/MainDataCollector.h"
#include "Model.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/Person.h"

OBJECTPOOL_IMPL(TestTreatmentFailureEvent)

TestTreatmentFailureEvent::TestTreatmentFailureEvent()
    : clinical_caused_parasite_(nullptr), therapyId_(0) {}

void TestTreatmentFailureEvent::schedule_event(
    Scheduler* scheduler, Person* p,
    ClonalParasitePopulation* clinical_caused_parasite, const int &time,
    const int &t_id) {
  // Ensure that the scheduler exists
  assert(scheduler != nullptr);

  // Create the event to be added to the queue
  auto* e = new TestTreatmentFailureEvent();
  e->dispatcher = p;
  e->set_clinical_caused_parasite(clinical_caused_parasite);
  e->time = time;
  e->set_therapyId(t_id);
  p->add(e);
  scheduler->schedule_individual_event(e);
}

void TestTreatmentFailureEvent::execute() {
  auto* person = dynamic_cast<Person*>(dispatcher);

  // If the parasite is still present at a detectable level, then it's a
  // treatment failure
  if (person->all_clonal_parasite_populations()->contain(
          clinical_caused_parasite())
      && clinical_caused_parasite_->last_update_log10_parasite_density()
             > Model::CONFIG->parasite_density_level()
                   .log_parasite_density_detectable) {
    Model::MAIN_DATA_COLLECTOR->record_1_treatment_failure_by_therapy(
        person->location(), person->age_class(), therapyId_);
  } else {
    Model::MAIN_DATA_COLLECTOR->record_1_treatment_success_by_therapy(
        person->location(), person->age_class(), therapyId_);
  }
}
