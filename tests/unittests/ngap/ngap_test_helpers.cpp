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

#include "ngap_test_helpers.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/plmn_identity.h"
#include "srsran/ran/rb_id.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <chrono>

using namespace srsran;
using namespace srs_cu_cp;

ngap_test::ngap_test() :
  cu_cp_cfg([this]() {
    cu_cp_configuration cucfg     = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers         = &timers;
    cucfg.services.cu_cp_executor = &ctrl_worker;
    cucfg.ngap.ngaps.push_back(cu_cp_configuration::ngap_config{
        &n2_gw,
        {supported_tracking_area{
            7,
            {plmn_item{plmn_identity::test_value(),
                       std::vector<s_nssai_t>{s_nssai_t{slice_service_type{1}, slice_differentiator{}}}}}}}});
    return cucfg;
  }())
{
  test_logger.set_level(srslog::basic_levels::debug);
  ngap_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  ngap_configuration ngap_cfg{};
  ngap_cfg.gnb_id                      = cu_cp_cfg.node.gnb_id;
  ngap_cfg.ran_node_name               = cu_cp_cfg.node.ran_node_name;
  ngap_cfg.supported_tas               = cu_cp_cfg.ngap.ngaps.front().supported_tas;
  ngap_cfg.request_pdu_session_timeout = cu_cp_cfg.ue.request_pdu_session_timeout;
  ngap = create_ngap(ngap_cfg, cu_cp_notifier, *cu_cp_cfg.ngap.ngaps.front().n2_gw, timers, ctrl_worker);

  cu_cp_notifier.connect_ngap(ngap->get_ngap_ue_context_removal_handler());
  n2_gw.attach_handler(&dummy_amf);

  // Initiate N2 TNL association to AMF.
  report_fatal_error_if_not(ngap->handle_amf_tnl_connection_request(), "Unable to establish connection to AMF");
}

ngap_test::~ngap_test()
{
  // flush logger after each test
  srslog::flush();
}

bool ngap_test::run_ng_setup()
{
  // Launch NG setup procedure
  test_logger.info("Launch ng setup request procedure...");
  async_task<ngap_ng_setup_result>         t = ngap->handle_ng_setup_request(1);
  lazy_task_launcher<ngap_ng_setup_result> t_launcher(t);

  // Inject NG setup response message.
  ngap_message ng_setup_response = generate_ng_setup_response();
  test_logger.info("Injecting NGSetupResponse");
  ngap->handle_message(ng_setup_response);

  if (!std::holds_alternative<ngap_ng_setup_response>(t.get())) {
    test_logger.error("NG Setup procedure failed");
    return false;
  }

  return true;
}

ue_index_t ngap_test::create_ue(rnti_t rnti)
{
  // Create UE in UE manager
  ue_index_t ue_index = ue_mng.add_ue(du_index_t::min, int_to_gnb_du_id(0), MIN_PCI, rnti, du_cell_index_t::min);
  if (ue_index == ue_index_t::invalid) {
    test_logger.error(
        "Failed to create UE with pci={} rnti={} pcell_index={}", MIN_PCI, rnti_t::MIN_CRNTI, du_cell_index_t::min);
    return ue_index_t::invalid;
  }
  if (!ue_mng.set_plmn(ue_index, plmn_identity::test_value())) {
    test_logger.error("ue={}: Failed to set PLMN", ue_index);
    ue_mng.remove_ue(ue_index);
    return ue_index_t::invalid;
  }

  // Inject UE creation at NGAP
  test_ues.emplace(ue_index, test_ue(ue_index));
  test_ue& new_test_ue = test_ues.at(ue_index);

  ue_mng.get_ngap_rrc_ue_adapter(ue_index).connect_rrc_ue(new_test_ue.rrc_ue_handler);

  // generate and inject valid initial ue message
  cu_cp_initial_ue_message msg = generate_initial_ue_message(ue_index);
  ngap->handle_initial_ue_message(msg);

  new_test_ue.ran_ue_id =
      uint_to_ran_ue_id(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.init_ue_msg()->ran_ue_ngap_id);

  return ue_index;
}

ue_index_t ngap_test::create_ue_without_init_ue_message(rnti_t rnti)
{
  // Create UE in UE manager
  ue_index_t ue_index = ue_mng.add_ue(du_index_t::min, int_to_gnb_du_id(0), MIN_PCI, rnti, du_cell_index_t::min);
  if (ue_index == ue_index_t::invalid) {
    test_logger.error(
        "Failed to create UE with pci={} rnti={} pcell_index={}", MIN_PCI, rnti_t::MIN_CRNTI, du_cell_index_t::min);
    return ue_index_t::invalid;
  }
  if (!ue_mng.set_plmn(ue_index, plmn_identity::test_value())) {
    test_logger.error("ue={}: Failed to set PLMN", ue_index);
    ue_mng.remove_ue(ue_index);
    return ue_index_t::invalid;
  }

  // Inject UE creation at NGAP
  test_ues.emplace(ue_index, test_ue(ue_index));
  test_ue& new_test_ue = test_ues.at(ue_index);

  ue_mng.get_ngap_rrc_ue_adapter(ue_index).connect_rrc_ue(new_test_ue.rrc_ue_handler);

  return ue_index;
}

void ngap_test::run_dl_nas_transport(ue_index_t ue_index)
{
  auto& ue     = test_ues.at(ue_index);
  ue.amf_ue_id = uint_to_amf_ue_id(test_rgen::uniform_int<uint64_t>(16, 128));
  ue.amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));

  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);
}

void ngap_test::run_ul_nas_transport(ue_index_t ue_index)
{
  cu_cp_ul_nas_transport ul_nas_transport = generate_ul_nas_transport_message(ue_index);
  ngap->handle_ul_nas_transport_message(ul_nas_transport);
}

void ngap_test::run_initial_context_setup(ue_index_t ue_index)
{
  auto& ue = test_ues.at(ue_index);

  ngap_message init_context_setup_request =
      generate_valid_initial_context_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);
}

void ngap_test::run_pdu_session_resource_setup(ue_index_t ue_index, pdu_session_id_t pdu_session_id)
{
  auto& ue = test_ues.at(ue_index);

  ngap_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message(
      ue.amf_ue_id.value(),
      ue.ran_ue_id.value(),
      {{pdu_session_id, {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 9}}}}});
  ngap->handle_message(pdu_session_resource_setup_request);
}

void ngap_test::add_pdu_session_to_up_manager(ue_index_t       ue_index,
                                              pdu_session_id_t pdu_session_id,
                                              drb_id_t         drb_id,
                                              qos_flow_id_t    qos_flow_id)
{
  auto&                                        up_mng = ue_mng.find_ue(ue_index)->get_up_resource_manager();
  up_config_update_result                      result;
  up_pdu_session_context_update                ctxt_update{pdu_session_id};
  std::map<qos_flow_id_t, up_qos_flow_context> qos_flows;
  qos_flows[qos_flow_id]         = {qos_flow_id, {}};
  ctxt_update.drb_to_add[drb_id] = {drb_id, pdu_session_id, {}, false, {}, {}, qos_flows, {}, {}, {}};

  result.pdu_sessions_added_list.push_back(ctxt_update);

  up_mng.apply_config_update(result);
}

void ngap_test::tick()
{
  timers.tick();
  ctrl_worker.run_pending_tasks();
}
