/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "bsr_format.h"
#include "phr_config.h"
#include "time_alignment_group_config.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/sr_configuration.h"

namespace srsran {

/// \c MAC-CellGroupConfig, TS 38.331.
struct mac_cell_group_config {
  /// \c schedulingRequestConfig.
  std::vector<scheduling_request_to_addmod> scheduling_request_config;
  /// \c bsr-Config.
  optional<bsr_config> bsr_cfg;
  /// \c tag-Config.
  static_vector<tag, MAX_NOF_TAGS> tag_config;
  /// \c phr-Config.
  optional<phr_config> phr_cfg;
  /// \c skipUplinkTxDynamic.
  bool skip_uplink_tx_dynamic;
  // TODO: add remaining fields.
};

} // namespace srsran
