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

#include "srsran/phy/upper/upper_phy_error_handler.h"
#include "srsran/phy/upper/upper_phy_error_notifier.h"
#include <functional>

namespace srsran {

/// Upper physical layer error handler implementation.
class upper_phy_error_handler_impl : public upper_phy_error_handler
{
public:
  upper_phy_error_handler_impl();

  // See interface for documentation.
  void handle_late_downlink_message(slot_point slot) override;

  /// Sets the given notifier as the error notifier of this handler.
  void set_error_notifier(upper_phy_error_notifier& notifier) { error_notifier = std::ref(notifier); }

private:
  std::reference_wrapper<upper_phy_error_notifier> error_notifier;
};

} // namespace srsran
