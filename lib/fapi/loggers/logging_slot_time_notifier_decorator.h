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

#include "srsran/fapi/slot_time_message_notifier.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace fapi {

/// Adds logging information over the implemented interface.
class logging_slot_time_notifier_decorator : public slot_time_message_notifier
{
public:
  explicit logging_slot_time_notifier_decorator(unsigned sector_id_, srslog::basic_logger& logger_);

  // See interface for documentation.
  void on_slot_indication(const slot_indication_message& msg) override;

  /// Sets the slot time message notifier to the given one.
  void set_slot_time_message_notifier(slot_time_message_notifier& time_notifier);

private:
  const unsigned              sector_id;
  srslog::basic_logger&       logger;
  slot_time_message_notifier* notifier;
};

} // namespace fapi
} // namespace srsran
