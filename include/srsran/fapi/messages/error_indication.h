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
