/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include <cstdint>

namespace srsran {
namespace fapi {

/// Encodes the error codes.
enum class error_code_id : uint8_t {
  msg_ok                               = 0x0,
  msg_invalid_state                    = 0x1,
  msg_invalid_config                   = 0x2,
  out_of_sync                          = 0x3,
  msg_slot_err                         = 0x4,
  msg_bch_missing                      = 0x5,
  msg_invalid_sfn                      = 0x6,
  msg_ul_dci_err                       = 0x7,
  msg_tx_err                           = 0x8,
  msg_invalid_phy_id                   = 0x9,
  msg_uninstantiated_phy               = 0xa,
  msg_invalid_dfe_profile              = 0xb,
  phy_profile_incompatible_running_phy = 0xc
};

} // namespace fapi
} // namespace srsran
