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

#pragma once

#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/f1ap/common/test_helpers.h"
#include "tests/unittests/f1ap/cu_cp/f1ap_cu_test_helpers.h"
#include "tests/unittests/rrc/test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/du_processor_config.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// Adapter to receive request, generate response and feed back.
class f1ap_mobility_test_adapter : public f1ap_message_handler
{
public:
  f1ap_mobility_test_adapter() = default;
  void handle_message(const f1ap_message& msg) override
  {
    if (handler) {
      if (msg.pdu.type().value == asn1::f1ap::f1ap_pdu_c::types::init_msg &&
          msg.pdu.init_msg().value.type().value ==
              asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_setup_request) {
        gnb_cu_ue_f1ap_id_t cu_ue_id = int_to_gnb_cu_ue_f1ap_id(0);
        gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(1);
        f1ap_message        response_msg;

        if (generate_context_setup_success) {
          rnti_t crnti = to_rnti(0x4601);
          response_msg = generate_ue_context_setup_response(cu_ue_id, du_ue_id, crnti);
        } else {
          response_msg = generate_ue_context_setup_failure(cu_ue_id, du_ue_id);
        }
        // Feed back response.
        handler->handle_message(response_msg);
      } else if (msg.pdu.type().value == asn1::f1ap::f1ap_pdu_c::types::init_msg &&
                 msg.pdu.init_msg().value.type().value ==
                     asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd) {
        gnb_cu_ue_f1ap_id_t cu_ue_id     = int_to_gnb_cu_ue_f1ap_id(0);
        gnb_du_ue_f1ap_id_t du_ue_id     = int_to_gnb_du_ue_f1ap_id(1);
        f1ap_message        response_msg = generate_ue_context_release_complete(cu_ue_id, du_ue_id);
        handler->handle_message(response_msg);
      }
    }
  }

  void set_context_setup_outcome(bool success) { generate_context_setup_success = success; }

  void set_handler(f1ap_message_handler* handler_) { handler = handler_; }

private:
  f1ap_message_handler* handler                        = nullptr;
  bool                  generate_context_setup_success = false;
};

struct du_wrapper {
  du_wrapper(ue_manager& ue_mng) : cu_cp_notifier(ngap_ue_removal_handler, &ue_mng), f1ap_pdu_notifier(nullptr){};

  dummy_cell_meas_manager                               cell_meas_mng;
  dummy_ngap_ue_context_removal_handler                 ngap_ue_removal_handler;
  dummy_du_processor_cu_cp_notifier                     cu_cp_notifier;
  dummy_f1ap_pdu_notifier                               f1ap_pdu_notifier;
  dummy_f1ap_du_management_notifier                     f1ap_du_mgmt_notifier;
  dummy_du_processor_e1ap_control_notifier              e1ap_ctrl_notifier;
  dummy_du_processor_ngap_control_notifier              ngap_ctrl_notifier;
  dummy_f1ap_ue_removal_notifier                        f1ap_cu_cp_notifier;
  dummy_rrc_ue_ngap_adapter                             rrc_ue_ngap_notifier;
  dummy_rrc_ue_cu_cp_adapter                            rrc_ue_cu_cp_notifier;
  std::unique_ptr<dummy_du_processor_ue_task_scheduler> ue_task_sched;
  std::unique_ptr<du_processor_interface>               du_processor_obj;

  f1ap_mobility_test_adapter test_adapter;
};

/// Fixture class for CU-CP mobility tests
class mobility_test : public ::testing::Test
{
protected:
  mobility_test();
  ~mobility_test() override;

  du_wrapper* create_du(const du_processor_config_t& du_cfg);
  void        attach_du_to_cu(du_wrapper& du, unsigned gnb_du_id, unsigned nrcell_id, pci_t pci);
  void        attach_ue(du_wrapper& du, ue_index_t ue_index, unsigned nrcell_id);

  ue_manager* get_ue_manager() { return &ue_mng; }

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  manual_task_worker      ctrl_worker{128};
  timer_manager           timers;
  ue_configuration        ue_config;
  up_resource_manager_cfg up_config;
  ue_manager              ue_mng{ue_config, up_config};

  std::unordered_map<du_index_t, du_wrapper> du_db;
};

} // namespace srs_cu_cp
} // namespace srsran
