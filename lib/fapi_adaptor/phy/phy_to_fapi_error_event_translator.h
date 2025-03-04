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

  // See interface for documentation.
  void on_late_uplink_message(slot_point ul_frame_slot) override;

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
