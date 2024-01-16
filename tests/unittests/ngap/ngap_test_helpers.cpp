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

#include "ngap_test_helpers.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <chrono>
#include <memory>

using namespace srsran;
using namespace srs_cu_cp;

ngap_test::ngap_test() : ngap_ue_task_scheduler(timers, ctrl_worker)
{
  test_logger.set_level(srslog::basic_levels::debug);
  ngap_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  cfg.gnb_id        = 411;
  cfg.ran_node_name = "srsgnb01";
  cfg.plmn          = "00101";
  cfg.tac           = 7;
  s_nssai_t slice_cfg;
  slice_cfg.sst = 1;
  cfg.slice_configurations.push_back(slice_cfg);
  cfg.ue_context_setup_timeout_s = std::chrono::seconds(2);

  ngap = create_ngap(cfg, cu_cp_paging_notifier, ngap_ue_task_scheduler, ue_mng, msg_notifier, ctrl_worker);

  du_processor_notifier =
      std::make_unique<dummy_ngap_du_processor_notifier>(ngap->get_ngap_ue_context_removal_handler());
}

ngap_test::~ngap_test()
{
  // flush logger after each test
  srslog::flush();
}

ue_index_t ngap_test::create_ue(rnti_t rnti)
{
  // Create UE in UE manager
  ue_index_t ue_index = ue_mng.allocate_new_ue_index(uint_to_du_index(0));
  auto*      ue       = ue_mng.add_ue(ue_index, MIN_PCI, rnti);
  if (ue == nullptr) {
    test_logger.error("Failed to create UE with pci={} and rnti={}", MIN_PCI, rnti_t::MIN_CRNTI);
    return ue_index_t::invalid;
  }

  // Inject UE creation at NGAP
  test_ues.emplace(ue_index, test_ue(ue_index));
  test_ue& new_test_ue = test_ues.at(ue_index);
  ngap->create_ngap_ue(ue_index, new_test_ue.rrc_ue_notifier, new_test_ue.rrc_ue_notifier, *du_processor_notifier);

  // generate and inject valid initial ue message
  cu_cp_initial_ue_message msg = generate_initial_ue_message(ue_index);
  ngap->handle_initial_ue_message(msg);

  new_test_ue.ran_ue_id =
      uint_to_ran_ue_id(msg_notifier.last_ngap_msgs.back().pdu.init_msg().value.init_ue_msg()->ran_ue_ngap_id);

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

void ngap_test::run_inital_context_setup(ue_index_t ue_index)
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
      ue.amf_ue_id.value(), ue.ran_ue_id.value(), pdu_session_id);
  ngap->handle_message(pdu_session_resource_setup_request);
}

void ngap_test::add_pdu_session_to_up_manager(ue_index_t       ue_index,
                                              pdu_session_id_t pdu_session_id,
                                              drb_id_t         drb_id,
                                              qos_flow_id_t    qos_flow_id)
{
  auto&                                        up_mng = ue_mng.find_ngap_ue(ue_index)->get_up_resource_manager();
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
