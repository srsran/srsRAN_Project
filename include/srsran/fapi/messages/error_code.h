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
