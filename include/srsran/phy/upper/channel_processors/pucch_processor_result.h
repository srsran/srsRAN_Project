/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/optional.h"
#include "srsran/phy/upper/channel_processors/pucch_uci_message.h"
#include "srsran/phy/upper/channel_state_information.h"

namespace srsran {

/// Collects PUCCH processor results.
struct pucch_processor_result {
  /// Channel state information.
  channel_state_information csi;
  /// UCI message.
  pucch_uci_message message;
  /// Detection metric normalized with respect to the detection threshold (if applicable).
  optional<float> detection_metric;
};

} // namespace srsran
