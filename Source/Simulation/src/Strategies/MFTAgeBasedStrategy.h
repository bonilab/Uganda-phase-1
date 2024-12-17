#ifndef STRATEGIES_MFTAGEBASED_H
#define STRATEGIES_MFTAGEBASED_H

#include "Strategies/IStrategy.h"
class MFTAgeBasedStrategy : public IStrategy {
public:
  std::vector<Therapy*> therapy_list;
  std::vector<double> age_boundaries;

  std::vector<int> map_age_to_therapy_index;

  MFTAgeBasedStrategy();
  MFTAgeBasedStrategy(const MFTAgeBasedStrategy &) = delete;
  MFTAgeBasedStrategy(MFTAgeBasedStrategy &&) = delete;
  MFTAgeBasedStrategy &operator=(const MFTAgeBasedStrategy &) = delete;
  MFTAgeBasedStrategy &operator=(MFTAgeBasedStrategy &&) = delete;
  ~MFTAgeBasedStrategy() override = default;

  void add_therapy(Therapy* therapy) override;

  Therapy* get_therapy(Person* person) override;

  [[nodiscard]] std::string to_string() const override;

  void update_end_of_time_step() override{};

  void adjust_started_time_point(const int &currentTime) override{};

  void monthly_update() override{};

  [[nodiscard]] size_t find_age_range_index(double age) const;

  static size_t find_age_range_index(const std::vector<double> &ageBoundaries,
                                     double age);
};

#endif  // STRATEGIES_MFTAGEBASED_H

