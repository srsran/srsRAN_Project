/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "logical_channel_list_config.h"

namespace srsran {

struct sched_ue_config_request;

struct ue_creation_params {
  const sched_ue_config_request&  cfg_req;
  logical_channel_config_list_ptr lc_ch_list;
};

struct ue_reconfig_params {
  const sched_ue_config_request&                 cfg_req;
  std::optional<logical_channel_config_list_ptr> lc_ch_list;
};

} // namespace srsran
