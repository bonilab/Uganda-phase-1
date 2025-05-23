/*
 * Person.cpp
 *
 * Implement the Person object. Note that this is a very complex part of the
 * model and care should be taken in modifying it.
 *
 * NOTE Longer term the goal is to simplify this a bit by shifting some
 *      operations out - remember, separation of concerns!
 */
#include "Person.h"

#include <algorithm>
#include <cmath>

#include "ClonalParasitePopulation.h"
#include "Constants.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "DrugsInBlood.h"
#include "Events/CirculateToTargetLocationNextDayEvent.h"
#include "Events/EndClinicalByNoTreatmentEvent.h"
#include "Events/EndClinicalEvent.h"
#include "Events/MatureGametocyteEvent.h"
#include "Events/MoveParasiteToBloodEvent.h"
#include "Events/ProgressToClinicalEvent.h"
#include "Events/ReceiveTherapyEvent.h"
#include "Events/ReturnToResidenceEvent.h"
#include "Events/TestTreatmentFailureEvent.h"
#include "Events/UpdateEveryKDaysEvent.h"
#include "Events/UpdateWhenDrugIsPresentEvent.h"
#include "Helpers/ObjectHelpers.h"
#include "ImmuneSystem.h"
#include "MDC/MainDataCollector.h"
#include "Model.h"
#include "Population.h"
#include "SingleHostClonalParasitePopulations.h"
#include "Therapies/Drug.h"
#include "Therapies/DrugType.h"
#include "Therapies/MACTherapy.h"
#include "Therapies/SCTherapy.h"
#include "Validation/MovementValidation.h"

OBJECTPOOL_IMPL(Person)

Person::Person()
    : location_(-1),
      residence_location_(-1),
      host_state_(SUSCEPTIBLE),
      age_(-1),
      age_class_(-1),
      birthday_(-1),
      latest_update_time_(-1),
      biting_level_(-1),
      base_biting_level_value_(0),
      moving_level_(-1),
      liver_parasite_type_(nullptr),
      number_of_times_bitten_(0),
      number_of_trips_taken_(0),
      last_therapy_id_(0),
      population_(nullptr),
      immune_system_(nullptr),
      all_clonal_parasite_populations_(nullptr),
      drugs_in_blood_(nullptr),
      today_infections_(nullptr),
#ifdef ENABLE_TRAVEL_TRACKING
      day_that_last_trip_was_initiated_(-1),
      day_that_last_trip_outside_district_was_initiated_(-1),
#endif
      today_target_locations_(nullptr) {
}

void Person::init() {
  Dispatcher::init();

  // Refresh the UID
  _uid = UniqueId::get_instance().get_uid();

  immune_system_ = new ImmuneSystem(this);

  all_clonal_parasite_populations_ =
      new SingleHostClonalParasitePopulations(this);
  all_clonal_parasite_populations_->init();

  drugs_in_blood_ = new DrugsInBlood(this);
  drugs_in_blood_->init();

  today_infections_ = new IntVector();
  today_target_locations_ = new IntVector();
}

Person::~Person() {
  Dispatcher::clear_events();
  ObjectHelpers::delete_pointer<ImmuneSystem>(immune_system_);
  ObjectHelpers::delete_pointer<SingleHostClonalParasitePopulations>(
      all_clonal_parasite_populations_);
  ObjectHelpers::delete_pointer<DrugsInBlood>(drugs_in_blood_);
  ObjectHelpers::delete_pointer<IntVector>(today_infections_);
  ObjectHelpers::delete_pointer<IntVector>(today_target_locations_);
}

void Person::NotifyChange(const Property &property, const void* oldValue,
                          const void* newValue) {
  if (population_ != nullptr) {
    population_->notify_change(this, property, oldValue, newValue);
  }
}

int Person::location() const { return location_; }

void Person::set_location(const int &value) {
  if (location_ != value) {
    all_clonal_parasite_populations_->remove_all_infection_force();
    if (Model::MAIN_DATA_COLLECTOR != nullptr) {
      const auto day_diff =
          (Constants::DAYS_IN_YEAR() - Model::SCHEDULER->current_day_in_year());
      if (location_ != -1) {
        Model::MAIN_DATA_COLLECTOR->update_person_days_by_years(location_,
                                                                -day_diff);
      }
      Model::MAIN_DATA_COLLECTOR->update_person_days_by_years(value, day_diff);
    }

    NotifyChange(LOCATION, &location_, &value);

    location_ = value;
    all_clonal_parasite_populations_->add_all_infection_force();
  }
}

Person::HostStates Person::host_state() const { return host_state_; }

void Person::set_host_state(const HostStates &value) {
  if (host_state_ != value) {
    NotifyChange(HOST_STATE, &host_state_, &value);
    if (value == DEAD) {
      // clear also remove all infection forces
      all_clonal_parasite_populations_->clear();
      clear_events();

      Model::MAIN_DATA_COLLECTOR->record_1_death(
          location_, birthday_, number_of_times_bitten_, age_class_);
    }

    host_state_ = value;
  }
}

int Person::age() const { return age_; }

double Person::age_in_floating() const {
  auto days = Model::SCHEDULER->current_time() - birthday_;
  return days / Constants::DAYS_IN_YEAR();
}

void Person::set_age(const int &value) {
  if (age_ != value) {
    // TODO::if age access the limit of age structure i.e. 100, remove person???

    NotifyChange(AGE, &age_, &value);

    // update biting rate level
    age_ = value;

    // update age class
    if (Model::MODEL != nullptr) {
      unsigned int ac = age_class_ == -1 ? 0 : age_class_;

      while (ac < (Model::CONFIG->number_of_age_classes() - 1)
             && age_ >= Model::CONFIG->age_structure()[ac]) {
        ac++;
      }

      set_age_class((int)ac);
    }
  }
}

int Person::age_class() const { return age_class_; }

void Person::set_age_class(const int &value) {
  if (age_class_ != value) {
    NotifyChange(AGE_CLASS, &age_class_, &value);
    age_class_ = value;
  }
}

int Person::biting_level() const { return biting_level_; }

void Person::set_biting_level(const int &value) {
  auto new_value = value;
  if (new_value < 0) { new_value = 0; }

  if (new_value
      > (Model::CONFIG->relative_bitting_info().number_of_biting_levels - 1)) {
    new_value =
        Model::CONFIG->relative_bitting_info().number_of_biting_levels - 1;
  }
  if (biting_level_ != new_value) {
    all_clonal_parasite_populations_->remove_all_infection_force();

    NotifyChange(BITING_LEVEL, &biting_level_, &new_value);
    biting_level_ = new_value;
    all_clonal_parasite_populations_->add_all_infection_force();
  }
}

int Person::moving_level() const { return moving_level_; }

void Person::set_moving_level(const int &value) {
  if (moving_level_ != value) {
    NotifyChange(MOVING_LEVEL, &moving_level_, &value);
    moving_level_ = value;
  }
}

void Person::increase_age_by_1_year() { set_age(age_ + 1); }

ClonalParasitePopulation* Person::add_new_parasite_to_blood(
    Genotype* parasite_type) const {
  auto* blood_parasite = new ClonalParasitePopulation(parasite_type);

  all_clonal_parasite_populations_->add(blood_parasite);

  blood_parasite->set_last_update_log10_parasite_density(
      Model::CONFIG->parasite_density_level().log_parasite_density_from_liver);

  return blood_parasite;
}

void Person::notify_change_in_force_of_infection(
    const double &sign, const int &parasite_type_id,
    const double &blood_parasite_log_relative_density,
    const double &log_total_relative_parasite_density) {
  // Return if the relative density is zero
  if (blood_parasite_log_relative_density == 0.0) { return; }

  // FOI_i = (+/-) b_i * g(D_i) * D_r
  const auto relative_force_of_infection =
      sign * get_biting_level_value()
      * relative_infectivity(log_total_relative_parasite_density)
      * blood_parasite_log_relative_density;

  population_->notify_change_in_force_of_infection(location_, parasite_type_id,
                                                   relative_force_of_infection);
}

double Person::get_biting_level_value() {
  return Model::CONFIG->relative_bitting_info()
      .v_biting_level_value[biting_level_];
}

double Person::relative_infectivity(const double &log10_parasite_density) {
  const auto d_n =
      log10_parasite_density * Model::CONFIG->relative_infectivity().sigma
      + Model::CONFIG->relative_infectivity().ro_star;
  const auto p = Model::RANDOM->cdf_standard_normal_distribution(d_n);

  return p * p + 0.01;
}

double Person::get_probability_progress_to_clinical() {
  return immune_system_->get_clinical_progression_probability();
}

void Person::cancel_all_other_progress_to_clinical_events_except(
    Event* event) const {
  for (auto* e : *events()) {
    if (e != event && dynamic_cast<ProgressToClinicalEvent*>(e) != nullptr) {
      e->executable = false;
    }
  }
}

void Person::cancel_all_events_except(Event* event) const {
  for (auto* e : *events()) {
    if (e != event) { e->executable = false; }
  }
}

void Person::change_all_parasite_update_function(
    ParasiteDensityUpdateFunction* from,
    ParasiteDensityUpdateFunction* to) const {
  all_clonal_parasite_populations_->change_all_parasite_update_function(from,
                                                                        to);
}

bool Person::will_progress_to_death_when_receive_no_treatment() {
  // yes == death
  const auto p = Model::RANDOM->random_flat(0.0, 1.0);
  return p <= Model::CONFIG
                  ->mortality_when_treatment_fail_by_age_class()[age_class_];
}

bool Person::will_progress_to_death_when_receive_treatment() {
  // yes == death
  double P = Model::RANDOM->random_flat(0.0, 1.0);
  // 90% lower than no treatment
  return P <= Model::CONFIG
                      ->mortality_when_treatment_fail_by_age_class()[age_class_]
                  * (1 - 0.9);
}

void Person::schedule_progress_to_clinical_event_by(
    ClonalParasitePopulation* blood_parasite) {
  const auto time = (age_ <= 5) ? Model::CONFIG->days_to_clinical_under_five()
                                : Model::CONFIG->days_to_clinical_over_five();

  ProgressToClinicalEvent::schedule_event(
      Model::SCHEDULER, this, blood_parasite,
      Model::SCHEDULER->current_time() + time);
}

void Person::schedule_test_treatment_failure_event(
    ClonalParasitePopulation* blood_parasite, const int &testing_day,
    const int &t_id) {
  TestTreatmentFailureEvent::schedule_event(
      Model::SCHEDULER, this, blood_parasite,
      Model::SCHEDULER->current_time() + testing_day, t_id);
}

int Person::complied_dosing_days(const SCTherapy* therapy) {
  // Return the max day if we have full compliance
  if (therapy->full_compliance()) { return therapy->get_max_dosing_day(); }

  // Roll the dice
  auto rv = Model::RANDOM->random_flat(0.0, 1.0);

  // Otherwise, iterate through the probabilities that they will complete the
  // therapy on the given day
  auto upper_bound = therapy->pr_completed_days[0];
  for (auto days = 1; days < therapy->pr_completed_days.size() + 1; days++) {
    if (rv < upper_bound) { return days; }
    upper_bound += therapy->pr_completed_days[days];
  }

  // We encountered an error, this should not happen
  throw std::runtime_error("Bounds of pr_completed_days exceeded: rv = "
                           + std::to_string(rv));
}

// Give the therapy indicated to the individual, making note of the parasite
// that caused the clinical case. Note that we assume that MACTherapy is going
// to be fairly rare, but that additional bookkeeping needs to be done in the
// event of one.
void Person::receive_therapy(Therapy* therapy,
                             ClonalParasitePopulation* clinical_caused_parasite,
                             bool is_mac_therapy) {
  // Start by checking if this is a simple therapy with a single dosing regime
  auto* sc_therapy = dynamic_cast<SCTherapy*>(therapy);
  if (sc_therapy != nullptr) {
    receive_therapy(sc_therapy, is_mac_therapy);
  } else {
    // This is not a simple therapy, multiple treatments and dosing regimes may
    // be involved
    auto* mac_therapy = dynamic_cast<MACTherapy*>(therapy);
    assert(mac_therapy != nullptr);

    starting_mac_drug_values.clear();
    for (std::size_t i = 0; i < mac_therapy->therapy_ids().size(); i++) {
      const auto therapy_id = mac_therapy->therapy_ids()[i];
      const auto start_day = mac_therapy->start_at_days()[i];
      assert(start_day >= 1);

      // Verify the therapy that is part of the regimen
      sc_therapy =
          dynamic_cast<SCTherapy*>(Model::CONFIG->therapy_db()[therapy_id]);
      if (sc_therapy == nullptr) {
        auto message = "Complex therapy (" + std::to_string(therapy->id())
                       + ") contains a reference to an unknown therapy id ("
                       + std::to_string(therapy_id) + ")";
        throw std::runtime_error(message);
      }
      if (!sc_therapy->full_compliance()) {
        auto message = "Complex therapy (" + std::to_string(therapy->id())
                       + ") contains a reference to a therapy ("
                       + std::to_string(therapy_id)
                       + ") that has variable compliance";
        throw std::runtime_error(message);
      }

      if (start_day == 1) {
        receive_therapy(sc_therapy, true);
      } else {
        ReceiveTherapyEvent::schedule_event(
            Model::SCHEDULER, this, sc_therapy,
            Model::SCHEDULER->current_time() + start_day - 1,
            clinical_caused_parasite, true);
      }
    }
  }

  last_therapy_id_ = therapy->id();
}

void Person::receive_therapy(SCTherapy* sc_therapy, bool is_mac_therapy) {
  // Determine the dosing days
  auto dosing_days = complied_dosing_days(sc_therapy);

  // Add the treatment to the blood
  for (int drug_id : sc_therapy->drug_ids) {
    add_drug_to_blood(Model::CONFIG->drug_db()->at(drug_id), dosing_days,
                      is_mac_therapy);
  }
}

void Person::add_drug_to_blood(DrugType* dt, const int &dosing_days,
                               bool is_mac_therapy) {
  // Prepare the drug object
  auto* drug = new Drug(dt);
  drug->set_dosing_days(dosing_days);
  drug->set_last_update_time(Model::SCHEDULER->current_time());

  // Find the mean and standard deviation for the drug, and use those values to
  // determine the drug level for this individual
  const auto sd = dt->age_group_specific_drug_concentration_sd()[age_class_];
  const auto mean_drug_absorption =
      dt->age_specific_drug_absorption()[age_class_];
  auto drug_level =
      Model::RANDOM->random_normal_truncated(mean_drug_absorption, sd);

  // If this is going to be part of a complex therapy regime then we need to
  // note this initial drug level
  if (is_mac_therapy) {
    if (drugs_in_blood()->drugs()->find(dt->id())
        != drugs_in_blood()->drugs()->end()) {
      // Long half-life drugs are already present in the blood
      drug_level = drugs_in_blood()->get_drug(dt->id())->starting_value();
    } else if (starting_mac_drug_values.find(dt->id())
               != starting_mac_drug_values.end()) {
      // Short half-life drugs that were taken, but cleared the blood already
      drug_level = starting_mac_drug_values[dt->id()];
    }
    // Note the value for future use
    starting_mac_drug_values[dt->id()] = drug_level;
  }

  // Set the starting level for this course of treatment
  drug->set_starting_value(drug_level);

  if (drugs_in_blood_->is_drug_in_blood(dt)) {
    drug->set_last_update_value(
        drugs_in_blood_->get_drug(dt->id())->last_update_value());
  } else {
    drug->set_last_update_value(0.0);
  }

  drug->set_start_time(Model::SCHEDULER->current_time());
  drug->set_end_time(Model::SCHEDULER->current_time()
                     + dt->get_total_duration_of_drug_activity(dosing_days));

  drugs_in_blood_->add_drug(drug);
}

void Person::schedule_update_by_drug_event(
    ClonalParasitePopulation* clinical_caused_parasite) {
  UpdateWhenDrugIsPresentEvent::schedule_event(
      Model::SCHEDULER, this, clinical_caused_parasite,
      Model::SCHEDULER->current_time() + 1);
}

void Person::schedule_end_clinical_event(
    ClonalParasitePopulation* clinical_caused_parasite) {
  int dClinical = Model::RANDOM->random_normal(7, 2);
  dClinical = std::min<int>(std::max<int>(dClinical, 5), 14);

  EndClinicalEvent::schedule_event(
      Model::SCHEDULER, this, clinical_caused_parasite,
      Model::SCHEDULER->current_time() + dClinical);
}

void Person::schedule_end_clinical_by_no_treatment_event(
    ClonalParasitePopulation* clinical_caused_parasite) {
  auto d_clinical = Model::RANDOM->random_normal(7, 2);
  d_clinical = std::min<int>(std::max<int>(d_clinical, 5), 14);

  EndClinicalByNoTreatmentEvent::schedule_event(
      Model::SCHEDULER, this, clinical_caused_parasite,
      Model::SCHEDULER->current_time() + d_clinical);
}

void Person::change_state_when_no_parasite_in_blood() {
  if (all_clonal_parasite_populations_->size() == 0) {
    if (liver_parasite_type_ == nullptr) {
      set_host_state(SUSCEPTIBLE);
    } else {
      set_host_state(EXPOSED);
    }
    immune_system_->set_increase(false);
  }
}

void Person::determine_relapse_or_not(
    ClonalParasitePopulation* clinical_caused_parasite) {
  if (all_clonal_parasite_populations_->contain(clinical_caused_parasite)) {
    const auto p = Model::RANDOM->random_flat(0.0, 1.0);

    if (p <= Model::CONFIG->p_relapse()) {
      // progress to clinical after several days
      clinical_caused_parasite->set_update_function(
          Model::MODEL->progress_to_clinical_update_function());
      clinical_caused_parasite->set_last_update_log10_parasite_density(
          Model::CONFIG->parasite_density_level()
              .log_parasite_density_asymptomatic);
      schedule_relapse_event(clinical_caused_parasite,
                             Model::CONFIG->relapse_duration());

    } else {
      // progress to clearance
      if (clinical_caused_parasite->last_update_log10_parasite_density()
          > Model::CONFIG->parasite_density_level()
                .log_parasite_density_asymptomatic) {
        clinical_caused_parasite->set_last_update_log10_parasite_density(
            Model::CONFIG->parasite_density_level()
                .log_parasite_density_asymptomatic);
      }
      clinical_caused_parasite->set_update_function(
          Model::MODEL->immunity_clearance_update_function());
    }
  }
}

void Person::determine_clinical_or_not(
    ClonalParasitePopulation* clinical_caused_parasite) {
  if (all_clonal_parasite_populations_->contain(clinical_caused_parasite)) {
    const auto p = Model::RANDOM->random_flat(0.0, 1.0);

    if (p <= get_probability_progress_to_clinical()) {
      // progress to clinical after several days
      clinical_caused_parasite->set_update_function(
          Model::MODEL->progress_to_clinical_update_function());
      clinical_caused_parasite->set_last_update_log10_parasite_density(
          Model::CONFIG->parasite_density_level()
              .log_parasite_density_asymptomatic);
      schedule_relapse_event(clinical_caused_parasite,
                             Model::CONFIG->relapse_duration());

    } else {
      // progress to clearance

      clinical_caused_parasite->set_update_function(
          Model::MODEL->immunity_clearance_update_function());
    }
  }
}

void Person::schedule_relapse_event(
    ClonalParasitePopulation* clinical_caused_parasite,
    const int &time_until_relapse) {
  int duration = Model::RANDOM->random_normal(time_until_relapse, 15);
  duration = std::min<int>(std::max<int>(duration, time_until_relapse - 15),
                           time_until_relapse + 15);
  ProgressToClinicalEvent::schedule_event(
      Model::SCHEDULER, this, clinical_caused_parasite,
      Model::SCHEDULER->current_time() + duration);
}

void Person::update() {
  // Make sure we haven't already updated
  assert(host_state_ != DEAD);
  if (latest_update_time_ == Model::SCHEDULER->current_time()) return;

  // Start by updating the density of each blood parasite in parasite
  // population, parasite will be killed by immune system
  all_clonal_parasite_populations_->update();

  // Update the drug concentration, then apply this to the parasite(s) present
  drugs_in_blood_->update();
  all_clonal_parasite_populations_->update_by_drugs(drugs_in_blood_);

  // Update the individual immune system
  immune_system_->update();

  // Update the overall state
  update_current_state();

  update_biting_level();

  // Set the current time for bookkeeping
  latest_update_time_ = Model::SCHEDULER->current_time();
}

void Person::update_biting_level() {
  if (Model::CONFIG->using_age_dependent_bitting_level()) {
    const auto new_biting_level_value =
        base_biting_level_value_ * get_age_dependent_biting_factor();
    const auto diff_in_level = static_cast<int>(
        std::floor(new_biting_level_value - get_biting_level_value())
        / ((Model::CONFIG->relative_bitting_info().max_relative_biting_value
            - 1)
           / static_cast<double>(
               Model::CONFIG->relative_bitting_info().number_of_biting_levels
               - 1)));
    if (diff_in_level != 0) { set_biting_level(biting_level_ + diff_in_level); }
  }
}

void Person::update_current_state() {
  // clear drugs <=0.1
  drugs_in_blood_->clear_cut_off_drugs();
  // clear cured parasite
  all_clonal_parasite_populations_->clear_cured_parasites();

  if (all_clonal_parasite_populations_->size() == 0) {
    change_state_when_no_parasite_in_blood();
  } else {
    immune_system_->set_increase(true);
  }
}

void Person::randomly_choose_parasite() {
  if (today_infections_->empty()) {
    // already chose
    return;
  }
  if (today_infections_->size() == 1) {
    infected_by(today_infections_->at(0));
  } else {
    const std::size_t index_random_parasite =
        Model::RANDOM->random_uniform(today_infections_->size());
    infected_by(today_infections_->at(index_random_parasite));
  }

  today_infections_->clear();
}

void Person::infected_by(const int &parasite_type_id) {
  // only infect if liver is available :D
  if (liver_parasite_type_ == nullptr) {
    if (host_state_ == SUSCEPTIBLE) { set_host_state(EXPOSED); }

    Genotype* genotype = Model::CONFIG->genotype_db()->at(parasite_type_id);
    liver_parasite_type_ = genotype;

    // move parasite to blood in next 7 days
    schedule_move_parasite_to_blood(genotype, 7);
  }
}

// Inflict a bite upon the person with sporozoites of the given type being
// contained within the bite. When a bite is inflicted, the immune system
// is challenged. If the challenge fails the method will return true
// indicating that they are now infected.
bool Person::inflict_bite(const unsigned int parasite_type_id) {
  // Update the overall bite count
  increase_number_of_times_bitten();

  // Get the probability of infection of a naive individual
  double pr = Model::CONFIG->transmission_parameter();

  // Get the current immunity and calculate the baseline probability
  double theta = immune_system()->get_current_value();
  double pr_inf = pr * (1 - (theta - 0.2) / 0.6) + 0.1 * ((theta - 0.2) / 0.6);

  // High immunity reduces likelihood of infection
  if (theta > 0.8) { pr_inf = 0.1; }

  // Low immunity sets likelihood at the probability of infection
  if (theta < 0.2) { pr_inf = pr; }

  // If the draw is less than pr_inf, they get infected
  const double draw = Model::RANDOM->random_flat(0.0, 1.0);
  if (draw < pr_inf) {
    if (host_state() != Person::EXPOSED && liver_parasite_type() == nullptr) {
      today_infections()->push_back((int)parasite_type_id);
      return true;
    }
  }

  // We were not infected
  return false;
}

void Person::schedule_move_parasite_to_blood(Genotype* genotype,
                                             const int &time) {
  MoveParasiteToBloodEvent::schedule_event(
      Model::SCHEDULER, this, genotype,
      Model::SCHEDULER->current_time() + time);
}

void Person::schedule_mature_gametocyte_event(
    ClonalParasitePopulation* clinical_caused_parasite) {
  const auto day_mature_gametocyte =
      (age_ <= 5) ? Model::CONFIG->days_mature_gametocyte_under_five()
                  : Model::CONFIG->days_mature_gametocyte_over_five();
  MatureGametocyteEvent::schedule_event(
      Model::SCHEDULER, this, clinical_caused_parasite,
      Model::SCHEDULER->current_time() + day_mature_gametocyte);
}

void Person::schedule_update_every_K_days_event(const int &time) {
  UpdateEveryKDaysEvent::schedule_event(
      Model::SCHEDULER, this, Model::SCHEDULER->current_time() + time);
}

void Person::randomly_choose_target_location() {
  if (today_target_locations_->empty()) {
    // already chose
    return;
  }
  int target_location =
      today_target_locations_->size() == 1
          ? today_target_locations_->front()
          : today_target_locations_->at(static_cast<int>(
              Model::RANDOM->random_uniform(today_target_locations_->size())));

  // Report the movement if need be
  if (Model::MODEL->report_movement()) {
    auto person_index = static_cast<int>(PersonIndexAllHandler::index());
    MovementValidation::add_move(person_index, location_, target_location);
  }

  schedule_move_to_target_location_next_day_event(target_location);
  today_target_locations_->clear();

#ifdef ENABLE_TRAVEL_TRACKING
  // Update the day of the last initiated trip to the next day from current
  // time.
  day_that_last_trip_was_initiated_ = Model::SCHEDULER->current_time() + 1;

  // Check for district raster data availability for spatial analysis.
  if (SpatialData::get_instance().has_raster(
          SpatialData::SpatialFileType::Districts)) {
    auto &spatial_data = SpatialData::get_instance();

    // Determine the source and destination districts for the current trip.
    int source_district = spatial_data.district_lookup()[location_];
    int destination_district = spatial_data.district_lookup()[target_location];

    // If the trip crosses district boundaries, update the day of the last
    // outside-district trip to the next day from current time.
    if (source_district != destination_district) {
      day_that_last_trip_outside_district_was_initiated_ =
          Model::SCHEDULER->current_time() + 1;
    }
    /* fmt::print("Person {} moved from district {} to district {}\n", _uid, */
    /*            source_district, destination_district); */
  }
#endif
}

void Person::schedule_move_to_target_location_next_day_event(
    const int &location) {
  this->number_of_trips_taken_ += 1;
  CirculateToTargetLocationNextDayEvent::schedule_event(
      Model::SCHEDULER, this, location, Model::SCHEDULER->current_time() + 1);
}

bool Person::has_return_to_residence_event() const {
  for (Event* event : *events()) {
    if (dynamic_cast<ReturnToResidenceEvent*>(event) != nullptr) {
      return true;
    }
  }
  return false;
}

void Person::cancel_all_return_to_residence_events() const {
  for (Event* e : *events()) {
    if (dynamic_cast<ReturnToResidenceEvent*>(e) != nullptr) {
      e->executable = false;
    }
  }
}

bool Person::has_detectable_parasite() const {
  return all_clonal_parasite_populations_->has_detectable_parasite();
}

void Person::increase_number_of_times_bitten() {
  if (Model::SCHEDULER->current_time()
      >= Model::CONFIG->start_collect_data_day()) {
    number_of_times_bitten_++;
  }
}

double Person::get_age_dependent_biting_factor() const {
  //
  // 0.00 - 0.25  -  6.5
  // 0.25 - 0.50  -  8.0
  // 0.50 - 0.75  -  9.0
  // 0.75 - 1.00  -  9.5
  // 1.00 - 2.00  -  11.0
  // 2.00 - 3.00  -  13.5
  // 3.00 - 4.00  -  15.5
  // 4.00 - 5.00  -  17.5
  // + 2.75kg until 20
  // then divide by 61.5

  const auto age = age_in_floating();
  // std::cout << "age: " << age << std::endl;
  if (age < 1) {
    if (age < 0.25) return 0.106;
    if (age < 0.5) return 0.13;
    if (age < 0.75) return 0.1463;
    return 0.1545;
  }
  if (age < 2) return 0.1789;
  if (age < 3) return 0.2195;
  if (age < 4) return 0.2520;
  if (age < 20) return (17.5 + (age - 4) * 2.75) / 61.5;
  return 1.0;
}

bool Person::isGametocytaemic() const {
  return all_clonal_parasite_populations_->is_gametocytaemic();
}

void Person::generate_prob_present_at_mda_by_age() {
  if (prob_present_at_mda_by_age().empty()) {
    for (std::size_t i = 0;
         i < Model::CONFIG->mean_prob_individual_present_at_mda().size(); i++) {
      auto value = Model::RANDOM->random_beta(
          Model::CONFIG->prob_individual_present_at_mda_distribution()[i].alpha,
          Model::CONFIG->prob_individual_present_at_mda_distribution()[i].beta);
      prob_present_at_mda_by_age_.push_back(value);
    }
  }
}

double Person::prob_present_at_mda() {
  std::size_t i = 0;
  while (age_ > Model::CONFIG->age_bracket_prob_individual_present_at_mda()[i]
         && i < Model::CONFIG->age_bracket_prob_individual_present_at_mda()
                    .size()) {
    i++;
  }

  return prob_present_at_mda_by_age_[i];
}

bool Person::has_effective_drug_in_blood() const {
  for (const auto &kv_drug : *drugs_in_blood_->drugs()) {
    if (kv_drug.second->last_update_value() > 0.5) return true;
  }
  return false;
}
