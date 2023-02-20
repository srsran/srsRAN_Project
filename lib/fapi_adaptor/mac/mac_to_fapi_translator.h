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

#include "srsgnb/mac/mac_cell_result.h"

namespace srsran {

namespace fapi {
class slot_last_message_notifier;
class slot_message_gateway;
} // namespace fapi

namespace fapi_adaptor {

/// \brief MAC-to-FAPI translator.
///
/// This class listens to cell-specific MAC events carrying scheduling results and translates them into FAPI messages
/// that are sent through the FAPI message gateway.
class mac_to_fapi_translator : public mac_cell_result_notifier
{
public:
  mac_to_fapi_translator(fapi::slot_message_gateway& msg_gw_, fapi::slot_last_message_notifier& last_msg_notifier_) :
    msg_gw(msg_gw_), last_msg_notifier(last_msg_notifier_)
  {
  }

  // See interface for documentation.
  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override;

  // See interface for documentation.
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override;

  // See interface for documentation.
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override;

  // See interface for documentation.
  void on_cell_results_completion(slot_point slot) override;

private:
  /// Handles the UL_DCI.request message.
  void
  handle_ul_dci_request(span<const pdcch_ul_information> pdcch_info, span<const dci_payload> payloads, slot_point slot);

private:
  /// FAPI message gateway to the outside world.
  fapi::slot_message_gateway& msg_gw;
  /// Slot-specific last message notifier.
  fapi::slot_last_message_notifier& last_msg_notifier;
};

} // namespace fapi_adaptor
} // namespace srsran
