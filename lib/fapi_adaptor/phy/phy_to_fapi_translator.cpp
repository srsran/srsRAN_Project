/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_to_fapi_translator.h"
#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/srslog/logger.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

namespace {

/// Dummy implementation of the slot_message_notifier interface.
class slot_time_message_notifier_dummy : public slot_time_message_notifier
{
public:
  void on_slot_indication(const slot_indication_message& msg) override {}
};

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

/// Instance used by \c phy_to_fapi_translator until it gets configured through the \c
/// set_fapi_slot_time_message_notifier method.
static slot_time_message_notifier_dummy dummy_time_notifier;

/// Instance used by \c phy_to_fapi_translator until it gets configured through the \c
/// set_fapi_slot_data_message_notifier method.
static slot_data_message_notifier_dummy dummy_data_notifier;

phy_to_fapi_translator::phy_to_fapi_translator() :
  time_notifier(dummy_time_notifier), data_notifier(dummy_data_notifier)
{
}

void phy_to_fapi_translator::handle_new_slot(slot_point slot)
{
  time_notifier.get().on_slot_indication(build_slot_indication_message(slot.sfn(), slot.slot_index()));
}

void phy_to_fapi_translator::set_fapi_slot_time_message_notifier(fapi::slot_time_message_notifier& fapi_time_notifier)
{
  time_notifier = std::ref(fapi_time_notifier);
}

void phy_to_fapi_translator::set_fapi_slot_data_message_notifier(slot_data_message_notifier& fapi_data_notifier)
{
  data_notifier = std::ref(fapi_data_notifier);
}

void phy_to_fapi_translator::on_new_prach_results(const ul_prach_results& result)
{
  rach_indication_message         msg;
  rach_indication_message_builder builder(msg);

  slot_point slot = result.context.slot;
  builder.set_basic_parameters(slot.sfn(), slot.slot_index());

  // NOTE: Currently not managing handle.
  static constexpr unsigned handle = 0U;
  // NOTE: Currently not supporting PRACH multiplexed in frequency domain.
  static constexpr unsigned   fd_ra_index = 0U;
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
