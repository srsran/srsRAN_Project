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
#include "srsran/fapi/messages/error_code.h"

namespace srsran {
namespace fapi {

/// Encodes the error indication message.
struct error_indication_message : public base_message {
  uint16_t        sfn;
  uint16_t        slot;
  message_type_id message_id;
  error_code_id   error_code;
  uint16_t        expected_sfn;
  uint16_t        expected_slot;
};

} // namespace fapi
} // namespace srsran
