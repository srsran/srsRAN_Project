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

void phy_to_fapi_translator::handle_new_slot(slot_point slot)
{
  notifier.on_slot_indication(build_slot_indication_message(slot.sfn(), slot.slot_index()));
}
