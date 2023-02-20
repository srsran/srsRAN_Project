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
#include "srsran/fapi/messages.h"
#include "srsran/mac/mac_cell_slot_handler.h"

using namespace srsran;
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

fapi_to_mac_time_msg_translator::fapi_to_mac_time_msg_translator(subcarrier_spacing scs_) :
  scs(scs_), mac_slot_handler(mac_dummy_handler)
{
}

void fapi_to_mac_time_msg_translator::on_slot_indication(const fapi::slot_indication_message& msg)
{
  mac_slot_handler.get().handle_slot_indication(slot_point(to_numerology_value(scs), msg.sfn, msg.slot));
}
