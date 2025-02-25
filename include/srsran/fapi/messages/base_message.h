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

/// Message type IDs.
enum class message_type_id : uint16_t {
  param_request      = 0x00,
  param_response     = 0x01,
  config_request     = 0x02,
  config_response    = 0x03,
  start_request      = 0x04,
  stop_request       = 0x05,
  stop_indication    = 0x06,
  error_indication   = 0x07,
  dl_tti_request     = 0x80,
  dl_tti_response    = 0x8a,
  ul_tti_request     = 0x81,
  slot_indication    = 0x82,
  ul_dci_request     = 0x83,
  tx_data_request    = 0x84,
  rx_data_indication = 0x85,
  crc_indication     = 0x86,
  uci_indication     = 0x87,
  srs_indication     = 0x88,
  rach_indication    = 0x89
};

/// Main PHY API message header.
struct message_header {
  uint8_t num_messages;
  uint8_t handle;
  uint8_t padding[2];
};

/// Common PHY API message structure.
struct base_message {
  message_type_id message_type;
  uint8_t         padding[2];
  uint32_t        length;
};

} // namespace fapi
} // namespace srsran
