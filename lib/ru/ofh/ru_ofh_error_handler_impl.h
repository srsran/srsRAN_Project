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

#include "srsran/ofh/ofh_error_notifier.h"
#include "srsran/ru/ru_error_notifier.h"

namespace srsran {

/// Radio Unit error handler for the Open Fronthaul interface.
class ru_ofh_error_handler_impl : public ofh::error_notifier
{
public:
  explicit ru_ofh_error_handler_impl(ru_error_notifier& notifier_) : error_notifier(notifier_) {}

  // See interface for documentation.
  void on_late_downlink_message(const ofh::error_context& context) override;

  // See interface for documentation.
  void on_late_uplink_message(const ofh::error_context& context) override;

  // See interface for documentation.
  void on_late_prach_message(const ofh::error_context& context) override;

private:
  ru_error_notifier& error_notifier;
};

} // namespace srsran
