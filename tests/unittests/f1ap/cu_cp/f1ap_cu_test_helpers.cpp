/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "f1ap_cu_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/asn1/rrc_nr/dl_ccch_msg.h"
#include "srsran/asn1/rrc_nr/dl_ccch_msg_ies.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_configuration_update.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_factory.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/qos/five_qi.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace srs_cu_cp;

gnb_cu_ue_f1ap_id_t srsran::srs_cu_cp::generate_random_gnb_cu_ue_f1ap_id()
{
  return int_to_gnb_cu_ue_f1ap_id(test_rgen::uniform_int<uint64_t>(
      gnb_cu_ue_f1ap_id_to_uint(gnb_cu_ue_f1ap_id_t::min), gnb_cu_ue_f1ap_id_to_uint(gnb_cu_ue_f1ap_id_t::max) - 1));
}

f1ap_cu_test::f1ap_cu_test(const f1ap_configuration& f1ap_cfg)
{
  test_logger.set_level(srslog::basic_levels::debug);
  f1ap_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  // We enable Json logging by default for the purpose of testing.
  f1ap_configuration tmp = f1ap_cfg;
  tmp.json_log_enabled   = true;

  f1ap = create_f1ap(tmp, f1ap_pdu_notifier, du_processor_notifier, timers, ctrl_worker);
}

f1ap_cu_test::~f1ap_cu_test()
{
  // flush logger after each test
  srslog::flush();
}

f1ap_cu_test::test_ue& f1ap_cu_test::create_ue(gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message msg = test_helpers::generate_init_ul_rrc_message_transfer(
      du_ue_id, to_rnti(0x4601), byte_buffer::create({0x1, 0x2, 0x3, 0x4}).value());
  f1ap->handle_message(msg);
  ue_index_t ue_index = *du_processor_notifier.last_created_ue_index;
  test_ues.emplace(ue_index, test_ue{ue_index});
  test_ues[ue_index].ue_index = ue_index;
  test_ues[ue_index].du_ue_id = du_ue_id;
  return test_ues[ue_index];
}

f1ap_cu_test::test_ue& f1ap_cu_test::run_ue_context_setup()
{
  f1ap_ue_context_setup_request req = create_ue_context_setup_request({});

  // Start procedure in CU.
  async_task<f1ap_ue_context_setup_response>         t = f1ap->handle_ue_context_setup_request(req, {});
  lazy_task_launcher<f1ap_ue_context_setup_response> t_launcher(t);

  // Take allocated CU ID from UE context setup request.
  gnb_cu_ue_f1ap_id_t cu_ue_id = int_to_gnb_cu_ue_f1ap_id(
      this->f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.ue_context_setup_request()->gnb_cu_ue_f1ap_id);

  // Generate random DU ID
  gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(test_rgen::uniform_int<uint32_t>());

  // Handle response from DU.
  f1ap_message response = test_helpers::generate_ue_context_setup_response(cu_ue_id, du_ue_id);
  f1ap->handle_message(response);

  srsran_assert(t.ready(), "The procedure should have completed by now");

  // Create test UE using identifiers allocated from precedure.
  ue_index_t ue_index = t.get().ue_index;
  test_ues.emplace(ue_index, test_ue{ue_index});
  test_ues[ue_index].ue_index = ue_index;
  test_ues[ue_index].cu_ue_id = cu_ue_id;
  test_ues[ue_index].du_ue_id = du_ue_id;
  return test_ues[ue_index];
}

bool f1ap_cu_test::was_rrc_reject_sent()
{
  // Make sure that the last message was a DLRRCMessageTransfer
  if (f1ap_pdu_notifier.last_f1ap_msg.pdu.type().value != asn1::f1ap::f1ap_pdu_c::types::init_msg) {
    return false;
  }
  if (f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.type().value !=
      asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types::dl_rrc_msg_transfer) {
    return false;
  }

  // Make sure that the DLRRCMessageTransfer contains an RRC Reject.
  asn1::rrc_nr::dl_ccch_msg_s ccch;
  {
    asn1::cbit_ref bref{f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.dl_rrc_msg_transfer()->rrc_container};
    if (ccch.unpack(bref) != asn1::SRSASN_SUCCESS) {
      return false;
    }
  }
  return ccch.msg.c1().type().value == asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types_opts::rrc_reject;
}

void f1ap_cu_test::tick()
{
  this->timers.tick();
  this->ctrl_worker.run_pending_tasks();
}

f1ap_ue_context_setup_request
srsran::srs_cu_cp::create_ue_context_setup_request(const std::initializer_list<drb_id_t>& drbs_to_add)
{
  f1ap_ue_context_setup_request req;

  req.ue_index = uint_to_ue_index(19);

  // sp cell id
  req.sp_cell_id.nci     = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
  req.sp_cell_id.plmn_id = plmn_identity::test_value();

  // serv cell idx
  req.serv_cell_idx = 1;

  // cu to du to rrc info
  req.cu_to_du_rrc_info.cg_cfg_info               = make_byte_buffer("deadbeef").value();
  req.cu_to_du_rrc_info.ue_cap_rat_container_list = make_byte_buffer("deadbeef").value();
  req.cu_to_du_rrc_info.meas_cfg                  = make_byte_buffer("deadbeef").value();

  // drx cycle
  f1ap_drx_cycle drx_cycle;
  drx_cycle.long_drx_cycle_len    = 5;
  drx_cycle.short_drx_cycle_len   = 1;
  drx_cycle.short_drx_cycle_timer = 10;
  req.drx_cycle                   = drx_cycle;

  for (const auto& drb : drbs_to_add) {
    f1ap_drb_to_setup drb_item;
    drb_item.drb_id                                               = drb;
    drb_item.qos_info.drb_qos.alloc_retention_prio.prio_level_arp = 1;
    drb_item.qos_info.s_nssai.sst                                 = slice_service_type{1};
    drb_item.mode                                                 = rlc_mode::am;
    drb_item.pdcp_sn_len                                          = pdcp_sn_size::size12bits;

    req.drbs_to_be_setup_list.push_back(drb_item);
  }
  return req;
}

f1ap_gnb_cu_configuration_update srsran::srs_cu_cp::create_gnb_cu_configuration_update()
{
  f1ap_gnb_cu_configuration_update req;

  req.cells_to_be_activated_list = {
      {nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(0).value()}, MIN_PCI},
      {nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(1).value()}, MIN_PCI + 1}};

  req.cells_to_be_deactivated_list = {
      {nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(2).value()}}};

  req.gnb_cu_name = "srs_cu_cp";

  return req;
}
