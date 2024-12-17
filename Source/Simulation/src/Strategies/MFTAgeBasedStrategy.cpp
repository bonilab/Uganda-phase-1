
#include "MFTAgeBasedStrategy.h"

#include <sstream>

#include "Population/Person.h"
#include "Therapies/Therapy.hxx"

// This is the unit test for the find_age_range_index function
// std::vector<double> age_boundaries = {2.7, 5.5, 10.8, 20.8};
// std::vector<double> sample_ages = {-1, 2, 2.7, 4, 5.5, 8, 10.8, 15, 20.8,
// 30};
//
// for (auto age : sample_ages) {
//     auto index = find_age_range_index(age_boundaries, age);
//     std::cout << "The age " << age << " is contained in the range at index:
//     " << index << std::endl;
// }
// NOTE: This function can be generalized and moved to a utility class
size_t MFTAgeBasedStrategy::find_age_range_index(
    const std::vector<double> &ageBoundaries, double age) {
  // using binary search to find the age range index
  if (age < ageBoundaries[0]) {
    // Age is below the first boundary
    return 0;
  }
  if (age >= ageBoundaries.back()) {
    // Age is above the last boundary, return the size of the boundaries which
    // is the last index of the therapy list
    return ageBoundaries.size();
  }

  size_t left = 0;
  size_t right = ageBoundaries.size() - 1;

  while (left < right) {
    size_t mid = left + (right - left) / 2;

    // if found the age at the boundaries, return the next index
    if (age == ageBoundaries[mid]) { return mid + 1; }

    if (age > ageBoundaries[mid]) {
      left = mid + 1;
    } else {
      right = mid;
    }
  }
  // the loop exists when left == right
  // ageBoundaries[right - 1] < age < ageBoundaries[right] and right is the
  // according index in the therapy list
  return right;
}

size_t MFTAgeBasedStrategy::find_age_range_index(double age) const {
  return find_age_range_index(age_boundaries, age);
}

MFTAgeBasedStrategy::MFTAgeBasedStrategy()
    : IStrategy("MFTAgeBased", MFTAgeBased) {}

void MFTAgeBasedStrategy::add_therapy(Therapy* therapy) {
  therapy_list.push_back(therapy);
}

Therapy* MFTAgeBasedStrategy::get_therapy(Person* person) {
  auto therapyIndex = find_age_range_index(person->age_in_floating());
  // std::cout << "The age " << person->age_in_floating()
  //           << " is receiving therapy: " << therapyIndex << std::endl;
  return therapy_list[therapyIndex];
}

std::string MFTAgeBasedStrategy::to_string() const {
  std::stringstream sstm;
  sstm << IStrategy::id() << "-" << IStrategy::name() << "-";
  std::string sep;
  for (auto* therapy : therapy_list) {
    sstm << sep << therapy->id();
    sep = ",";
  }
  sep = "";
  sstm << "-";
  for (auto dist : age_boundaries) {
    sstm << sep << dist;
    sep = ",";
  }
  return sstm.str();
}
