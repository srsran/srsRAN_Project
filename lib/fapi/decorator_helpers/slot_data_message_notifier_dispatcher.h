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

#include "srsran/fapi/slot_data_message_notifier.h"

namespace srsran {
namespace fapi {

/// Slot data message notifier dispatcher that forwards data messages to the configured notifier.
class slot_data_message_notifier_dispatcher : public slot_data_message_notifier
{
  slot_data_message_notifier* notifier = nullptr;

public:
  slot_data_message_notifier_dispatcher();

  // See interface for documentation.
  void on_rx_data_indication(const rx_data_indication_message& msg) override;

  // See interface for documentation.
  void on_crc_indication(const crc_indication_message& msg) override;

  // See interface for documentation.
  void on_uci_indication(const uci_indication_message& msg) override;

  // See interface for documentation.
  void on_srs_indication(const srs_indication_message& msg) override;

  // See interface for documentation.
  void on_rach_indication(const rach_indication_message& msg) override;

  /// Sets the slot data message notifier to the given one.
  void set_slot_data_message_notifier(slot_data_message_notifier& data_notifier);
};

} // namespace fapi
} // namespace srsran
