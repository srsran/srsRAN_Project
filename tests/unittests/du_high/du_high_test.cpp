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

#include "lib/du_high/du_high.h"
#include "lib/du_high/du_high_executor_strategies.h"
#include "lib/f1ap/common/f1ap_asn1_packer.h"
#include "lib/gateways/sctp_network_gateway_impl.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/test_utils.h"
#include "test_utils/du_high_worker_manager.h"
#include "test_utils/dummy_test_components.h"
#include "tests/unittests/f1ap/common/test_helpers.h"
#include "tests/unittests/gateways/test_helpers.h"

using namespace srsran;
using namespace srs_du;

/// Test F1 setup over "local" connection to DU.
void test_f1_setup_local()
{
  test_delimit_logger delim{"Test F1 Setup Local"};

  du_high_worker_manager     workers;
  dummy_f1ap_message_handler msg_handler;
  dummy_f1ap_pdu_notifier    notifier(&msg_handler);

  phy_test_dummy phy;
  mac_pcap_dummy pcap;
  timer_manager  timers;

  du_high_configuration cfg{};
  cfg.du_mng_executor = &workers.ctrl_worker;
  cfg.cell_executors  = &workers.cell_exec_mapper;
  cfg.ue_executors    = &workers.ue_exec_mapper;
  cfg.f1ap_notifier   = &notifier;
  cfg.phy_adapter     = &phy;
  cfg.timers          = &timers;
  cfg.cells           = {config_helpers::make_default_du_cell_config()};
  cfg.sched_cfg       = config_helpers::make_default_scheduler_expert_config();
  cfg.pcap            = &pcap;
  du_high du_obj(cfg);

  du_obj.start();

  workers.stop();
}

/// Test F1 setup over network connection.
/// TODO: This is a component (or even integration) test by definition!!
void test_f1_setup_network()
{
  test_delimit_logger delim{"Test F1 Setup Network"};

  //
  class f1ap_network_adapter : public f1ap_message_handler
  {
  public:
    f1ap_network_adapter() : gw(gw_config, gw_notifier, gw_notifier), packer(gw, *this) {}

    /// We require a network gateway and a packer
    sctp_network_gateway_config    gw_config;
    dummy_network_gateway_notifier gw_notifier;
    sctp_network_gateway_impl      gw;
    f1ap_asn1_packer               packer;

    f1ap_message last_f1ap_msg;
    void         handle_message(const f1ap_message& msg) override { last_f1ap_msg = msg; }
  };

  du_high_worker_manager  workers;
  f1ap_network_adapter    pdu_handler;
  dummy_f1ap_pdu_notifier notifier(&pdu_handler);
  phy_test_dummy          phy;
  mac_pcap_dummy          pcap;
  timer_manager           timers;

  du_high_configuration cfg{};
  cfg.du_mng_executor = &workers.ctrl_worker;
  cfg.cell_executors  = &workers.cell_exec_mapper;
  cfg.ue_executors    = &workers.ue_exec_mapper;
  cfg.f1ap_notifier   = &notifier;
  cfg.phy_adapter     = &phy;
  cfg.timers          = &timers;
  cfg.cells           = {config_helpers::make_default_du_cell_config()};
  cfg.sched_cfg       = config_helpers::make_default_scheduler_expert_config();
  cfg.pcap            = &pcap;

  du_high du_obj(cfg);

  du_obj.start();

  workers.stop();
}

void test_du_ue_create()
{
  test_delimit_logger delim{"Test DU UE Create"};

  class dummy_f1ap_pdu_handler : public f1ap_message_handler
  {
  public:
    f1ap_message   last_f1ap_msg;
    task_executor* ctrl_exec;
    void           handle_message(const f1ap_message& msg) override
    {
      ctrl_exec->execute([this, msg]() { last_f1ap_msg = msg; });
    }
  };

  // Setup Task Workers.
  du_high_worker_manager workers;

  // Setup F1AP PDU handler.
  dummy_f1ap_pdu_handler pdu_handler;
  pdu_handler.ctrl_exec = &workers.ctrl_worker;
  phy_test_dummy phy;

  dummy_f1ap_pdu_notifier notifier(&pdu_handler);
  mac_pcap_dummy          pcap;
  timer_manager           timers;

  du_high_configuration cfg{};
  cfg.du_mng_executor = &workers.ctrl_worker;
  cfg.cell_executors  = &workers.cell_exec_mapper;
  cfg.ue_executors    = &workers.ue_exec_mapper;
  cfg.f1ap_notifier   = &notifier;
  cfg.phy_adapter     = &phy;
  cfg.timers          = &timers;
  cfg.cells           = {config_helpers::make_default_du_cell_config()};
  cfg.sched_cfg       = config_helpers::make_default_scheduler_expert_config();
  cfg.pcap            = &pcap;
  du_high du_obj(cfg);

  du_obj.start();
  TESTASSERT(pdu_handler.last_f1ap_msg.pdu.type() != asn1::f1ap::f1ap_pdu_c::types_opts::nulltype);
  pdu_handler.last_f1ap_msg.pdu = {};

  // Push F1AP setup response to DU, signaling that the CU accepted the F1 connection.
  f1ap_message f1ap_msg;
  f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  f1ap_msg.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list_present = true;
  f1ap_msg.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list.value.resize(1);
  du_obj.get_f1ap_message_handler().handle_message(f1ap_msg);

  // Add UE
  mac_rx_pdu_list lst;
  lst.push_back(mac_rx_pdu{to_rnti(0x4601), 0, 0, {0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00}});
  du_obj.get_pdu_handler(to_du_cell_index(0))
      .handle_rx_data_indication(mac_rx_data_indication{slot_point{0, 0}, to_du_cell_index(0), std::move(lst)});

  slot_point sl_tx{0, 0};
  for (uint32_t count = 0; count < 10000; count++) {
    du_obj.get_slot_handler(to_du_cell_index(0)).handle_slot_indication(sl_tx++);
    if (pdu_handler.last_f1ap_msg.pdu.type() != asn1::f1ap::f1ap_pdu_c::types_opts::nulltype) {
      break;
    }
    workers.ctrl_worker.run_next_blocking();
  }
  TESTASSERT(pdu_handler.last_f1ap_msg.pdu.type() == asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  TESTASSERT(pdu_handler.last_f1ap_msg.pdu.init_msg().proc_code == ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER);

  workers.stop();
}

int main()
{
  srslog::init();

  srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("F1AP").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::info);

  test_f1_setup_local();
  test_f1_setup_network();
  test_du_ue_create();
}
