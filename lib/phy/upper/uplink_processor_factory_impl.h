/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/uplink_processor_factory.h"

namespace srsgnb {

class prach_detector_factory;
class task_executor;
class upper_phy_rx_results_notifier;

/// \brief Factory to create single executor uplink processors.
class uplink_processor_single_executor_factory : public uplink_processor_factory
{
public:
  uplink_processor_single_executor_factory(std::shared_ptr<prach_detector_factory> prach_factory_,
                                           task_executor&                          executor,
                                           upper_phy_rx_results_notifier&          results_notifier);

  // See interface for documentation.
  std::unique_ptr<uplink_processor> create(const uplink_processor_config& config) override;

private:
  std::shared_ptr<prach_detector_factory> prach_factory;
  task_executor&                          executor;
  upper_phy_rx_results_notifier&          results_notifier;
};

} // namespace srsgnb
