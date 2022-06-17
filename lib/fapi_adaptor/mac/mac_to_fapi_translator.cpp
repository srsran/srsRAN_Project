/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_to_fapi_translator.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/fapi_adaptor/mac/messages/ssb.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

static void add_ssb_pdus_to_request(dl_tti_request_message_builder& builder, const mac_dl_sched_result& dl_res)
{
  for (const auto& pdu : dl_res.ssb_pdu) {
    dl_ssb_pdu_builder ssb_builder = builder.add_ssb_pdu();
    convert_ssb_mac_to_fapi(ssb_builder, pdu);
  }
}

void mac_to_fapi_translator::on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res)
{
  // :TODO: should we manage here 2 different numerologies in the same dl_res?
  dl_tti_request_message         msg;
  dl_tti_request_message_builder builder(msg);

  // FAPI implementation doesn't support groups at the moment.
  static const unsigned num_groups = 0;
  builder.set_basic_parameters(dl_res.slot.sfn(), dl_res.slot.slot_index(), num_groups);

  // Add SSB PDUs to the DL_TTI.request.
  add_ssb_pdus_to_request(builder, dl_res);

  // Validate that the DL_TTI.request message is correct.
  error_type<validator_report> result = validate_dl_tti_request(msg);

  if (!result) {
    // :TODO: log errors,

    return;
  }

  gateway.dl_tti_request(msg);
}

void mac_to_fapi_translator::on_new_downlink_data(const mac_dl_data_result& dl_data) {}

void mac_to_fapi_translator::on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) {}
