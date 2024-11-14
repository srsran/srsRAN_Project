/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/mac/bsr_config.h"
#include "srsran/mac/phr_config.h"
#include "srsran/ran/sr_configuration.h"
#include "srsran/ran/time_alignment_config.h"

namespace srsran {

/// \c MAC-CellGroupConfig, TS 38.331.
struct mac_cell_group_config {
  /// \c schedulingRequestConfig.
  std::vector<scheduling_request_to_addmod> scheduling_request_config;
  /// \c bsr-Config.
  std::optional<bsr_config> bsr_cfg;
  /// \c tag-Config.
  static_vector<time_alignment_group, MAX_NOF_TIME_ALIGNMENT_GROUPS> tag_config;
  /// \c phr-Config.
  std::optional<phr_config> phr_cfg;
  /// \c skipUplinkTxDynamic.
  bool skip_uplink_tx_dynamic;
  // TODO: add remaining fields.
};

} // namespace srsran
