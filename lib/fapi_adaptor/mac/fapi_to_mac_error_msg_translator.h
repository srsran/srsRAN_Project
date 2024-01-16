/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
