/*
 * StrategyBuilder.h
 *
 * Define the factory for strategy objects.
 */
#ifndef STRATEGYBUILDER_H
#define STRATEGYBUILDER_H

#include <yaml-cpp/yaml.h>

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"

class IStrategy;

class Config;

class StrategyBuilder {
  DELETE_COPY_AND_MOVE(StrategyBuilder)

public:
  StrategyBuilder();

  virtual ~StrategyBuilder();

  static IStrategy* build(const YAML::Node &ns, const int &strategy_id,
                          Config* config);

  static void add_therapies(const YAML::Node &ns, IStrategy* result,
                            Config* config);

  static void add_distributions(const YAML::Node &ns, DoubleVector &v);

  static IStrategy* buildSFTStrategy(const YAML::Node &ns,
                                     const int &strategy_id, Config* config);

  static IStrategy* buildCyclingStrategy(const YAML::Node &ns,
                                         const int &strategy_id,
                                         Config* config);

  static IStrategy* buildMFTStrategy(const YAML::Node &ns,
                                     const int &strategy_id, Config* config);

  static IStrategy* buildNestedSwitchingStrategy(const YAML::Node &ns,
                                                 const int &strategy_id,
                                                 Config* config);

  static IStrategy* buildMFTMultiLocationStrategy(const YAML::Node &node,
                                                  const int &id,
                                                  Config* config);

  static IStrategy* buildNestedMFTDifferentDistributionByLocationStrategy(
      const YAML::Node &ns, const int &strategy_id, Config* config);

  static IStrategy* buildDistrictMftStrategy(const YAML::Node &node,
                                             const int &strategy_id,
                                             Config* config);

  static IStrategy* buildMFTAgeBasedStrategy(const YAML::Node &node,
                                             const int &strategyId,
                                             Config* config);
};

#endif
