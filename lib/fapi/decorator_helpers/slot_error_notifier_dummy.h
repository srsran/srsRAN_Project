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

#include "srsran/fapi//slot_error_message_notifier.h"

namespace srsran {
namespace fapi {

/// Dummy FAPI slot error message notifier that will close the application if its methods are called.
class slot_error_message_notifier_dummy : public slot_error_message_notifier
{
public:
  // See interface for documentation.
  void on_error_indication(const error_indication_message& msg) override;
};

} // namespace fapi
} // namespace srsran
