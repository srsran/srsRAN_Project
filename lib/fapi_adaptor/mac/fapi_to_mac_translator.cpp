/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "fapi_to_mac_translator.h"
#include "slot_event_dispatcher.h"
#include "srsgnb/fapi/messages.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

void fapi_to_mac_translator::on_dl_tti_response(const dl_tti_response_message& msg) {}

void fapi_to_mac_translator::on_slot_indication(const slot_indication_message& msg)
{
  slot_dispatcher.handle_new_slot(slot_point(static_cast<unsigned>(scs), msg.sfn, msg.slot));
}

void fapi_to_mac_translator::on_rx_data_indication(const rx_data_indication_message& msg) {}

void fapi_to_mac_translator::on_crc_indication(const crc_indication_message& msg) {}

void fapi_to_mac_translator::on_uci_indication(const uci_indication_message& msg) {}

void fapi_to_mac_translator::on_srs_indication(const srs_indication_message& msg) {}

void fapi_to_mac_translator::on_rach_indication(const rach_indication_message& msg) {}
