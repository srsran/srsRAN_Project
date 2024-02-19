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
#include "srsran/phy/upper/upper_phy_error_notifier.h"
#include <functional>

namespace srsran {
namespace fapi_adaptor {

/// \brief PHY-to-FAPI error event translator.
///
/// This class listens to upper PHY error events and translates them into FAPI ERROR.indication messages that are sent
/// through the FAPI error-specific message notifier.
class phy_to_fapi_error_event_translator : public upper_phy_error_notifier
{
public:
  phy_to_fapi_error_event_translator();

  // See interface for documentation.
  void on_late_downlink_message(slot_point dl_frame_slot) override;

  /// Configures the FAPI slot-based, error-specific notifier to the given one.
  void set_slot_error_message_notifier(fapi::slot_error_message_notifier& fapi_error_notifier)
  {
    error_notifier = std::ref(fapi_error_notifier);
  }

private:
  /// Error indication notifier.
  std::reference_wrapper<fapi::slot_error_message_notifier> error_notifier;
};

} // namespace fapi_adaptor
} // namespace srsran
