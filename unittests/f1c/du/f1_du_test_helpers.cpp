/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1_du_test_helpers.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace srs_du;

f1_setup_request_message srsgnb::srs_du::generate_f1_setup_request_message()
{
  f1_setup_request_message           request_msg  = {};
  du_setup_params                    setup_params = {};
  du_cell_config                     cell         = config_helpers::make_default_du_cell_config();
  std::vector<const du_cell_config*> cells        = {&cell};
  fill_asn1_f1_setup_request(request_msg.msg, setup_params, cells);

  return request_msg;
}

f1c_message srsgnb::srs_du::generate_f1_setup_response_message(unsigned transaction_id)
{
  f1c_message f1_setup_response = {};

  f1_setup_response.pdu.set_successful_outcome();
  f1_setup_response.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_res                 = f1_setup_response.pdu.successful_outcome().value.f1_setup_resp();
  setup_res->transaction_id.value = transaction_id;
  setup_res->gnb_cu_name_present  = true;
  setup_res->gnb_cu_name.value.from_string("srsCU");
  setup_res->gnb_cu_rrc_version.value.latest_rrc_version.from_number(2);

  return f1_setup_response;
}

f1c_message srsgnb::srs_du::generate_f1_setup_failure_message(unsigned transaction_id)
{
  f1c_message f1_setup_failure = {};

  f1_setup_failure.pdu.set_unsuccessful_outcome();
  f1_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_fail                 = f1_setup_failure.pdu.unsuccessful_outcome().value.f1_setup_fail();
  setup_fail->transaction_id.value = transaction_id;
  setup_fail->cause.value.set_radio_network();
  setup_fail->cause.value.radio_network() =
      asn1::f1ap::cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;
  setup_fail->time_to_wait_present     = false;
  setup_fail->crit_diagnostics_present = false;
  // add critical diagnostics

  return f1_setup_failure;
}

f1c_message srsgnb::srs_du::generate_f1_setup_failure_message_with_time_to_wait(unsigned transaction_id,
                                                                                asn1::f1ap::time_to_wait_e time_to_wait)
{
  f1c_message f1_setup_failure = generate_f1_setup_failure_message(transaction_id);

  auto& setup_fail                 = f1_setup_failure.pdu.unsuccessful_outcome().value.f1_setup_fail();
  setup_fail->time_to_wait_present = true;
  setup_fail->time_to_wait.value   = time_to_wait;

  return f1_setup_failure;
}

f1c_message srsgnb::srs_du::generate_f1_dl_rrc_message_transfer(srb_id_t srb_id, const byte_buffer& rrc_container)
{
  f1c_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_DLRRC_MSG_TRANSFER);
  auto& dl_msg                      = msg.pdu.init_msg().value.dlrrc_msg_transfer();
  dl_msg->gnb_cu_ue_f1_ap_id->value = 0;
  dl_msg->gnb_du_ue_f1_ap_id->value = 0;
  dl_msg->srbid->value              = srb_id_to_uint(srb_id);
  dl_msg->rrc_container->resize(rrc_container.length());
  std::copy(rrc_container.begin(), rrc_container.end(), dl_msg->rrc_container->begin());

  return msg;
}

asn1::f1ap::drbs_to_be_setup_item_s srsgnb::srs_du::generate_drb_am_setup_item(drb_id_t drbid)
{
  using namespace asn1::f1ap;

  drbs_to_be_setup_item_s drb;
  drb.drbid = drb_id_to_uint(drbid);
  drb.qo_sinfo.set_choice_ext().load_info_obj(ASN1_F1AP_ID_DRB_INFO);
  auto& drb_info                                                            = drb.qo_sinfo.choice_ext()->drb_info();
  drb_info.drb_qos.qo_s_characteristics.set_non_dynamic_minus5_qi().five_qi = 8;
  drb_info.drb_qos.ngra_nalloc_retention_prio.prio_level                    = 1;
  drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_cap.value =
      pre_emption_cap_opts::shall_not_trigger_pre_emption;
  drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_vulnerability.value =
      pre_emption_vulnerability_opts::not_pre_emptable;
  drb_info.drb_qos.reflective_qos_attribute_present = true;
  drb_info.drb_qos.reflective_qos_attribute.value =
      qo_sflow_level_qos_params_s::reflective_qos_attribute_opts::subject_to;
  drb_info.snssai.sst.from_string("01");
  drb_info.snssai.sd.from_string("0027db");
  drb.rlc_mode.value = rlc_mode_opts::rlc_am;

  return drb;
}

f1c_message srsgnb::srs_du::generate_f1_ue_context_setup_request(const std::initializer_list<drb_id_t> drbs_to_add)
{
  using namespace asn1::f1ap;
  f1c_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_request_s& dl_msg    = msg.pdu.init_msg().value.ue_context_setup_request();
  dl_msg->gnb_cu_ue_f1_ap_id->value     = 0;
  dl_msg->gnb_du_ue_f1_ap_id->value     = 0;
  dl_msg->srbs_to_be_setup_list_present = true;
  dl_msg->srbs_to_be_setup_list.value.resize(1);
  dl_msg->srbs_to_be_setup_list.value[0].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_ITEM);
  srbs_to_be_setup_item_s& srb2 = dl_msg->srbs_to_be_setup_list.value[0]->srbs_to_be_setup_item();
  srb2.srbid                    = 2;

  dl_msg->drbs_to_be_setup_list_present = drbs_to_add.size() > 0;
  dl_msg->drbs_to_be_setup_list.value.resize(drbs_to_add.size());
  unsigned count = 0;
  for (drb_id_t drbid : drbs_to_add) {
    dl_msg->drbs_to_be_setup_list.value[count].load_info_obj(ASN1_F1AP_ID_DRB_INFO);
    dl_msg->drbs_to_be_setup_list.value[count]->drbs_to_be_setup_item() = generate_drb_am_setup_item(drbid);
    ++count;
  }

  dl_msg->rrc_container_present = true;
  dl_msg->rrc_container.value.append(test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(1, 100)));

  return msg;
}

asn1::f1ap::drbs_to_be_setup_mod_item_s srsgnb::srs_du::generate_drb_am_mod_item(drb_id_t drbid)
{
  using namespace asn1::f1ap;
  drbs_to_be_setup_mod_item_s drb;
  drb.drbid = drb_id_to_uint(drbid);
  drb.qo_sinfo.set_choice_ext().load_info_obj(ASN1_F1AP_ID_DRB_INFO);
  auto& drb_info                                                            = drb.qo_sinfo.choice_ext()->drb_info();
  drb_info.drb_qos.qo_s_characteristics.set_non_dynamic_minus5_qi().five_qi = 8;
  drb_info.drb_qos.ngra_nalloc_retention_prio.prio_level                    = 1;
  drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_cap.value =
      pre_emption_cap_opts::shall_not_trigger_pre_emption;
  drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_vulnerability.value =
      pre_emption_vulnerability_opts::not_pre_emptable;
  drb_info.drb_qos.reflective_qos_attribute_present = true;
  drb_info.drb_qos.reflective_qos_attribute.value =
      qo_sflow_level_qos_params_s::reflective_qos_attribute_opts::subject_to;
  drb_info.snssai.sst.from_string("01");
  drb_info.snssai.sd.from_string("0027db");
  drb.rlc_mode.value = rlc_mode_opts::rlc_am;
  return drb;
}

f1c_message
srsgnb::srs_du::generate_f1_ue_context_modification_request(const std::initializer_list<drb_id_t> drbs_to_add)
{
  using namespace asn1::f1ap;
  f1c_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
  ue_context_mod_request_s& dl_msg  = msg.pdu.init_msg().value.ue_context_mod_request();
  dl_msg->gnb_cu_ue_f1_ap_id->value = 0;
  dl_msg->gnb_du_ue_f1_ap_id->value = 0;

  dl_msg->drbs_to_be_setup_mod_list_present = drbs_to_add.size() > 0;
  dl_msg->drbs_to_be_setup_mod_list.value.resize(drbs_to_add.size());
  unsigned count = 0;
  for (drb_id_t drbid : drbs_to_add) {
    dl_msg->drbs_to_be_setup_mod_list.value[count].load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_MOD_ITEM);
    dl_msg->drbs_to_be_setup_mod_list.value[count]->drbs_to_be_setup_mod_item() = generate_drb_am_mod_item(drbid);
    ++count;
  }

  dl_msg->rrc_container_present = true;
  dl_msg->rrc_container.value.append(test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(1, 100)));
  return msg;
}

//////////////////////////////////

f1ap_du_test::f1ap_du_test()
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
  srslog::init();

  f1ap = create_f1ap(msg_notifier, f1c_du_cfg_handler, ctrl_worker, ue_exec_mapper);
  f1c_du_cfg_handler.connect(*f1ap);
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
  async_task<f1_setup_response_message>         t = f1ap->handle_f1ap_setup_request(request_msg);
  lazy_task_launcher<f1_setup_response_message> t_launcher(t);

  // Action 2: F1 setup response received.
  unsigned    transaction_id    = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();
  f1c_message f1_setup_response = generate_f1_setup_response_message(transaction_id);
  test_logger.info("Injecting F1SetupResponse");
  f1ap->handle_message(f1_setup_response);
}

f1ap_du_test::ue_test_context* f1ap_du_test::run_f1_ue_create(du_ue_index_t ue_index)
{
  unsigned srb1_idx = srb_id_to_uint(srb_id_t::srb1);
  test_ues.emplace(ue_index);
  test_ues[ue_index].crnti = to_rnti(0x4601 + ue_index);
  test_ues[ue_index].f1c_bearers.emplace(srb_id_to_uint(srb_id_t::srb1));
  test_ues[ue_index].f1c_bearers[srb1_idx].srb_id = srb_id_t::srb1;

  f1ap_ue_creation_request msg;
  msg.ue_index    = ue_index;
  msg.pcell_index = to_du_cell_index(0);
  msg.c_rnti      = test_ues[ue_index].crnti;
  msg.du_cu_rrc_container.append({0x1, 0x2, 0x3});
  for (auto& f1c_bearer : test_ues[ue_index].f1c_bearers) {
    f1c_bearer_to_addmod b;
    b.srb_id       = f1c_bearer.srb_id;
    b.tx_pdu_notif = &f1c_bearer.tx_pdu_notifier;
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

void f1ap_du_test::run_ue_context_setup_procedure(du_ue_index_t                   ue_index,
                                                  std::initializer_list<drb_id_t> drbs_to_addmod)
{
  ue_test_context& ue = test_ues[ue_index];

  f1c_message msg = generate_f1_ue_context_setup_request(drbs_to_addmod);

  // Generate dummy DU manager UE Config Update command to F1AP.
  const auto& f1c_req                         = msg.pdu.init_msg().value.ue_context_setup_request();
  f1c_du_cfg_handler.next_ue_cfg_req.ue_index = ue_index;
  for (const auto& srb : f1c_req->srbs_to_be_setup_list.value) {
    srb_id_t srb_id = (srb_id_t)srb.value().srbs_to_be_setup_item().srbid;

    // Create F1-C bearer in test ue.
    ue.f1c_bearers.emplace(srb_id_to_uint(srb_id));
    ue.f1c_bearers[srb_id_to_uint(srb_id)].srb_id = srb_id;

    f1c_bearer_to_addmod bearer;
    bearer.srb_id       = srb_id;
    bearer.tx_pdu_notif = &ue.f1c_bearers[srb_id_to_uint(srb_id)].tx_pdu_notifier;
    f1c_du_cfg_handler.next_ue_cfg_req.f1c_bearers_to_add.push_back(bearer);
  }
  for (const auto& drb : f1c_req->drbs_to_be_setup_list.value) {
    drb_id_t drb_id = (drb_id_t)drb.value().drbs_to_be_setup_item().drbid;
    ue.f1u_bearers.emplace(drb_id_to_uint(drb_id) - 1);
    auto& f1u_bearer  = ue.f1u_bearers[drb_id_to_uint(drb_id) - 1];
    f1u_bearer.drb_id = drb_id;

    f1u_bearer_to_addmod bearer;
    bearer.drb_id          = drb_id;
    bearer.rx_sdu_notifier = &f1u_bearer.rx_sdu_notifier;
    f1c_du_cfg_handler.next_ue_cfg_req.f1u_bearers_to_add.push_back(bearer);
  }

  // Generate DU manager response to UE context update.
  f1c_du_cfg_handler.next_ue_context_update_response.result                 = true;
  f1c_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container = {0x1, 0x2, 0x3};

  // Send UE CONTEXT SETUP REQUEST message to F1AP.
  f1ap->handle_message(msg);

  // Register created F1-C and F1-U bearers in dummy DU manager.
  for (const auto& created_srb : f1c_du_cfg_handler.last_ue_cfg_response->f1c_bearers_added) {
    ue.f1c_bearers[srb_id_to_uint(created_srb.srb_id)].bearer = created_srb.bearer;
  }
  for (const auto& created_drb : f1c_du_cfg_handler.last_ue_cfg_response->f1u_bearers_added) {
    ue.f1u_bearers[drb_id_to_uint(created_drb.drb_id) - 1].bearer = created_drb.bearer;
  }
}

f1ap_ue_configuration_response f1ap_du_test::update_f1_ue_config(du_ue_index_t                   ue_index,
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
    b.srb_id       = f1c_bearer.srb_id;
    b.tx_pdu_notif = &f1c_bearer.tx_pdu_notifier;
    req.f1c_bearers_to_add.push_back(b);
  }
  for (drb_id_t drb_id : drbs) {
    auto&                f1u_bearer = test_ues[ue_index].f1u_bearers[drb_id_to_uint(drb_id) - 1];
    f1u_bearer_to_addmod b;
    b.drb_id          = f1u_bearer.drb_id;
    b.rx_sdu_notifier = &f1u_bearer.rx_sdu_notifier;
    req.f1u_bearers_to_add.push_back(b);
  }

  test_logger.info("Configuring UE={}", ue_index);
  return f1ap->handle_ue_configuration_request(req);
}
