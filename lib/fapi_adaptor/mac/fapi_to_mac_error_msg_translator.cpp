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

#include "fapi_to_mac_error_msg_translator.h"
#include "srsran/fapi/messages.h"
#include "srsran/mac/mac_cell_slot_handler.h"

using namespace srsran;
using namespace fapi_adaptor;

namespace {

class mac_cell_slot_handler_dummy : public mac_cell_slot_handler
{
public:
  void handle_slot_indication(slot_point sl_tx) override {}
  void handle_error_indication(slot_point sl_tx, error_event event) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the FAPI-to-MAC translator as a placeholder for the actual,
/// cell-specific MAC slot handler, which will be later set up through the \ref set_cell_slot_handler() method.
static mac_cell_slot_handler_dummy mac_dummy_handler;

fapi_to_mac_error_msg_translator::fapi_to_mac_error_msg_translator(subcarrier_spacing scs_) :
  scs(scs_), mac_slot_handler(mac_dummy_handler)
{
}

/// Converts a FAPI error indication error code to the corresponding MAC specific error event.
static mac_cell_slot_handler::error_event to_error_event(fapi::error_code_id code, fapi::message_type_id id)
{
  mac_cell_slot_handler::error_event mac_event{};

  switch (code) {
    case fapi::error_code_id::out_of_sync:
    case fapi::error_code_id::msg_slot_err:
      // DL_TTI.request OR UL_TTI.request.
      if (id == fapi::message_type_id::dl_tti_request) {
        mac_event.pdsch_discarded = true;
        mac_event.pdcch_discarded = true;
      } else {
        mac_event.pusch_and_pucch_discarded = true;
      }
      break;
    case fapi::error_code_id::msg_invalid_sfn:
      // UL_DCI.request OR Tx_Data.request.
      if (id == fapi::message_type_id::ul_dci_request) {
        mac_event.pdcch_discarded = true;
      } else {
        mac_event.pdsch_discarded = true;
      }
      break;
    case fapi::error_code_id::msg_ul_dci_err:
      // UL_DCI.request only.
      mac_event.pdcch_discarded = true;
      break;
    case fapi::error_code_id::msg_tx_err:
      // Tx_Data.request only.
      mac_event.pdsch_discarded = true;
      break;
    default:
      break;
  }

  return mac_event;
}

void fapi_to_mac_error_msg_translator::on_error_indication(const fapi::error_indication_message& msg)
{
  mac_slot_handler.get().handle_error_indication(slot_point(to_numerology_value(scs), msg.sfn, msg.slot),
                                                 to_error_event(msg.error_code, msg.message_id));
}
