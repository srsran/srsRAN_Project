/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/scheduler/config/scheduler_expert_config.h"
#include "srsgnb/scheduler/scheduler_configurator.h"
#include "srsgnb/scheduler/scheduler_metrics.h"

namespace srsgnb {

/// \brief Configuration used to create a scheduler instance.
struct scheduler_config {
  const scheduler_expert_config& expert_params;
  sched_configuration_notifier&  config_notifier;
  scheduler_ue_metrics_notifier& metrics_notifier;
};

} // namespace srsgnb