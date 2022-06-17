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

using namespace srsgnb;
using namespace fapi_adaptor;

namespace {

/// Dummy implementation of the mac_cell_slot_handler interface.
class mac_cell_slot_handler_dummy : public mac_cell_slot_handler
{
public:
  void handle_slot_indication(slot_point sl_tx) override {}
};

} // namespace

/// Instance used when the slot_event_dispatcher, until it gets configured by \c set_mac_cell_slot_handler()
static mac_cell_slot_handler_dummy mac_dummy_handler;

slot_event_dispatcher::slot_event_dispatcher() : mac_slot_handler(mac_dummy_handler) {}

void slot_event_dispatcher::set_mac_cell_slot_handler(mac_cell_slot_handler& handler)
{
  mac_slot_handler = std::ref(handler);
}

void slot_event_dispatcher::handle_new_slot(slot_point slot)
{
  mac_slot_handler.get().handle_slot_indication(slot);
}
