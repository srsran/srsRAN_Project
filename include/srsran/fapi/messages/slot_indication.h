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

namespace srsran {
namespace fapi {

/// Slot indication message.
struct slot_indication_message : public base_message {
  uint16_t sfn;
  uint16_t slot;
};

} // namespace fapi
} // namespace srsran
