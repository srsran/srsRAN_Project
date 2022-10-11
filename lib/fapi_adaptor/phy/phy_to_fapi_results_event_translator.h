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

#include "srsgnb/fapi/slot_data_message_notifier.h"
#include "srsgnb/phy/upper/upper_phy_rx_results_notifier.h"
#include <functional>

namespace srsgnb {
namespace fapi_adaptor {

/// \brief PHY-to-FAPI uplink results events translator.
///
/// This class listens to upper PHY uplink result events and translates them into FAPI indication messages that are sent
/// through the FAPI data-specific message notifier.
class phy_to_fapi_results_event_translator : public upper_phy_rx_results_notifier
{
public:
  phy_to_fapi_results_event_translator();

  // See interface for documentation.
  void on_new_prach_results(const ul_prach_results& result) override;

  // See interface for documentation.
  void on_new_pusch_results(const ul_pusch_results& result) override;

  // See interface for documentation.
  void on_new_pucch_results(const ul_pucch_results& result) override;

  /// Configures the FAPI slot-based, data-specific notifier to the given one.
  void set_slot_data_message_notifier(fapi::slot_data_message_notifier& fapi_data_slot_notifier)
  {
    data_notifier = std::ref(fapi_data_slot_notifier);
  }

private:
  /// Notifies a new FAPI \e CRC.indication through the data notifier.
  void notify_crc_indication(const ul_pusch_results& result);

  /// Notifies a new FAPI \e Rx_Data.indication through the data notifier.
  void notify_rx_data_indication(const ul_pusch_results& result);

private:
  /// FAPI slot-based, data-specific message notifier.
  std::reference_wrapper<fapi::slot_data_message_notifier> data_notifier;
  srslog::basic_logger&                                    logger;
};

} // namespace fapi_adaptor
} // namespace srsgnb
