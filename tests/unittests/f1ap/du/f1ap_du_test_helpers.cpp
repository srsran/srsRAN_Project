/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "f1ap_du_test_helpers.h"
#include "../common/f1ap_cu_test_messages.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace srs_du;

gnb_du_ue_f1ap_id_t srsran::srs_du::generate_random_gnb_du_ue_f1ap_id()
{
  return int_to_gnb_du_ue_f1ap_id(test_rgen::uniform_int<uint64_t>(
      gnb_du_ue_f1ap_id_to_uint(gnb_du_ue_f1ap_id_t::min), gnb_du_ue_f1ap_id_to_uint(gnb_du_ue_f1ap_id_t::max) - 1));
}

f1_setup_request_message srsran::srs_du::generate_f1_setup_request_message()
{
  f1_setup_request_message      request_msg = {};
  du_manager_params::ran_params ran_params;
  ran_params.gnb_du_name  = "srsgnb";
  ran_params.gnb_du_id    = 1;
  ran_params.rrc_version  = 1;
  ran_params.du_bind_addr = {"127.0.0.1"};
  du_cell_config cell     = config_helpers::make_default_du_cell_config();
  ran_params.cells        = {cell};
  fill_f1_setup_request(request_msg, ran_params);

  return request_msg;
}

asn1::f1ap::drbs_to_be_setup_item_s srsran::srs_du::generate_drb_am_setup_item(drb_id_t drbid)
{
  using namespace asn1::f1ap;

  drbs_to_be_setup_item_s drb;
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
  drb_info.snssai.sst.from_string("01");
  drb_info.snssai.sd.from_string("0027db");
  drb.rlc_mode.value = rlc_mode_opts::rlc_am;
  drb.ul_up_tnl_info_to_be_setup_list.resize(1);
  auto&                   gtp_tun = drb.ul_up_tnl_info_to_be_setup_list[0].ul_up_tnl_info.set_gtp_tunnel();
  transport_layer_address addr{"127.0.0.1"};
  gtp_tun.transport_layer_address.from_string(addr.to_bitstring());
  gtp_tun.gtp_teid.from_number(1);

  return drb;
}

f1ap_message srsran::srs_du::generate_ue_context_setup_request(const std::initializer_list<drb_id_t>& drbs_to_add)
{
  using namespace asn1::f1ap;
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_request_s& dl_msg    = msg.pdu.init_msg().value.ue_context_setup_request();
  dl_msg->gnb_cu_ue_f1ap_id             = 0;
  dl_msg->gnb_du_ue_f1ap_id_present     = true;
  dl_msg->gnb_du_ue_f1ap_id             = 0;
  dl_msg->srbs_to_be_setup_list_present = true;
  dl_msg->srbs_to_be_setup_list.resize(1);
  dl_msg->srbs_to_be_setup_list[0].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_ITEM);
  srbs_to_be_setup_item_s& srb2 = dl_msg->srbs_to_be_setup_list[0]->srbs_to_be_setup_item();
  srb2.srb_id                   = 2;

  dl_msg->drbs_to_be_setup_list_present = drbs_to_add.size() > 0;
  dl_msg->drbs_to_be_setup_list.resize(drbs_to_add.size());
  unsigned count = 0;
  for (drb_id_t drbid : drbs_to_add) {
    dl_msg->drbs_to_be_setup_list[count].load_info_obj(ASN1_F1AP_ID_DRB_INFO);
    dl_msg->drbs_to_be_setup_list[count]->drbs_to_be_setup_item() = generate_drb_am_setup_item(drbid);
    ++count;
  }

  dl_msg->rrc_container_present = true;
  dl_msg->rrc_container.append(test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(3, 100)));

  return msg;
}

asn1::f1ap::drbs_to_be_setup_mod_item_s srsran::srs_du::generate_drb_am_mod_item(drb_id_t drbid)
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
  drb_info.snssai.sst.from_string("01");
  drb_info.snssai.sd.from_string("0027db");
  drb.rlc_mode.value = rlc_mode_opts::rlc_am;
  drb.ul_up_tnl_info_to_be_setup_list.resize(1);
  auto&                   gtp_tun = drb.ul_up_tnl_info_to_be_setup_list[0].ul_up_tnl_info.set_gtp_tunnel();
  transport_layer_address addr{"127.0.0.1"};
  gtp_tun.transport_layer_address.from_string(addr.to_bitstring());
  gtp_tun.gtp_teid.from_number(1);
  return drb;
}

f1ap_message
srsran::srs_du::generate_ue_context_modification_request(const std::initializer_list<drb_id_t>& drbs_to_add)
{
  using namespace asn1::f1ap;
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
  ue_context_mod_request_s& dl_msg = msg.pdu.init_msg().value.ue_context_mod_request();
  dl_msg->gnb_cu_ue_f1ap_id        = 0;
  dl_msg->gnb_du_ue_f1ap_id        = 0;

  dl_msg->drbs_to_be_setup_mod_list_present = drbs_to_add.size() > 0;
  dl_msg->drbs_to_be_setup_mod_list.resize(drbs_to_add.size());
  unsigned count = 0;
  for (drb_id_t drbid : drbs_to_add) {
    dl_msg->drbs_to_be_setup_mod_list[count].load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_MOD_ITEM);
    dl_msg->drbs_to_be_setup_mod_list[count]->drbs_to_be_setup_mod_item() = generate_drb_am_mod_item(drbid);
    ++count;
  }

  dl_msg->rrc_container_present = true;
  dl_msg->rrc_container.append(test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(1, 100)));
  return msg;
}

f1ap_message srsran::srs_du::generate_ue_context_release_command()
{
  using namespace asn1::f1ap;
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
  ue_context_release_cmd_s& dl_msg = msg.pdu.init_msg().value.ue_context_release_cmd();
  dl_msg->gnb_cu_ue_f1ap_id        = 0;
  dl_msg->gnb_du_ue_f1ap_id        = 0;
  dl_msg->cause.set(cause_c::types_opts::misc);
  dl_msg->cause.misc().value    = asn1::f1ap::cause_misc_opts::unspecified;
  dl_msg->srb_id_present        = true;
  dl_msg->srb_id                = 1;
  dl_msg->rrc_container_present = true;
  dl_msg->rrc_container         = byte_buffer{0x1, 0x2, 0x3, 0x4};

  return msg;
}

namespace {

class dummy_f1ap_tx_pdu_notifier : public f1ap_message_notifier
{
public:
  dummy_f1ap_tx_pdu_notifier(f1ap_message& last_tx_pdu_) : last_tx_pdu(last_tx_pdu_) {}

  void on_new_message(const f1ap_message& msg) override { last_tx_pdu = msg; }

  f1ap_message& last_tx_pdu;
};

} // namespace

std::unique_ptr<f1ap_message_notifier>
dummy_f1c_connection_client::handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier_)
{
  du_rx_pdu_notifier = std::move(du_rx_pdu_notifier_);
  return std::make_unique<dummy_f1ap_tx_pdu_notifier>(last_tx_f1ap_pdu);
}

//////////////////////////////////

f1ap_du_test::f1ap_du_test()
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
  srslog::init();

  f1ap = create_f1ap(f1c_gw, f1ap_du_cfg_handler, ctrl_worker, ue_exec_mapper, paging_handler);
  f1ap_du_cfg_handler.connect(*f1ap);
}

f1ap_du_test::~f1ap_du_test()
{
  // flush logger after each test
  srslog::flush();
}

void f1ap_du_test::run_f1_setup_procedure()
{
  // Action 1: Launch F1 setup procedure
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Launch f1 setup request procedure...");
  async_task<f1_setup_response_message>         t = f1ap->handle_f1_setup_request(request_msg);
  lazy_task_launcher<f1_setup_response_message> t_launcher(t);

  // Action 2: F1 setup response received.
  unsigned     transaction_id    = get_transaction_id(f1c_gw.last_tx_f1ap_pdu.pdu).value();
  f1ap_message f1_setup_response = generate_f1_setup_response_message(transaction_id);
  test_logger.info("Injecting F1SetupResponse");
  f1ap->handle_message(f1_setup_response);
}

f1ap_du_test::ue_test_context* f1ap_du_test::run_f1ap_ue_create(du_ue_index_t ue_index)
{
  unsigned srb1_idx = srb_id_to_uint(srb_id_t::srb1);
  test_ues.emplace(ue_index);
  test_ues[ue_index].crnti    = to_rnti(0x4601 + ue_index);
  test_ues[ue_index].ue_index = ue_index;
  test_ues[ue_index].f1c_bearers.emplace(srb_id_to_uint(srb_id_t::srb1));
  test_ues[ue_index].f1c_bearers[srb1_idx].srb_id = srb_id_t::srb1;

  f1ap_ue_creation_request msg;
  msg.ue_index    = ue_index;
  msg.pcell_index = to_du_cell_index(0);
  msg.c_rnti      = test_ues[ue_index].crnti;
  msg.du_cu_rrc_container.append({0x1, 0x2, 0x3});
  for (auto& f1c_bearer : test_ues[ue_index].f1c_bearers) {
    f1c_bearer_to_addmod b;
    b.srb_id          = f1c_bearer.srb_id;
    b.rx_sdu_notifier = &f1c_bearer.rx_sdu_notifier;
    msg.f1c_bearers_to_add.push_back(b);
  }
  test_logger.info("Creating ueId={}", msg.ue_index);
  f1ap_ue_creation_response resp = f1ap->handle_ue_creation_request(msg);
  if (resp.result) {
    unsigned count = 0;
    for (auto& f1c_bearer : test_ues[ue_index].f1c_bearers) {
      f1c_bearer.bearer = resp.f1c_bearers_added[count++];
    }
    return &test_ues[ue_index];
  }
  test_ues.erase(ue_index);
  return nullptr;
}

void f1ap_du_test::run_ue_context_setup_procedure(du_ue_index_t ue_index, const f1ap_message& msg)
{
  ue_test_context& ue = test_ues[ue_index];

  // Update F1AP UE IDs if not set yet.
  const auto& f1ap_req = msg.pdu.init_msg().value.ue_context_setup_request();
  if (not ue.gnb_du_ue_f1ap_id.has_value()) {
    ue.gnb_du_ue_f1ap_id = (gnb_du_ue_f1ap_id_t)f1ap_req->gnb_du_ue_f1ap_id;
  }
  if (not ue.gnb_cu_ue_f1ap_id.has_value()) {
    ue.gnb_cu_ue_f1ap_id = (gnb_cu_ue_f1ap_id_t)f1ap_req->gnb_cu_ue_f1ap_id;
  }

  // Generate dummy DU manager UE Config Update command to F1AP.
  f1ap_du_cfg_handler.next_ue_cfg_req.ue_index = ue_index;
  for (const auto& srb : f1ap_req->srbs_to_be_setup_list) {
    srb_id_t srb_id = (srb_id_t)srb.value().srbs_to_be_setup_item().srb_id;

    // Create F1-C bearer in test ue.
    ue.f1c_bearers.emplace(srb_id_to_uint(srb_id));
    ue.f1c_bearers[srb_id_to_uint(srb_id)].srb_id = srb_id;

    f1c_bearer_to_addmod bearer;
    bearer.srb_id          = srb_id;
    bearer.rx_sdu_notifier = &ue.f1c_bearers[srb_id_to_uint(srb_id)].rx_sdu_notifier;
    f1ap_du_cfg_handler.next_ue_cfg_req.f1c_bearers_to_add.push_back(bearer);
  }
  for (const auto& drb : f1ap_req->drbs_to_be_setup_list) {
    drb_id_t drb_id = (drb_id_t)drb.value().drbs_to_be_setup_item().drb_id;
    ue.f1u_bearers.emplace(drb_id_to_uint(drb_id) - 1);
    auto& f1u_bearer  = ue.f1u_bearers[drb_id_to_uint(drb_id) - 1];
    f1u_bearer.drb_id = drb_id;
  }

  // Generate DU manager response to UE context update.
  f1ap_du_cfg_handler.next_ue_context_update_response.result                 = true;
  f1ap_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container = {0x1, 0x2, 0x3};

  // Send UE CONTEXT SETUP REQUEST message to F1AP.
  f1ap->handle_message(msg);

  // Register created F1-C and F1-U bearers in dummy DU manager.
  for (const auto& created_srb : f1ap_du_cfg_handler.last_ue_cfg_response->f1c_bearers_added) {
    ue.f1c_bearers[srb_id_to_uint(created_srb.srb_id)].bearer = created_srb.bearer;
  }
}

f1ap_ue_configuration_response f1ap_du_test::update_f1ap_ue_config(du_ue_index_t                   ue_index,
                                                                   std::initializer_list<srb_id_t> srbs,
                                                                   std::initializer_list<drb_id_t> drbs)
{
  for (srb_id_t srb_id : srbs) {
    test_ues[ue_index].f1c_bearers.emplace(srb_id_to_uint(srb_id));
    test_ues[ue_index].f1c_bearers[srb_id_to_uint(srb_id)].srb_id = srb_id;
  }
  for (drb_id_t drb_id : drbs) {
    test_ues[ue_index].f1u_bearers.emplace(drb_id_to_uint(drb_id) - 1);
    test_ues[ue_index].f1u_bearers[drb_id_to_uint(drb_id) - 1].drb_id = drb_id;
  }

  f1ap_ue_configuration_request req;
  req.ue_index = ue_index;
  for (srb_id_t srb_id : srbs) {
    auto&                f1c_bearer = test_ues[ue_index].f1c_bearers[srb_id_to_uint(srb_id)];
    f1c_bearer_to_addmod b;
    b.srb_id          = f1c_bearer.srb_id;
    b.rx_sdu_notifier = &f1c_bearer.rx_sdu_notifier;
    req.f1c_bearers_to_add.push_back(b);
  }

  test_logger.info("Configuring ue={}", ue_index);
  return f1ap->handle_ue_configuration_request(req);
}

void f1ap_du_test::tick()
{
  timer_service.tick();
  ctrl_worker.run_pending_tasks();
}
