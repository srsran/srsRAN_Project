/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_to_fapi_time_event_translator.h"
#include "fapi_to_phy_translator.h"
#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/srslog/logger.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

namespace {

class slot_time_message_notifier_dummy : public slot_time_message_notifier
{
public:
  void on_slot_indication(const slot_indication_message& msg) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the PHY-to-FAPI time event translator as a placeholder for the
/// actual, time-specific notifier, which will be later set up through the \ref set_slot_time_message_notifier() method.
static slot_time_message_notifier_dummy dummy_time_notifier;

phy_to_fapi_time_event_translator::phy_to_fapi_time_event_translator(fapi_to_phy_translator& translator) :
  time_notifier(dummy_time_notifier), translator(translator)
{
}

void phy_to_fapi_time_event_translator::on_tti_boundary(slot_point slot)
{
  translator.handle_new_slot(slot);

  // Delivering the slot.indication message must always be the last step.
  time_notifier.get().on_slot_indication(build_slot_indication_message(slot.sfn(), slot.slot_index()));
}
