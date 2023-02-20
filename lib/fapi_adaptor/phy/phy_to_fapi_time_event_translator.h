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

#include "srsgnb/fapi/slot_time_message_notifier.h"
#include "srsgnb/phy/upper/upper_phy_timing_notifier.h"
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
  /// FAPI slot-based, time-specific message notifier.
  std::reference_wrapper<fapi::slot_time_message_notifier> time_notifier;
  /// FAPI to PHY data translator.
  fapi_to_phy_translator& translator;
};

} // namespace fapi_adaptor
} // namespace srsran
