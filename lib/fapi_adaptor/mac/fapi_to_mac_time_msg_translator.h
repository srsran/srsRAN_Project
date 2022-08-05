/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/fapi/slot_time_message_notifier.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include <functional>

namespace srsgnb {

class mac_cell_slot_handler;

namespace fapi_adaptor {

class mac_to_fapi_translator;

/// \brief FAPI-to-MAC timing message translator.
///
/// This class listens to slot-based, time-specific FAPI message events and translates them to the suitable data
/// types for the MAC layer.
class fapi_to_mac_time_msg_translator : public fapi::slot_time_message_notifier
{
public:
  fapi_to_mac_time_msg_translator(mac_to_fapi_translator& translator, subcarrier_spacing scs);

  // See interface for documentation.
  void on_slot_indication(const fapi::slot_indication_message& msg) override;

  /// Sets the given cell-specific slot handler. This handler will be used to receive new slot notifications.
  ///
  /// \param handler Handler to be set.
  void set_cell_slot_handler(mac_cell_slot_handler& handler) { mac_slot_handler = std::ref(handler); }

private:
  // :TODO: subcarrier spacing should be retrieved from the cells configuration in the future.
  const subcarrier_spacing                      scs;
  std::reference_wrapper<mac_cell_slot_handler> mac_slot_handler;
  mac_to_fapi_translator&                       translator;
};

} // namespace fapi_adaptor
} // namespace srsgnb
