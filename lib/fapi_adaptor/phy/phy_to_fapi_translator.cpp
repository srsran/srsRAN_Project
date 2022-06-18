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

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

namespace {

/// Dummy implementation of the slot_message_notifier interface.
class slot_message_notifier_dummy : public slot_message_notifier
{
public:
  void on_dl_tti_response(const dl_tti_response_message& msg) override {}
  void on_slot_indication(const slot_indication_message& msg) override {}
  void on_rx_data_indication(const rx_data_indication_message& msg) override {}
  void on_crc_indication(const crc_indication_message& msg) override {}
  void on_uci_indication(const uci_indication_message& msg) override {}
  void on_srs_indication(const srs_indication_message& msg) override {}
  void on_rach_indication(const rach_indication_message& msg) override {}
};

} // namespace

/// Instance used by the phy_to_fapi_translator class until it gets configured through the \c
/// set_slot_message_notifier() method.
static slot_message_notifier_dummy dummy_notifier;

phy_to_fapi_translator::phy_to_fapi_translator() : notifier(dummy_notifier) {}

void phy_to_fapi_translator::handle_new_slot(slot_point slot)
{
  notifier.get().on_slot_indication(build_slot_indication_message(slot.sfn(), slot.slot_index()));
}

void phy_to_fapi_translator::set_slot_message_notifier(slot_message_notifier& fapi_slot_notifier)
{
  notifier = std::ref(fapi_slot_notifier);
}
