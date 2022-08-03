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
class slot_time_message_notifier_dummy : public slot_time_message_notifier
{
public:
  void on_slot_indication(const slot_indication_message& msg) override {}
};

} // namespace

/// Instance used by \c phy_to_fapi_translator until it gets configured through the \c
/// set_fapi_slot_time_message_notifier method.
static slot_time_message_notifier_dummy dummy_notifier;

phy_to_fapi_translator::phy_to_fapi_translator() : time_notifier(dummy_notifier) {}

void phy_to_fapi_translator::handle_new_slot(slot_point slot)
{
  time_notifier.get().on_slot_indication(build_slot_indication_message(slot.sfn(), slot.slot_index()));
}

void phy_to_fapi_translator::set_fapi_slot_time_message_notifier(fapi::slot_time_message_notifier& fapi_time_notifier)
{
  time_notifier = std::ref(fapi_time_notifier);
}
