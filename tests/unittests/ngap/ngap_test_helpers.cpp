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

#include "ngap_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"

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

  ngap = create_ngap(cfg, cu_cp_paging_notifier, ngap_ue_task_scheduler, ue_mng, msg_notifier, ctrl_worker);
}

ngap_test::~ngap_test()
{
  // flush logger after each test
  srslog::flush();
}

void ngap_test::create_ue(ue_index_t ue_index)
{
  // Inject UE creation at NGAP
  ngap->create_ngap_ue(ue_index, rrc_ue_notifier, rrc_ue_notifier, du_processor_notifier);

  // generate and inject valid initial ue message
  ngap_initial_ue_message msg = generate_initial_ue_message(ue_index);
  ngap->handle_initial_ue_message(msg);

  test_ues.emplace(ue_index, ue_index);
  test_ues.at(ue_index).ran_ue_id =
      uint_to_ran_ue_id(msg_notifier.last_ngap_msg.pdu.init_msg().value.init_ue_msg()->ran_ue_ngap_id);
}

void ngap_test::run_dl_nas_transport(ue_index_t ue_index)
{
  auto& ue     = test_ues.at(ue_index);
  ue.amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));

  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);
}

void ngap_test::run_ul_nas_transport(ue_index_t ue_index)
{
  ngap_ul_nas_transport_message ul_nas_transport = generate_ul_nas_transport_message(ue_index);
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

void ngap_test::tick()
{
  timers.tick();
  ctrl_worker.run_pending_tasks();
}
