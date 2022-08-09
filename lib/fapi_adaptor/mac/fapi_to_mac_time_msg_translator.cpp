/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "fapi_to_mac_time_msg_translator.h"
#include "mac_to_fapi_translator.h"
#include "srsgnb/fapi/messages.h"
#include "srsgnb/mac/mac_cell_slot_handler.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

namespace {

class mac_cell_slot_handler_dummy : public mac_cell_slot_handler
{
public:
  void handle_slot_indication(slot_point sl_tx) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the FAPI-to-MAC translator as a placeholder for the actual,
/// cell-specific MAC slot handler, which will be later set up through the \ref set_cell_slot_handler() method.
static mac_cell_slot_handler_dummy mac_dummy_handler;

fapi_to_mac_time_msg_translator::fapi_to_mac_time_msg_translator(mac_to_fapi_translator& translator,
                                                                 subcarrier_spacing      scs) :
  scs(scs), mac_slot_handler(mac_dummy_handler), translator(translator)
{
}

void fapi_to_mac_time_msg_translator::on_slot_indication(const slot_indication_message& msg)
{
  translator.handle_new_slot();
  mac_slot_handler.get().handle_slot_indication(slot_point(to_numerology_value(scs), msg.sfn, msg.slot));
}
