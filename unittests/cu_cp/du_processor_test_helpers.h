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

#include "../../lib/cu_cp/ue_manager.h"
#include "../../lib/f1ap/common/asn1_helpers.h"
#include "../f1ap/common/test_helpers.h"
#include "../f1ap/cu_cp/f1_cu_test_helpers.h"
#include "../rrc/test_helpers.h"
#include "du_processor_test_helpers.h"
#include "test_helpers.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/cu_cp/du_processor_factory.h"
#include "srsgnb/rrc/rrc.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

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
  asn1::f1ap::nr_cgi_s asn1_cgi;
  asn1_cgi.nr_cell_id.from_number(nrcell_id);
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

/// Fixture class for DU processor creation
class du_processor_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::init();
    test_logger.set_level(srslog::basic_levels::debug);

    cu_cp_notifier       = std::make_unique<dummy_du_processor_cu_cp_notifier>(nullptr);
    f1c_pdu_notifier     = std::make_unique<dummy_f1c_pdu_notifier>();
    f1c_du_mgmt_notifier = std::make_unique<dummy_f1c_du_management_notifier>();
    e1ap_ctrl_notifier   = std::make_unique<dummy_du_processor_e1ap_control_notifier>();
    rrc_ue_ngc_notifier  = std::make_unique<dummy_rrc_ue_ngc_adapter>();
    ue_task_sched        = std::make_unique<dummy_du_processor_to_cu_cp_task_scheduler>(timers);

    // create and start DU processor
    du_processor_config_t du_cfg = {};

    du_processor_obj = create_du_processor(std::move(du_cfg),
                                           *cu_cp_notifier,
                                           *f1c_du_mgmt_notifier,
                                           *f1c_pdu_notifier,
                                           *e1ap_ctrl_notifier,
                                           *rrc_ue_ngc_notifier,
                                           *rrc_ue_ngc_notifier,
                                           *ue_task_sched,
                                           ue_mng);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  void attach_ue()
  {
    // Generate valid F1SetupRequest
    f1_setup_request_message f1_setup_request_msg = generate_valid_f1_setup_request_message();
    // Pass message to DU processor
    du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

    // Generate ue_creation message
    ue_creation_message ue_creation_msg = generate_valid_ue_creation_message(MIN_CRNTI, 12345678);
    // Pass message to DU processor
    du_processor_obj->handle_ue_creation_request(ue_creation_msg);
  }

  void receive_rrc_reconfig_complete()
  {
    // inject RRC Reconfiguration complete into UE object
  }

  std::unique_ptr<du_processor_interface>                     du_processor_obj;
  std::unique_ptr<dummy_du_processor_cu_cp_notifier>          cu_cp_notifier;
  std::unique_ptr<dummy_f1c_pdu_notifier>                     f1c_pdu_notifier;
  std::unique_ptr<dummy_f1c_du_management_notifier>           f1c_du_mgmt_notifier;
  std::unique_ptr<dummy_du_processor_e1ap_control_notifier>   e1ap_ctrl_notifier;
  std::unique_ptr<dummy_rrc_ue_ngc_adapter>                   rrc_ue_ngc_notifier;
  timer_manager                                               timers;
  std::unique_ptr<dummy_du_processor_to_cu_cp_task_scheduler> ue_task_sched;
  ue_manager                                                  ue_mng;
  srslog::basic_logger&                                       test_logger = srslog::fetch_basic_logger("TEST");
};

} // namespace srs_cu_cp
} // namespace srsgnb
