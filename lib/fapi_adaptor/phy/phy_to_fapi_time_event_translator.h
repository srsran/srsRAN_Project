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

#include "srsran/fapi/slot_time_message_notifier.h"
#include "srsran/phy/upper/upper_phy_timing_notifier.h"
#include <functional>

namespace srsran {
namespace fapi_adaptor {

class fapi_to_phy_translator;

/// \brief PHY-to-FAPI time event translator.
///
/// This class listens to upper PHY timing events and translates them into FAPI indication messages that are sent
/// through the FAPI time-specific message notifier.
class phy_to_fapi_time_event_translator : public upper_phy_timing_notifier
{
public:
  /// \brief Constructor for the PHY-to-FAPI time event translator.
  ///
  /// \param[in] translator_ FAPI-to-PHY translator.
  explicit phy_to_fapi_time_event_translator(fapi_to_phy_translator& translator_);

  // See interface for documentation.
  void on_tti_boundary(slot_point slot) override;

  /// \brief Configures the FAPI slot-based, time-specific notifier to the given one.
  void set_slot_time_message_notifier(fapi::slot_time_message_notifier& fapi_time_slot_notifier)
  {
    time_notifier = std::ref(fapi_time_slot_notifier);
  }

private:
  /// FAPI to PHY data translator.
  fapi_to_phy_translator& translator;
  /// FAPI slot-based, time-specific message notifier.
  std::reference_wrapper<fapi::slot_time_message_notifier> time_notifier;
};

} // namespace fapi_adaptor
} // namespace srsran
