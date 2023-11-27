/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/fapi/slot_error_message_notifier.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace fapi {

/// Adds logging information over the implemented interface.
class logging_slot_error_notifier_decorator : public slot_error_message_notifier
{
public:
  logging_slot_error_notifier_decorator(srslog::basic_logger& logger_, slot_error_message_notifier& notifier_) :
    logger(logger_), notifier(notifier_)
  {
  }

  // See interface for documentation.
  void on_error_indication(const error_indication_message& msg) override;

private:
  /// FAPI logger.
  srslog::basic_logger& logger;
  /// Error notifier.
  slot_error_message_notifier& notifier;
};

} // namespace fapi
} // namespace srsran
