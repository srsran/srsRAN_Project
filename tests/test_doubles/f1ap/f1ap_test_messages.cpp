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

#include "f1ap_test_messages.h"
#include "../lib/f1ap/asn1_helpers.h"
#include "../pdcp/pdcp_pdu_generator.h"
#include "../rrc/rrc_packed_test_messages.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/ran/positioning/positioning_ids.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace asn1::f1ap;

f1ap_message srsran::test_helpers::generate_f1ap_reset_message(
    const std::vector<std::pair<std::optional<gnb_du_ue_f1ap_id_t>, std::optional<gnb_cu_ue_f1ap_id_t>>>& ues_to_reset)
{
  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_RESET);

  asn1::f1ap::reset_s& reset = msg.pdu.init_msg().value.reset();
  reset->cause.set_misc();
  reset->cause.misc().value = asn1::f1ap::cause_misc_opts::unspecified;

  if (ues_to_reset.empty()) {
    auto& f1 = reset->reset_type.set_f1_interface();
    f1.value = asn1::f1ap::reset_all_opts::reset_all;
  } else {
    auto& lst = reset->reset_type.set_part_of_f1_interface();
    lst.resize(ues_to_reset.size());
    for (unsigned i = 0; i != ues_to_reset.size(); ++i) {
      lst[i].load_info_obj(ASN1_F1AP_ID_UE_ASSOCIATED_LC_F1_CONN_ITEM);
      auto& conn                     = lst[i].value().ue_associated_lc_f1_conn_item();
      conn.gnb_du_ue_f1ap_id_present = ues_to_reset[i].first.has_value();
      if (conn.gnb_du_ue_f1ap_id_present) {
        conn.gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_to_uint(ues_to_reset[i].first.value());
      }
      conn.gnb_cu_ue_f1ap_id_present = ues_to_reset[i].second.has_value();
      if (conn.gnb_cu_ue_f1ap_id_present) {
        conn.gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(ues_to_reset[i].second.value());
      }
    }
  }

  return msg;
}

static byte_buffer generate_rrc_container(uint32_t pdcp_sn, unsigned pdu_len)
{
  return test_helpers::create_pdcp_pdu(
      pdcp_sn_size::size12bits, true, pdcp_sn, pdu_len, test_rgen::uniform_int<uint8_t>());
}

gnb_du_served_cells_item_s srsran::test_helpers::generate_served_cells_item(const served_cell_item_info& info)
{
  gnb_du_served_cells_item_s served_cells_item;
  served_cells_item.served_cell_info.nr_cgi.plmn_id.from_string("00f110");
  served_cells_item.served_cell_info.nr_cgi.nr_cell_id.from_number(info.nci.value());
  served_cells_item.served_cell_info.nr_pci              = info.pci;
  served_cells_item.served_cell_info.five_gs_tac_present = true;
  served_cells_item.served_cell_info.five_gs_tac.from_number(info.tac);

  served_plmns_item_s served_plmn;
  served_plmn.plmn_id.from_string("00f110");
  slice_support_item_s slice_support_item;
  slice_support_item.snssai.sst.from_number(1);
  served_plmn.ie_exts.tai_slice_support_list_present = true;
  served_plmn.ie_exts.tai_slice_support_list.push_back(slice_support_item);
  served_cells_item.served_cell_info.served_plmns.push_back(served_plmn);

  served_cells_item.served_cell_info.nr_mode_info.set_tdd();
  served_cells_item.served_cell_info.nr_mode_info.tdd().nr_freq_info.nr_arfcn = info.nr_arfcn;
  freq_band_nr_item_s freq_band_nr_item;
  freq_band_nr_item.freq_band_ind_nr = static_cast<uint16_t>(info.band);
  served_cells_item.served_cell_info.nr_mode_info.tdd().nr_freq_info.freq_band_list_nr.push_back(freq_band_nr_item);
  served_cells_item.served_cell_info.nr_mode_info.tdd().tx_bw.nr_scs.value = nr_scs_opts::scs30;
  served_cells_item.served_cell_info.nr_mode_info.tdd().tx_bw.nr_nrb.value = nr_nrb_opts::nrb51;
  served_cells_item.served_cell_info.meas_timing_cfg =
      create_meas_timing_cfg(info.meas_timing_cfg.carrier_freq, info.meas_timing_cfg.scs);

  served_cells_item.gnb_du_sys_info_present = true;
  served_cells_item.gnb_du_sys_info.mib_msg.from_string("01c586");
  served_cells_item.gnb_du_sys_info.sib1_msg.from_string(info.sib1_str);

  return served_cells_item;
}

f1ap_message srsran::test_helpers::generate_f1_setup_request(gnb_du_id_t                               gnb_du_id,
                                                             const std::vector<served_cell_item_info>& cells)
{
  f1ap_message msg;
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_req                = msg.pdu.init_msg().value.f1_setup_request();
  setup_req->transaction_id      = 99;
  setup_req->gnb_du_id           = (uint64_t)gnb_du_id;
  setup_req->gnb_du_name_present = true;
  setup_req->gnb_du_name.from_string("srsDU");
  setup_req->gnb_du_rrc_version.latest_rrc_version.from_number(1);
  setup_req->gnb_du_served_cells_list_present = true;
  setup_req->gnb_du_served_cells_list.resize(cells.size());
  for (unsigned i = 0; i != cells.size(); ++i) {
    setup_req->gnb_du_served_cells_list[i].load_info_obj(ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_ITEM);
    setup_req->gnb_du_served_cells_list[i].value().gnb_du_served_cells_item() = generate_served_cells_item(cells[i]);
  }

  return msg;
}

f1ap_message srsran::test_helpers::generate_f1_setup_response(const f1ap_message& f1_setup_request, bool activate_cells)
{
  srsran_assert(f1_setup_request.pdu.type().value == f1ap_pdu_c::types_opts::init_msg, "Expected F1 setup request");
  srsran_assert(f1_setup_request.pdu.init_msg().value.type().value ==
                    f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request,
                "Expected F1 setup request");
  auto& req = f1_setup_request.pdu.init_msg().value.f1_setup_request();

  f1ap_message resp;
  resp.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  f1_setup_resp_s& f1_setup_resp = resp.pdu.successful_outcome().value.f1_setup_resp();

  f1_setup_resp->transaction_id      = req->transaction_id;
  f1_setup_resp->gnb_cu_name_present = true;
  f1_setup_resp->gnb_cu_name.from_string("srscu");
  f1_setup_resp->gnb_cu_rrc_version.latest_rrc_version.from_number(2);

  f1_setup_resp->cells_to_be_activ_list_present = activate_cells;
  if (activate_cells) {
    f1_setup_resp->cells_to_be_activ_list.resize(req->gnb_du_served_cells_list.size());
    for (unsigned i = 0; i != req->gnb_du_served_cells_list.size(); ++i) {
      auto& req_cell = req->gnb_du_served_cells_list[i]->gnb_du_served_cells_item();
      f1_setup_resp->cells_to_be_activ_list[i].load_info_obj(ASN1_F1AP_ID_CELLS_TO_BE_ACTIV_LIST_ITEM);
      auto& cell          = f1_setup_resp->cells_to_be_activ_list[i].value().cells_to_be_activ_list_item();
      cell.nr_cgi         = req_cell.served_cell_info.nr_cgi;
      cell.nr_pci_present = true;
      cell.nr_pci         = req_cell.served_cell_info.nr_pci;
    }
  }

  return resp;
}

f1ap_message srsran::test_helpers::generate_f1_setup_failure(const f1ap_message& f1_setup_request)
{
  srsran_assert(f1_setup_request.pdu.type().value == f1ap_pdu_c::types_opts::init_msg, "Expected F1 setup request");
  srsran_assert(f1_setup_request.pdu.init_msg().value.type().value ==
                    f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request,
                "Expected F1 setup request");
  auto& req = f1_setup_request.pdu.init_msg().value.f1_setup_request();

  f1ap_message resp;
  resp.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  f1_setup_fail_s& f1_setup_fail = resp.pdu.unsuccessful_outcome().value.f1_setup_fail();

  f1_setup_fail->transaction_id = req->transaction_id;
  f1_setup_fail->cause.set(cause_c::types_opts::misc);
  f1_setup_fail->cause.misc().value = cause_misc_opts::unspecified;

  return resp;
}

f1ap_message
srsran::test_helpers::generate_gnb_du_configuration_update_acknowledge(const f1ap_message& gnb_du_config_update)
{
  const gnb_du_cfg_upd_s& req = gnb_du_config_update.pdu.init_msg().value.gnb_du_cfg_upd();

  f1ap_message msg;
  msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_GNB_DU_CFG_UPD);

  asn1::f1ap::gnb_du_cfg_upd_ack_s& ack = msg.pdu.successful_outcome().value.gnb_du_cfg_upd_ack();
  ack->transaction_id                   = req->transaction_id;

  return msg;
}

f1ap_message
srsran::test_helpers::generate_gnb_du_configuration_update_failure(const f1ap_message& gnb_du_config_update)
{
  const gnb_du_cfg_upd_s& req = gnb_du_config_update.pdu.init_msg().value.gnb_du_cfg_upd();

  f1ap_message msg;
  msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_GNB_DU_CFG_UPD);
  asn1::f1ap::gnb_du_cfg_upd_fail_s& fail = msg.pdu.unsuccessful_outcome().value.gnb_du_cfg_upd_fail();
  fail->transaction_id                    = req->transaction_id;
  fail->cause.set_misc().value            = cause_misc_opts::unspecified;

  return msg;
}

f1ap_message
srsran::test_helpers::generate_gnb_cu_configuration_update_request(unsigned                        transaction_id,
                                                                   span<const nr_cell_global_id_t> cgis_to_activate,
                                                                   span<const nr_cell_global_id_t> cgis_to_deactivate)
{
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_GNB_CU_CFG_UPD);
  gnb_cu_cfg_upd_s& req = msg.pdu.init_msg().value.gnb_cu_cfg_upd();
  req->transaction_id   = transaction_id;

  req->cells_to_be_activ_list_present = not cgis_to_activate.empty();
  req->cells_to_be_activ_list.resize(cgis_to_activate.size());
  for (unsigned i = 0, e = cgis_to_activate.size(); i != e; ++i) {
    req->cells_to_be_activ_list[i].load_info_obj(ASN1_F1AP_ID_CELLS_TO_BE_ACTIV_LIST_ITEM);
    req->cells_to_be_activ_list[i].value().cells_to_be_activ_list_item().nr_cgi = cgi_to_asn1(cgis_to_activate[i]);
  }
  req->cells_to_be_deactiv_list_present = not cgis_to_deactivate.empty();
  req->cells_to_be_deactiv_list.resize(cgis_to_deactivate.size());
  for (unsigned i = 0, e = cgis_to_deactivate.size(); i != e; ++i) {
    req->cells_to_be_deactiv_list[i].load_info_obj(ASN1_F1AP_ID_CELLS_TO_BE_DEACTIV_LIST_ITEM);
    req->cells_to_be_deactiv_list[i].value().cells_to_be_deactiv_list_item().nr_cgi =
        cgi_to_asn1(cgis_to_deactivate[i]);
  }

  return msg;
}

f1ap_message srsran::test_helpers::generate_gnb_cu_configuration_update_acknowledgement(
    const std::vector<f1ap_cell_failed_to_activate>& cells_failed_to_activate)
{
  f1ap_message gnb_cu_configuration_update_ack = {};

  gnb_cu_configuration_update_ack.pdu.set_successful_outcome();
  gnb_cu_configuration_update_ack.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_GNB_CU_CFG_UPD);

  auto& gnb_cu_cfg_upd_ack = gnb_cu_configuration_update_ack.pdu.successful_outcome().value.gnb_cu_cfg_upd_ack();

  for (const auto& cell : cells_failed_to_activate) {
    asn1::protocol_ie_single_container_s<asn1::f1ap::cells_failed_to_be_activ_list_item_ies_o> asn1_cell_container;
    cells_failed_to_be_activ_list_item_s& asn1_cell = asn1_cell_container->cells_failed_to_be_activ_list_item();

    asn1_cell.nr_cgi = cgi_to_asn1(cell.cgi);
    asn1_cell.cause  = cause_to_asn1(cell.cause);

    gnb_cu_cfg_upd_ack->cells_failed_to_be_activ_list.push_back(asn1_cell_container);
  }

  return gnb_cu_configuration_update_ack;
}

f1ap_message srsran::test_helpers::generate_gnb_cu_configuration_update_failure()
{
  f1ap_message gnb_cu_configuration_update_failure = {};

  gnb_cu_configuration_update_failure.pdu.set_unsuccessful_outcome();
  gnb_cu_configuration_update_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_GNB_CU_CFG_UPD);

  auto& gnb_cu_cfg_upd_fail =
      gnb_cu_configuration_update_failure.pdu.unsuccessful_outcome().value.gnb_cu_cfg_upd_fail();
  gnb_cu_cfg_upd_fail->cause.set_radio_network();
  gnb_cu_cfg_upd_fail->cause.radio_network() = cause_radio_network_opts::options::interaction_with_other_proc;

  return gnb_cu_configuration_update_failure;
}

f1ap_message srsran::test_helpers::generate_f1_removal_request(unsigned transaction_id)
{
  f1ap_message req;
  req.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_F1_REMOVAL);
  f1_removal_request_s& ie = req.pdu.init_msg().value.f1_removal_request();

  ie->resize(1);
  (*ie)[0].load_info_obj(ASN1_F1AP_ID_TRANSACTION_ID);
  (*ie)[0]->transaction_id() = transaction_id;

  return req;
}

f1ap_message srsran::test_helpers::generate_f1_removal_response(const f1ap_message& f1_removal_request)
{
  srsran_assert(f1_removal_request.pdu.type().value == f1ap_pdu_c::types_opts::init_msg, "Expected F1 removal request");
  srsran_assert(f1_removal_request.pdu.init_msg().value.type().value ==
                    f1ap_elem_procs_o::init_msg_c::types_opts::f1_removal_request,
                "Expected F1 removal request");

  f1ap_message resp;
  resp.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_REMOVAL);
  f1_removal_resp_s& ie = resp.pdu.successful_outcome().value.f1_removal_resp();
  ie->transaction_id    = (*f1_removal_request.pdu.init_msg().value.f1_removal_request())[0]->transaction_id();

  return resp;
}

static drbs_to_be_setup_item_s generate_drb_am_setup_item(drb_id_t drbid)
{
  using namespace asn1::f1ap;

  drbs_to_be_setup_item_s drb;
  drb.drb_id = drb_id_to_uint(drbid);
  drb.qos_info.set_choice_ext().load_info_obj(ASN1_F1AP_ID_DRB_INFO);
  auto& drb_info                                                 = drb.qos_info.choice_ext()->drb_info();
  drb_info.drb_qos.qos_characteristics.set_non_dyn_5qi().five_qi = 9;
  drb_info.drb_qos.ngra_nalloc_retention_prio.prio_level         = 1;
  drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_cap.value =
      pre_emption_cap_opts::shall_not_trigger_pre_emption;
  drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_vulnerability.value =
      pre_emption_vulnerability_opts::not_pre_emptable;
  drb_info.drb_qos.reflective_qos_attribute_present = true;
  drb_info.drb_qos.reflective_qos_attribute.value =
      qos_flow_level_qos_params_s::reflective_qos_attribute_opts::subject_to;
  drb_info.snssai.sst.from_string("01");
  drb_info.snssai.sd.from_string("0027db");
  drb.rlc_mode.value         = rlc_mode_opts::rlc_am;
  drb.ie_exts_present        = true;
  drb.ie_exts.dl_pdcp_sn_len = pdcp_sn_len_opts::twelve_bits;
  drb.ul_up_tnl_info_to_be_setup_list.resize(1);
  auto& gtp_tun = drb.ul_up_tnl_info_to_be_setup_list[0].ul_up_tnl_info.set_gtp_tunnel();
  auto  addr    = transport_layer_address::create_from_string("127.0.0.1");
  gtp_tun.transport_layer_address.from_string(addr.to_bitstring());
  gtp_tun.gtp_teid.from_number(1);

  return drb;
}

f1ap_message srsran::test_helpers::generate_ue_context_setup_request(gnb_cu_ue_f1ap_id_t                cu_ue_id,
                                                                     std::optional<gnb_du_ue_f1ap_id_t> du_ue_id,
                                                                     uint32_t                     rrc_container_pdcp_sn,
                                                                     const std::vector<drb_id_t>& drbs_to_setup,
                                                                     nr_cell_global_id_t          nr_cgi)
{
  using namespace asn1::f1ap;
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_request_s& dl_msg = msg.pdu.init_msg().value.ue_context_setup_request();

  dl_msg->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  if (du_ue_id.has_value()) {
    dl_msg->gnb_du_ue_f1ap_id_present = true;
    dl_msg->gnb_du_ue_f1ap_id         = (unsigned)*du_ue_id;
  }

  // spCell.
  dl_msg->sp_cell_id.plmn_id = nr_cgi.plmn_id.to_bytes();
  dl_msg->sp_cell_id.nr_cell_id.from_number(nr_cgi.nci.value());

  // SRB2.
  dl_msg->srbs_to_be_setup_list_present = true;
  dl_msg->srbs_to_be_setup_list.resize(1);
  dl_msg->srbs_to_be_setup_list[0].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_ITEM);
  srbs_to_be_setup_item_s& srb2 = dl_msg->srbs_to_be_setup_list[0]->srbs_to_be_setup_item();
  srb2.srb_id                   = 2;

  // drbs-to-be-setup.
  dl_msg->drbs_to_be_setup_list_present = drbs_to_setup.size() > 0;
  dl_msg->drbs_to_be_setup_list.resize(drbs_to_setup.size());
  unsigned count = 0;
  for (drb_id_t drbid : drbs_to_setup) {
    dl_msg->drbs_to_be_setup_list[count].load_info_obj(ASN1_F1AP_ID_DRB_INFO);
    dl_msg->drbs_to_be_setup_list[count]->drbs_to_be_setup_item() = generate_drb_am_setup_item(drbid);
    ++count;
  }

  dl_msg->rrc_container_present = true;
  bool success                  = dl_msg->rrc_container.append(
      generate_rrc_container(rrc_container_pdcp_sn, test_rgen::uniform_int<unsigned>(3, 100)));
  report_error_if_not(success, "Failed to allocate RRC container");

  return msg;
}

f1ap_message srsran::test_helpers::generate_ue_context_setup_response(gnb_cu_ue_f1ap_id_t   cu_ue_id,
                                                                      gnb_du_ue_f1ap_id_t   du_ue_id,
                                                                      std::optional<rnti_t> crnti,
                                                                      byte_buffer           cell_group_config)
{
  f1ap_message ue_context_setup_response = {};

  ue_context_setup_response.pdu.set_successful_outcome();
  ue_context_setup_response.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);

  auto& ue_context_setup_resp = ue_context_setup_response.pdu.successful_outcome().value.ue_context_setup_resp();
  ue_context_setup_resp->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  ue_context_setup_resp->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;

  if (crnti.has_value()) {
    ue_context_setup_resp->c_rnti_present = true;
    ue_context_setup_resp->c_rnti         = (unsigned)crnti.value();
  }

  ue_context_setup_resp->du_to_cu_rrc_info.cell_group_cfg = cell_group_config.copy();

  return ue_context_setup_response;
}

f1ap_message srsran::test_helpers::generate_ue_context_setup_failure(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                     gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message ue_context_setup_failure = {};

  ue_context_setup_failure.pdu.set_unsuccessful_outcome();
  ue_context_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);

  auto& ue_context_setup_fail = ue_context_setup_failure.pdu.unsuccessful_outcome().value.ue_context_setup_fail();
  ue_context_setup_fail->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  ue_context_setup_fail->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  ue_context_setup_fail->cause.set_radio_network();
  ue_context_setup_fail->cause.radio_network() =
      cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;

  return ue_context_setup_failure;
}

f1ap_message srsran::test_helpers::generate_ue_context_release_request(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                       gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message msg;
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE_REQUEST);

  auto& release_req              = msg.pdu.init_msg().value.ue_context_release_request();
  release_req->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  release_req->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  release_req->cause.set_radio_network();
  release_req->cause.radio_network().value = cause_radio_network_e::rl_fail_others;

  return msg;
}

f1ap_message srsran::test_helpers::generate_ue_context_release_command(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                       gnb_du_ue_f1ap_id_t du_ue_id,
                                                                       srb_id_t            srb_id,
                                                                       byte_buffer         rrc_container)
{
  f1ap_message msg;
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);

  auto& release_cmd              = msg.pdu.init_msg().value.ue_context_release_cmd();
  release_cmd->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  release_cmd->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  release_cmd->cause.set_radio_network();
  release_cmd->cause.radio_network().value = cause_radio_network_e::unspecified;
  if (not rrc_container.empty()) {
    release_cmd->srb_id_present        = true;
    release_cmd->srb_id                = srb_id_to_uint(srb_id);
    release_cmd->rrc_container_present = true;
    release_cmd->rrc_container         = std::move(rrc_container);
  }

  return msg;
}

f1ap_message srsran::test_helpers::generate_ue_context_release_complete(const f1ap_message& ue_ctxt_release_cmd)
{
  srsran_assert(ue_ctxt_release_cmd.pdu.type().value == f1ap_pdu_c::types_opts::init_msg, "Invalid argument message");
  srsran_assert(ue_ctxt_release_cmd.pdu.init_msg().value.type().value ==
                    f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd,
                "Invalid argument message");

  const ue_context_release_cmd_s& cmd = ue_ctxt_release_cmd.pdu.init_msg().value.ue_context_release_cmd();

  return generate_ue_context_release_complete(int_to_gnb_cu_ue_f1ap_id(cmd->gnb_cu_ue_f1ap_id),
                                              int_to_gnb_du_ue_f1ap_id(cmd->gnb_du_ue_f1ap_id));
}

f1ap_message srsran::test_helpers::generate_ue_context_release_complete(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                        gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message ue_ctxt_rel_complete_msg = {};
  ue_ctxt_rel_complete_msg.pdu.set_successful_outcome();
  ue_ctxt_rel_complete_msg.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
  ue_context_release_complete_s& rel_complete_msg =
      ue_ctxt_rel_complete_msg.pdu.successful_outcome().value.ue_context_release_complete();

  rel_complete_msg->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  rel_complete_msg->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;

  return ue_ctxt_rel_complete_msg;
}

static asn1::f1ap::ul_up_tnl_info_to_be_setup_list_l generate_ul_up_tnl_info_to_be_setup_list_l()
{
  asn1::f1ap::ul_up_tnl_info_to_be_setup_list_l list;

  list.resize(1);
  auto& gtp_tun = list[0].ul_up_tnl_info.set_gtp_tunnel();
  auto  addr    = transport_layer_address::create_from_string("127.0.0.1");
  gtp_tun.transport_layer_address.from_string(addr.to_bitstring());
  gtp_tun.gtp_teid.from_number(1);

  return list;
}

static asn1::f1ap::drbs_to_be_modified_item_s generate_to_modify_drb_am_mod_item(drb_id_t drbid)
{
  using namespace asn1::f1ap;

  drbs_to_be_modified_item_s drb;
  drb.drb_id                          = drb_id_to_uint(drbid);
  drb.ie_exts_present                 = true;
  drb.ie_exts.dl_pdcp_sn_len_present  = true;
  drb.ie_exts.dl_pdcp_sn_len          = pdcp_sn_len_opts::twelve_bits;
  drb.ul_up_tnl_info_to_be_setup_list = generate_ul_up_tnl_info_to_be_setup_list_l();
  return drb;
}

static asn1::f1ap::drbs_to_be_setup_mod_item_s generate_drb_am_mod_item(drb_id_t drbid)
{
  using namespace asn1::f1ap;
  drbs_to_be_setup_mod_item_s drb;
  drb.drb_id = drb_id_to_uint(drbid);
  drb.qos_info.set_choice_ext().load_info_obj(ASN1_F1AP_ID_DRB_INFO);
  auto& drb_info                                                 = drb.qos_info.choice_ext()->drb_info();
  drb_info.drb_qos.qos_characteristics.set_non_dyn_5qi().five_qi = 8;
  drb_info.drb_qos.ngra_nalloc_retention_prio.prio_level         = 1;
  drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_cap.value =
      pre_emption_cap_opts::shall_not_trigger_pre_emption;
  drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_vulnerability.value =
      pre_emption_vulnerability_opts::not_pre_emptable;
  drb_info.drb_qos.reflective_qos_attribute_present = true;
  drb_info.drb_qos.reflective_qos_attribute.value =
      qos_flow_level_qos_params_s::reflective_qos_attribute_opts::subject_to;
  drb_info.flows_mapped_to_drb_list.resize(1);
  drb_info.flows_mapped_to_drb_list[0].qos_flow_id = 0;
  auto& qos_flow                                   = drb_info.flows_mapped_to_drb_list[0].qos_flow_level_qos_params;
  auto& desc                                       = qos_flow.qos_characteristics.set_non_dyn_5qi();
  desc.five_qi                                     = 8;
  qos_flow.ngra_nalloc_retention_prio.pre_emption_cap.value = pre_emption_cap_opts::shall_not_trigger_pre_emption;
  qos_flow.ngra_nalloc_retention_prio.pre_emption_vulnerability.value =
      pre_emption_vulnerability_opts::not_pre_emptable;
  drb_info.snssai.sst.from_string("01");
  drb_info.snssai.sd.from_string("0027db");
  drb.rlc_mode.value                  = rlc_mode_opts::rlc_am;
  drb.ie_exts_present                 = true;
  drb.ie_exts.dl_pdcp_sn_len_present  = true;
  drb.ie_exts.dl_pdcp_sn_len          = pdcp_sn_len_opts::twelve_bits;
  drb.ul_up_tnl_info_to_be_setup_list = generate_ul_up_tnl_info_to_be_setup_list_l();
  return drb;
}

f1ap_message
srsran::test_helpers::generate_ue_context_modification_request(gnb_du_ue_f1ap_id_t                    du_ue_id,
                                                               gnb_cu_ue_f1ap_id_t                    cu_ue_id,
                                                               const std::initializer_list<drb_id_t>& drbs_to_setup,
                                                               const std::initializer_list<drb_id_t>& drbs_to_mod,
                                                               const std::initializer_list<drb_id_t>& drbs_to_rem,
                                                               byte_buffer                            rrc_container)
{
  using namespace asn1::f1ap;
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
  ue_context_mod_request_s& dl_msg = msg.pdu.init_msg().value.ue_context_mod_request();
  dl_msg->gnb_cu_ue_f1ap_id        = gnb_cu_ue_f1ap_id_to_uint(cu_ue_id);
  dl_msg->gnb_du_ue_f1ap_id        = gnb_du_ue_f1ap_id_to_uint(du_ue_id);

  dl_msg->drbs_to_be_setup_mod_list_present = drbs_to_setup.size() > 0;
  dl_msg->drbs_to_be_setup_mod_list.resize(drbs_to_setup.size());
  unsigned count = 0;
  for (drb_id_t drbid : drbs_to_setup) {
    dl_msg->drbs_to_be_setup_mod_list[count].load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_MOD_ITEM);
    dl_msg->drbs_to_be_setup_mod_list[count]->drbs_to_be_setup_mod_item() = generate_drb_am_mod_item(drbid);
    ++count;
  }

  dl_msg->drbs_to_be_released_list_present = drbs_to_rem.size() > 0;
  dl_msg->drbs_to_be_released_list.resize(drbs_to_rem.size());
  count = 0;
  for (drb_id_t drbid : drbs_to_rem) {
    dl_msg->drbs_to_be_released_list[count].load_info_obj(ASN1_F1AP_ID_DRBS_TO_BE_RELEASED_ITEM);
    dl_msg->drbs_to_be_released_list[count]->drbs_to_be_released_item().drb_id = drb_id_to_uint(drbid);
    ++count;
  }

  dl_msg->drbs_to_be_modified_list_present = drbs_to_mod.size() > 0;
  dl_msg->drbs_to_be_modified_list.resize(drbs_to_mod.size());
  count = 0;
  for (drb_id_t drbid : drbs_to_mod) {
    dl_msg->drbs_to_be_modified_list[count].load_info_obj(ASN1_F1AP_ID_DRBS_TO_BE_MODIFIED_ITEM);
    dl_msg->drbs_to_be_modified_list[count]->drbs_to_be_modified_item() = generate_to_modify_drb_am_mod_item(drbid);
    ++count;
  }

  dl_msg->rrc_container_present = not rrc_container.empty();
  dl_msg->rrc_container         = rrc_container.copy();

  return msg;
}

f1ap_message
srsran::test_helpers::generate_ue_context_modification_response(gnb_du_ue_f1ap_id_t          du_ue_id,
                                                                gnb_cu_ue_f1ap_id_t          cu_ue_id,
                                                                rnti_t                       crnti,
                                                                const std::vector<drb_id_t>& drbs_setup_mod_list,
                                                                const std::vector<drb_id_t>& drbs_modified_list)
{
  f1ap_message pdu = {};

  pdu.pdu.set_successful_outcome();
  pdu.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);

  auto& ue_context_mod_resp                        = pdu.pdu.successful_outcome().value.ue_context_mod_resp();
  ue_context_mod_resp->gnb_cu_ue_f1ap_id           = (unsigned)cu_ue_id;
  ue_context_mod_resp->gnb_du_ue_f1ap_id           = (unsigned)du_ue_id;
  ue_context_mod_resp->c_rnti_present              = true;
  ue_context_mod_resp->c_rnti                      = (unsigned)crnti;
  ue_context_mod_resp->drbs_setup_mod_list_present = true;

  ue_context_mod_resp->drbs_setup_mod_list_present = !drbs_setup_mod_list.empty();
  for (const auto& drb : drbs_setup_mod_list) {
    ue_context_mod_resp->drbs_setup_mod_list.push_back({});
    ue_context_mod_resp->drbs_setup_mod_list.back().load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_MOD_ITEM);
    ue_context_mod_resp->drbs_setup_mod_list.back().value().drbs_setup_mod_item().drb_id = drb_id_to_uint(drb);
  }

  ue_context_mod_resp->drbs_modified_list_present = !drbs_modified_list.empty();
  for (const auto& drb : drbs_modified_list) {
    ue_context_mod_resp->drbs_modified_list.push_back({});
    ue_context_mod_resp->drbs_modified_list.back().load_info_obj(ASN1_F1AP_ID_DRBS_MODIFIED_ITEM);
    ue_context_mod_resp->drbs_modified_list.back().value().drbs_modified_item().drb_id = drb_id_to_uint(drb);
  }
  return pdu;
}

f1ap_message srsran::test_helpers::generate_ue_context_modification_failure(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                            gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message ue_context_modification_failure = {};

  ue_context_modification_failure.pdu.set_unsuccessful_outcome();
  ue_context_modification_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);

  auto& ue_context_mod_fail = ue_context_modification_failure.pdu.unsuccessful_outcome().value.ue_context_mod_fail();
  ue_context_mod_fail->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  ue_context_mod_fail->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  ue_context_mod_fail->cause.set_radio_network();
  ue_context_mod_fail->cause.radio_network() =
      cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;

  return ue_context_modification_failure;
}

f1ap_message
srsran::test_helpers::generate_init_ul_rrc_message_transfer_without_du_to_cu_container(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                                       rnti_t              crnti)
{
  f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(du_ue_id, crnti);
  init_ul_rrc_msg.pdu.init_msg().value.init_ul_rrc_msg_transfer()->du_to_cu_rrc_container_present = false;

  return init_ul_rrc_msg;
}

f1ap_message srsran::test_helpers::generate_init_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                         rnti_t              crnti,
                                                                         byte_buffer         cell_group_cfg,
                                                                         byte_buffer         rrc_container)
{
  f1ap_message init_ul_rrc_msg;

  init_ul_rrc_msg.pdu.set_init_msg();
  init_ul_rrc_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER);

  init_ul_rrc_msg_transfer_s& init_ul_rrc = init_ul_rrc_msg.pdu.init_msg().value.init_ul_rrc_msg_transfer();
  init_ul_rrc->gnb_du_ue_f1ap_id          = (unsigned)du_ue_id;

  nr_cell_identity nci = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
  init_ul_rrc->nr_cgi.nr_cell_id.from_number(nci.value());
  init_ul_rrc->nr_cgi.plmn_id.from_string("00f110");
  init_ul_rrc->c_rnti = to_value(crnti);

  init_ul_rrc->sul_access_ind_present = true;
  init_ul_rrc->sul_access_ind.value   = sul_access_ind_opts::options::true_value;

  if (rrc_container.empty()) {
    init_ul_rrc->rrc_container.from_string("1dec89d05766");
  } else {
    init_ul_rrc->rrc_container = std::move(rrc_container);
  }

  init_ul_rrc->du_to_cu_rrc_container_present = true;
  if (cell_group_cfg.empty()) {
    init_ul_rrc->du_to_cu_rrc_container.from_string(
        "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
        "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
        "c400");
  } else {
    init_ul_rrc->du_to_cu_rrc_container = std::move(cell_group_cfg);
  }

  return init_ul_rrc_msg;
}

f1ap_message srsran::test_helpers::generate_dl_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                    gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                    srb_id_t            srb_id,
                                                                    byte_buffer         rrc_container)
{
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_DL_RRC_MSG_TRANSFER);
  auto& dlmsg = *msg.pdu.init_msg().value.dl_rrc_msg_transfer();

  dlmsg.gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  dlmsg.gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  dlmsg.srb_id            = (uint8_t)srb_id;
  dlmsg.rrc_container     = std::move(rrc_container);

  return msg;
}

f1ap_message srsran::test_helpers::generate_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                    gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                    srb_id_t            srb_id,
                                                                    byte_buffer         rrc_container)
{
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UL_RRC_MSG_TRANSFER);
  ul_rrc_msg_transfer_s& ulmsg = msg.pdu.init_msg().value.ul_rrc_msg_transfer();

  ulmsg->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  ulmsg->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  ulmsg->srb_id            = (uint8_t)srb_id;
  ulmsg->rrc_container     = std::move(rrc_container);

  return msg;
}

f1ap_message srsran::test_helpers::generate_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                    gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                    srb_id_t            srb_id,
                                                                    uint32_t            pdcp_sn,
                                                                    byte_buffer         ul_dcch_msg)
{
  // > Prepend PDCP header and append MAC.
  std::array<uint8_t, 2> pdcp_header{static_cast<uint8_t>((pdcp_sn >> 8U) & 0x0fU),
                                     static_cast<uint8_t>(pdcp_sn & 0xffU)};
  report_fatal_error_if_not(ul_dcch_msg.prepend(pdcp_header), "bad alloc");

  // > Append MAC
  report_fatal_error_if_not(ul_dcch_msg.append(std::array<uint8_t, 4>{}), "bad alloc");

  return generate_ul_rrc_message_transfer(du_ue_id, cu_ue_id, srb_id, std::move(ul_dcch_msg));
}

byte_buffer srsran::test_helpers::create_dl_dcch_rrc_container(uint32_t                              pdcp_sn,
                                                               const std::initializer_list<uint8_t>& dl_dcch_msg)
{
  return create_dl_dcch_rrc_container(pdcp_sn, byte_buffer::create(dl_dcch_msg).value());
}

byte_buffer srsran::test_helpers::create_dl_dcch_rrc_container(uint32_t pdcp_sn, const byte_buffer& dl_dcch_msg)
{
  byte_buffer container;

  // Add PDCP header.
  report_fatal_error_if_not(container.append(pdcp_sn >> 8U), "Failed to allocate");
  report_fatal_error_if_not(container.append(pdcp_sn & 0xfU), "Failed to allocate");

  // Add payload.
  report_fatal_error_if_not(container.append(dl_dcch_msg), "Failed to allocate");

  // Add dummy MAC.
  report_fatal_error_if_not(container.append({0x0, 0x0, 0x0, 0x0}), "Failed to allocate");

  return container;
}

byte_buffer srsran::test_helpers::extract_dl_dcch_msg(const byte_buffer& rrc_container)
{
  byte_buffer pdu = rrc_container.deep_copy().value();
  report_fatal_error_if_not(pdu.length() >= 7, "Invalid RRC container");

  // Remove PDCP header.
  pdu.trim_head(2);

  // Remove MAC.
  pdu.trim_tail(4);

  return pdu;
}

#ifndef SRSRAN_HAS_ENTERPRISE

f1ap_message srsran::test_helpers::generate_positioning_measurement_request(
    std::vector<trp_id_t>                                trp_ids,
    lmf_meas_id_t                                        lmf_meas_id,
    ran_meas_id_t                                        ran_meas_id,
    std::vector<asn1::f1ap::pos_meas_type_opts::options> pos_meas_type_list,
    subcarrier_spacing                                   scs,
    unsigned                                             srs_offset)
{
  return {};
}

f1ap_message srsran::test_helpers::generate_positioning_measurement_response(lmf_meas_id_t                lmf_meas_id,
                                                                             ran_meas_id_t                ran_meas_id,
                                                                             const std::vector<trp_id_t>& trp_ids,
                                                                             unsigned transaction_id)
{
  return {};
}

f1ap_message srsran::test_helpers::generate_positioning_measurement_failure(lmf_meas_id_t lmf_meas_id,
                                                                            ran_meas_id_t ran_meas_id)
{
  return {};
}

f1ap_message srsran::test_helpers::generate_trp_information_response(const trp_id_t& trp_id)
{
  return {};
}

f1ap_message srsran::test_helpers::generate_trp_information_failure()
{
  return {};
}

f1ap_message srsran::test_helpers::generate_positioning_information_response(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                             gnb_cu_ue_f1ap_id_t cu_ue_id)
{
  return {};
}

f1ap_message srsran::test_helpers::generate_positioning_information_failure(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                            gnb_cu_ue_f1ap_id_t cu_ue_id)
{
  return {};
}

f1ap_message srsran::test_helpers::generate_positioning_activation_response(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                            gnb_cu_ue_f1ap_id_t cu_ue_id)
{
  return {};
}

f1ap_message srsran::test_helpers::generate_positioning_activation_failure(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                           gnb_cu_ue_f1ap_id_t cu_ue_id)
{
  return {};
}

#endif // SRSRAN_HAS_ENTERPRISE
