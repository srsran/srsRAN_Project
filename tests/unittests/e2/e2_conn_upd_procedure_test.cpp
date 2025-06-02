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

#include "common/e2ap_asn1_packer.h"
#include "lib/e2/e2sm/e2sm_kpm/e2sm_kpm_cu_meas_provider_impl.h"
#include "lib/e2/e2sm/e2sm_kpm/e2sm_kpm_du_meas_provider_impl.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/srsran_test.h"
#include <gtest/gtest.h>

using namespace srsran;

// Helper global variables to pass pcap_writer to all tests.
bool      g_enable_pcap = false;
dlt_pcap* g_pcap        = nullptr;

class e2_agent_test_with_pcap : public e2_test_base_with_pcap
{
protected:
  dlt_pcap* external_pcap_writer;

  void SetUp() override
  {
    external_pcap_writer = GetParam();

    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    cfg                  = config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    gw                       = std::make_unique<dummy_sctp_association_sdu_notifier>();
    pcap                     = std::make_unique<dummy_e2ap_pcap>();
    e2_client                = std::make_unique<dummy_e2_connection_client>();
    du_metrics               = std::make_unique<dummy_e2_du_metrics>();
    f1ap_ue_id_mapper        = std::make_unique<dummy_f1ap_ue_id_translator>();
    factory                  = timer_factory{timers, task_worker};
    du_rc_param_configurator = std::make_unique<dummy_du_configurator>();
    e2agent                  = create_e2_du_agent(cfg,
                                 *e2_client,
                                 du_metrics.get(),
                                 f1ap_ue_id_mapper.get(),
                                 du_rc_param_configurator.get(),
                                 factory,
                                 task_worker);
    if (external_pcap_writer) {
      packer = std::make_unique<e2ap_asn1_packer>(*gw, e2agent->get_e2_interface(), *external_pcap_writer);
    } else {
      packer = std::make_unique<e2ap_asn1_packer>(*gw, e2agent->get_e2_interface(), *pcap);
    }
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
    pcap->close();
  }
};

TEST_P(e2_agent_test_with_pcap, e2_connection_update_failure)
{
  // We need this test to generate E2 Setup Request, so Wireshark can decode the following RIC indication messages.
  test_logger.info("Launch e2 setup request procedure with task worker...");
  e2agent->start();

  if (g_enable_pcap) {
    // Save E2 Setup Request
    packer->handle_message(e2_client->last_tx_e2_pdu);
  }

  // Need to send setup response, so the transaction can be completed.
  unsigned   transaction_id    = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message e2_setup_response = generate_e2_setup_response(transaction_id);
  e2_setup_response.pdu.successful_outcome()
      .value.e2setup_resp()
      ->ran_functions_accepted[0]
      ->ran_function_id_item()
      .ran_function_id = e2sm_kpm_asn1_packer::ran_func_id;
  test_logger.info("Injecting E2SetupResponse");

  if (g_enable_pcap) {
    // Save E2 Setup Request (send to pcap)
    packer->handle_message(e2_setup_response);
  }
  e2agent->get_e2_interface().handle_message(e2_setup_response);

  // Inject packed E2 Connection Update.
  uint8_t     e2_conn_upd[]   = {0x00, 0x0b, 0x00, 0x1b, 0x00, 0x00, 0x02, 0x00, 0x31, 0x00, 0x02,
                                 0x00, 0x03, 0x00, 0x2c, 0x00, 0x0e, 0x00, 0x00, 0x2b, 0x40, 0x09,
                                 0x21, 0xf0, 0x0a, 0x2a, 0x00, 0x61, 0x8e, 0x45, 0x40};
  byte_buffer e2_conn_upd_buf = byte_buffer::create(e2_conn_upd, e2_conn_upd + sizeof(e2_conn_upd)).value();

  asn1::cbit_ref bref(e2_conn_upd_buf);
  e2_message     e2_conn_upd_msg = {};
  if (e2_conn_upd_msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    return;
  }
  if (g_enable_pcap) {
    // Save E2 Setup Request (send to pcap)
    packer->handle_message(e2_conn_upd_msg);
  }
  e2agent->get_e2_interface().handle_message(e2_conn_upd_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  if (g_enable_pcap) {
    // Save E2 Setup Request (send to pcap)
    packer->handle_message(msg);
  }

  // Currently, E2 Connection Update is not implemented, so always return failure.
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::e2conn_upd_fail);
  e2agent->stop();
}

INSTANTIATE_TEST_SUITE_P(e2_agent_test_with_pcap, e2_agent_test_with_pcap, testing::Values(g_pcap));

int main(int argc, char** argv)
{
  // Check for '--enable_pcap' cmd line argument, do not use getopt as it interferes with gtest.
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--enable_pcap") {
      g_enable_pcap = true;
    }
  }

  srslog::init();

  std::unique_ptr<task_worker_executor> pcap_exec;
  std::unique_ptr<task_worker>          pcap_worker;
  std::unique_ptr<dlt_pcap>             common_pcap_writer;

  if (g_enable_pcap) {
    pcap_worker        = std::make_unique<task_worker>("pcap_worker", 128);
    pcap_exec          = std::make_unique<task_worker_executor>(*pcap_worker);
    common_pcap_writer = create_e2ap_pcap("/tmp/e2_conn_upd_test.pcap", *pcap_exec);
    g_pcap             = common_pcap_writer.get();
  }

  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return ret;
}
