/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/scheduler/scheduler_factory.h"
#include "scheduler_impl.h"

using namespace srsgnb;

std::unique_ptr<mac_scheduler> srsgnb::create_scheduler(sched_configuration_notifier& config_notifier)
{
  return std::make_unique<scheduler_impl>(config_notifier);
}
