/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "unittests/f1c/cu_cp/f1_cu_test_helpers.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/rrc/rrc.h"

namespace srsgnb {
namespace srs_cu_cp {

f1_setup_request_message generate_valid_f1_setup_request_message()
{
  f1c_message              f1setup_msg          = generate_f1_setup_request();
  f1_setup_request_message f1_setup_request_msg = {};
  f1_setup_request_msg.request                  = f1setup_msg.pdu.init_msg().value.f1_setup_request();
  return f1_setup_request_msg;
}

f1_setup_request_message generate_f1_setup_request_message_base()
{
  f1c_message f1setup_msg                                                               = generate_f1_setup_request();
  f1setup_msg.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list_present = false;
  f1setup_msg.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list->clear();
  f1_setup_request_message f1_setup_request_msg = {};
  f1_setup_request_msg.request                  = f1setup_msg.pdu.init_msg().value.f1_setup_request();
  return f1_setup_request_msg;
}

ue_creation_message generate_valid_ue_creation_message(rnti_t c_rnti, unsigned nrcell_id)
{
  ue_creation_message ue_creation_msg = {};
  ue_creation_msg.c_rnti              = c_rnti;
  asn1::f1ap::nrcgi_s asn1_cgi;
  asn1_cgi.nrcell_id.from_number(nrcell_id);
  asn1_cgi.plmn_id.from_string("02f899");
  ue_creation_msg.cgi = cgi_from_asn1(asn1_cgi);
  ue_creation_msg.du_to_cu_rrc_container.from_string(
      "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
      "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
      "c400");

  return ue_creation_msg;
}

ue_context_release_command_message generate_ue_context_release_command(ue_index_t ue_index)
{
  ue_context_release_command_message ue_context_release_command_msg = {};
  ue_context_release_command_msg.ue_index                           = ue_index;
  ue_context_release_command_msg.cause                              = ue_context_release_cause::radio_network;
  return ue_context_release_command_msg;
}

} // namespace srs_cu_cp
} // namespace srsgnb
