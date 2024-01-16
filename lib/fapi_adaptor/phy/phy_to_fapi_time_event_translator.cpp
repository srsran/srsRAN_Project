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

#include "phy_to_fapi_time_event_translator.h"
#include "fapi_to_phy_translator.h"
#include "srsran/fapi/message_builders.h"
#include "srsran/fapi/message_validators.h"

using namespace srsran;
using namespace fapi_adaptor;

namespace {

class slot_time_message_notifier_dummy : public fapi::slot_time_message_notifier
{
public:
  void on_slot_indication(const fapi::slot_indication_message& msg) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the PHY-to-FAPI time event translator as a placeholder for the
/// actual time-specific notifier, which will be later set up through the \ref set_slot_time_message_notifier() method.
static slot_time_message_notifier_dummy dummy_time_notifier;

phy_to_fapi_time_event_translator::phy_to_fapi_time_event_translator(fapi_to_phy_translator& translator_) :
  translator(translator_), time_notifier(dummy_time_notifier)
{
}

void phy_to_fapi_time_event_translator::on_tti_boundary(slot_point slot)
{
  translator.handle_new_slot(slot);

  // Delivering the slot.indication message must always be the last step.
  time_notifier.get().on_slot_indication(fapi::build_slot_indication_message(slot.sfn(), slot.slot_index()));
}
