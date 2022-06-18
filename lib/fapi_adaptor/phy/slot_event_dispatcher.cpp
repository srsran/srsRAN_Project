/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "slot_event_dispatcher.h"
#include "fapi_to_phy_translator.h"
#include "phy_to_fapi_translator.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

slot_event_dispatcher::slot_event_dispatcher(phy_to_fapi_translator& phy_translator,
                                             fapi_to_phy_translator& fapi_translator) :
  phy_translator(phy_translator), fapi_translator(fapi_translator)
{
}

void slot_event_dispatcher::on_tti_boundary(slot_point slot)
{
  fapi_translator.handle_new_slot(slot);

  // Passing it to the PHY to FAPI translator must be the last step.
  phy_translator.handle_new_slot(slot);
}
