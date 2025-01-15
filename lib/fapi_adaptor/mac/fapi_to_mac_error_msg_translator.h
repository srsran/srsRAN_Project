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

#include "srsran/fapi/slot_error_message_notifier.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <functional>

namespace srsran {

class mac_cell_slot_handler;

namespace fapi_adaptor {

/// \brief FAPI-to-MAC error message translator.
///
/// This class listens to slot-based, error-specific FAPI message events and translates them to the suitable data types
/// for the MAC layer.
class fapi_to_mac_error_msg_translator : public fapi::slot_error_message_notifier
{
public:
  explicit fapi_to_mac_error_msg_translator(subcarrier_spacing scs_);

  // See interface for documentation.
  void on_error_indication(const fapi::error_indication_message& msg) override;

  /// Sets the given cell-specific slot handler. This handler will be used to receive error notifications.
  void set_cell_slot_handler(mac_cell_slot_handler& handler) { mac_slot_handler = std::ref(handler); }

private:
  const subcarrier_spacing                      scs;
  std::reference_wrapper<mac_cell_slot_handler> mac_slot_handler;
};

} // namespace fapi_adaptor
} // namespace srsran
