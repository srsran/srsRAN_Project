/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_to_fapi_results_event_translator.h"
#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/fapi/message_validators.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

namespace {

class slot_data_message_notifier_dummy : public slot_data_message_notifier
{
public:
  void on_dl_tti_response(const dl_tti_response_message& msg) override {}
  void on_rx_data_indication(const rx_data_indication_message& msg) override {}
  void on_crc_indication(const crc_indication_message& msg) override {}
  void on_uci_indication(const uci_indication_message& msg) override {}
  void on_srs_indication(const srs_indication_message& msg) override {}
  void on_rach_indication(const rach_indication_message& msg) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the PHY-to-FAPI data event translator as a placeholder for the
/// actual data-specific notifier, which will be later set up through the \ref set_slot_data_message_notifier() method.
static slot_data_message_notifier_dummy dummy_data_notifier;

phy_to_fapi_results_event_translator::phy_to_fapi_results_event_translator() : data_notifier(dummy_data_notifier) {}

void phy_to_fapi_results_event_translator::on_new_prach_results(const ul_prach_results& result)
{
  rach_indication_message         msg;
  rach_indication_message_builder builder(msg);

  slot_point slot = result.context.slot;
  builder.set_basic_parameters(slot.sfn(), slot.slot_index());

  // NOTE: Currently not managing handle.
  static constexpr unsigned handle = 0U;
  // NOTE: Currently not supporting PRACH multiplexed in frequency domain.
  static constexpr unsigned fd_ra_index = 0U;

  rach_indication_pdu_builder builder_pdu = builder.add_pdu(
      handle, result.context.start_symbol, slot.slot_index(), fd_ra_index, result.result.rssi_dB, {}, {});

  for (const auto& preamble : result.result.preambles) {
    builder_pdu.add_preamble(
        preamble.preamble_index, {}, preamble.time_advance.to_seconds() * 1e9, preamble.power_dB, preamble.snr_dB);
  }

  error_type<validator_report> validation_result = validate_rach_indication(msg);
  if (!validation_result) {
    log_validator_report(validation_result.error());
    return;
  }

  data_notifier.get().on_rach_indication(msg);
}
