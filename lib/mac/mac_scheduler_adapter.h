/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "mac_ctrl/mac_scheduler_configurator.h"
#include "mac_ul/mac_scheduler_ul_buffer_state_updater.h"
#include "srsran/scheduler/mac_scheduler.h"

namespace srsran {

class mac_scheduler_adapter : public mac_scheduler_configurator, public mac_scheduler_ul_buffer_state_updater
{
public:
  virtual ~mac_scheduler_adapter() = default;

  virtual void set_sched(mac_scheduler& sched) = 0;
};

} // namespace srsran