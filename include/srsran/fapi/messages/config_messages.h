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

#include "srsran/fapi/messages/base_message.h"
#include "srsran/fapi/messages/config_request_tlvs.h"
#include "srsran/fapi/messages/error_code.h"
#include <array>

namespace srsran {
namespace fapi {

/// Param request message.
struct param_request : public base_message {
  uint8_t protocol_version;
};

/// Param response message.
struct param_response : public base_message {
  error_code_id error_code;
  uint8_t       num_tlv;
};

/// Config request message.
struct config_request : public base_message {
  phy_config     phy_cfg;
  carrier_config carrier_cfg;
  cell_config    cell_cfg;
  prach_config   prach_cfg;
  ssb_config     ssb_cfg;
  tdd_phy_config tdd_cfg;
};

/// Config response message.
struct config_response : public base_message {
  /// Maximum number of invalid TLVs supported.
  static constexpr unsigned MAX_NUM_TLVS = 2048;

  error_code_id error_code;
};

/// Start request message.
struct start_request : public base_message {};

/// Start response message.
struct start_response : public base_message {};

/// Stop request message.
struct stop_request : public base_message {};

/// Stop indication message.
struct stop_indication : public base_message {};

} // namespace fapi
} // namespace srsran
