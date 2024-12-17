#ifndef MDC_DATACOLLECTOR_H
#define MDC_DATACOLLECTOR_H

#include "Core/PropertyMacro.h"
#include "Model.h"

// WIP: This class is an interface for all data collectors
// Currently, it is not used in the project
// In the future, there will be mutliple data collectors that will collect
// different data and pluggable into the model when needed
class DataCollector {
public:
  // Base class constructor
  explicit DataCollector(Model* model = nullptr);
  virtual ~DataCollector() = default;
  DataCollector(const DataCollector &) = delete;
  DataCollector &operator=(const DataCollector &) = delete;
  DataCollector(DataCollector &&) = delete;
  DataCollector &operator=(DataCollector &&) = delete;

  virtual void initialize() = 0;
  virtual void perform_population_statistic() = 0;
  virtual void yearly_update() = 0;
  virtual void monthly_update() = 0;

protected:
  POINTER_PROPERTY(Model, model)
  // ... other common properties and methods ...

  // Utility methods that can be used by derived classes
};

#endif  // MDC_DATACOLLECTOR_H
