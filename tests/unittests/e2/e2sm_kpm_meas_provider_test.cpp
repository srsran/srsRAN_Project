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

#include "lib/e2/common/e2ap_asn1_packer.h"
#include "lib/e2/common/e2ap_asn1_utils.h"
#include "lib/e2/e2sm/e2sm_kpm/e2sm_kpm_du_meas_provider_impl.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

#define PCAP_OUTPUT 0

#if PCAP_OUTPUT
std::unique_ptr<dlt_pcap> g_pcap = std::make_unique<dlt_pcap_impl>(PCAP_E2AP_DLT, "E2AP");

inline void save_msg_pcap(const byte_buffer& last_pdu)
{
  if (not g_pcap->is_write_enabled()) {
    g_pcap->open("e2sm_kpm_meas_provider.pcap");
  }
  g_pcap->push_pdu(last_pdu.copy());
  usleep(200);
}
#endif

class e2_rlc_metrics_notifier : public e2_du_metrics_notifier, public e2_du_metrics_interface
{
public:
  void get_metrics(scheduler_ue_metrics& ue_metrics) override {}

  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override
  {
    if (e2_meas_provider) {
      e2_meas_provider->report_metrics(ue_metrics);
    }
  }

  void report_metrics(const rlc_metrics& metrics) override
  {
    if (e2_meas_provider) {
      e2_meas_provider->report_metrics(metrics);
    }
  }

  void connect_e2_du_meas_provider(std::unique_ptr<e2_du_metrics_notifier> meas_provider) override {}

  void connect_e2_du_meas_provider(e2_du_metrics_notifier* meas_provider) { e2_meas_provider = meas_provider; }

private:
  e2_du_metrics_notifier* e2_meas_provider;
};

class e2sm_kpm_meas_provider_test : public ::testing::Test
{
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    cfg                  = srsran::config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    du_metrics        = std::make_unique<e2_rlc_metrics_notifier>();
    f1ap_ue_id_mapper = std::make_unique<dummy_f1ap_ue_id_translator>();
    du_meas_provider  = std::make_unique<e2sm_kpm_du_meas_provider_impl>(*f1ap_ue_id_mapper);
    e2sm_packer       = std::make_unique<e2sm_kpm_asn1_packer>(*du_meas_provider);
    e2sm_iface        = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_packer, *du_meas_provider);
    gw                = std::make_unique<dummy_network_gateway_data_handler>();
    pcap              = std::make_unique<dummy_e2ap_pcap>();
    packer            = std::make_unique<srsran::e2ap_asn1_packer>(*gw, *e2, *pcap);

    du_metrics->connect_e2_du_meas_provider(du_meas_provider.get());
  }

  void TearDown() override
  {
    // Flush logger after each test.
    srslog::flush();
    pcap->close();
  }

protected:
  void tick()
  {
    timers.tick();
    task_worker.run_pending_tasks();
  }
  e2ap_configuration                                  cfg = {};
  timer_factory                                       factory;
  timer_manager                                       timers;
  std::unique_ptr<dummy_network_gateway_data_handler> gw;
  std::unique_ptr<e2_interface>                       e2;
  std::unique_ptr<dummy_e2ap_pcap>                    pcap;
  std::unique_ptr<srsran::e2ap_asn1_packer>           packer;
  std::unique_ptr<e2sm_interface>                     e2sm_iface;
  std::unique_ptr<e2sm_handler>                       e2sm_packer;
  std::unique_ptr<e2_subscription_manager>            e2_subscription_mngr;
  std::unique_ptr<e2_rlc_metrics_notifier>            du_metrics;
  std::unique_ptr<dummy_f1ap_ue_id_translator>        f1ap_ue_id_mapper;
  std::unique_ptr<e2sm_kpm_du_meas_provider_impl>     du_meas_provider;
  manual_task_worker                                  task_worker{64};
  std::unique_ptr<dummy_e2_pdu_notifier>              msg_notifier;
  std::unique_ptr<dummy_e2_connection_client>         e2_client;
  srslog::basic_logger&                               test_logger = srslog::fetch_basic_logger("TEST");
};

rlc_metrics generate_rlc_metrics(uint32_t ue_idx, uint32_t bearer_id)
{
  rlc_metrics rlc_metric;
  rlc_metric.ue_index              = static_cast<du_ue_index_t>(ue_idx);
  rlc_metric.rb_id                 = rb_id_t(drb_id_t(bearer_id));
  rlc_metric.rx.mode               = rlc_mode::am;
  rlc_metric.rx.num_pdus           = 5;
  rlc_metric.rx.num_pdu_bytes      = rlc_metric.rx.num_pdus * 1000;
  rlc_metric.rx.num_sdus           = 5;
  rlc_metric.rx.num_sdu_bytes      = rlc_metric.rx.num_sdus * 1000;
  rlc_metric.rx.num_lost_pdus      = 1;
  rlc_metric.rx.num_malformed_pdus = 0;

  rlc_metric.tx.num_sdus             = 10;
  rlc_metric.tx.num_sdu_bytes        = rlc_metric.tx.num_sdus * 1000;
  rlc_metric.tx.num_dropped_sdus     = 1;
  rlc_metric.tx.num_discarded_sdus   = 0;
  rlc_metric.tx.num_discard_failures = 0;
  rlc_metric.tx.num_pdus             = 10;
  rlc_metric.tx.num_pdu_bytes        = rlc_metric.tx.num_pdus * 1000;

  return rlc_metric;
}

#if PCAP_OUTPUT
TEST_F(e2_entity_test, e2sm_kpm_generates_ran_func_desc)
{
  dummy_e2_pdu_notifier* dummy_msg_notifier = e2_client->get_e2_msg_notifier();
  // We need this test to generate E2 Setup Request, so Wireshark can decode the following RIC indication messages.
  test_logger.info("Launch e2 setup request procedure with task worker...");
  e2->start();

  save_msg_pcap(gw->last_pdu);

  // Need to send setup response, so the transaction can be completed.
  unsigned   transaction_id    = get_transaction_id(dummy_msg_notifier->last_e2_msg.pdu).value();
  e2_message e2_setup_response = generate_e2_setup_response(transaction_id);
  e2_setup_response.pdu.successful_outcome()
      .value.e2setup_resp()
      ->ra_nfunctions_accepted.value[0]
      ->ra_nfunction_id_item()
      .ran_function_id = 147;
  test_logger.info("Injecting E2SetupResponse");
  e2->handle_message(e2_setup_response);
}
#endif

TEST_F(e2sm_kpm_meas_provider_test, e2sm_kpm_ind_three_drb_rlc_metrics)
{
  std::vector<uint32_t> ue_ids        = {31, 23, 152};
  std::vector<uint32_t> nof_drbs      = {3, 1, 2};
  uint32_t              nof_ues       = ue_ids.size();
  uint32_t              nof_meas_data = 5;
  uint32_t              nof_records   = 1;

  uint32_t              expected_drop_rate       = 10;
  uint32_t              expected_ul_success_rate = 80;
  uint32_t              expected_ul_throughput   = 5000;
  std::vector<uint32_t> expected_dl_vol;
  std::vector<uint32_t> expected_ul_vol;

  for (auto& d : nof_drbs) {
    // Unit is kbit.
    expected_dl_vol.push_back(10 * 1000 * d * 8 / 1000);
    expected_ul_vol.push_back(5 * 1000 * d * 8 / 1000);
  }

  // Define E2SM_KPM action format 5.
  e2_sm_kpm_action_definition_s action_def;
  action_def.ric_style_type = 5;
  e2_sm_kpm_action_definition_format5_s& action_def_f5 =
      action_def.action_definition_formats.set_action_definition_format5();

  action_def_f5.matching_ueid_list.resize(nof_ues);
  for (unsigned i = 0; i < nof_ues; i++) {
    action_def_f5.matching_ueid_list[i].ue_id.set_gnb_du_ueid() = generate_ueid_gnb_du(ue_ids[i]);
  }

  e2_sm_kpm_action_definition_format1_s& subscript_info = action_def_f5.subscription_info;
  subscript_info.cell_global_id_present                 = false;
  subscript_info.granul_period                          = 100;

  meas_info_item_s meas_info_item;
  meas_info_item.meas_type.set_meas_name().from_string("DRB.RlcPacketDropRateDl");
  label_info_item_s label_info_item;
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_opts::true_value;
  meas_info_item.label_info_list.push_back(label_info_item);
  subscript_info.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("DRB.RlcSduTransmittedVolumeDL");
  subscript_info.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("DRB.RlcSduTransmittedVolumeUL");
  subscript_info.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("DRB.PacketSuccessRateUlgNBUu");
  subscript_info.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("DRB.UEThpUl");
  subscript_info.meas_info_list.push_back(meas_info_item);

  nof_records = subscript_info.meas_info_list.size();

  asn1::e2ap::ri_caction_to_be_setup_item_s ric_action = generate_e2sm_kpm_ric_action(action_def);

#if PCAP_OUTPUT
  // Save E2 Subscription Request.
  e2_message e2_subscript_req = generate_e2sm_kpm_subscription_request(ric_action);
  packer->handle_message(e2_subscript_req);
  save_msg_pcap(gw->last_pdu);
#endif

  ASSERT_TRUE(e2sm_iface->action_supported(ric_action));
  auto report_service = e2sm_iface->get_e2sm_report_service(ric_action.ric_action_definition);

  for (unsigned i = 0; i < nof_meas_data; ++i) {
    // Push dummy metric measurements.
    rlc_metrics rlc_metrics;
    for (unsigned u = 0; u < ue_ids.size(); ++u) {
      for (unsigned b = 1; b < (nof_drbs[u] + 1); ++b) {
        rlc_metrics = generate_rlc_metrics(ue_ids[u], b);
        du_metrics->report_metrics(rlc_metrics);
      }
    }

    // Trigger measurement collection.
    report_service->collect_measurements();
  }

  TESTASSERT_EQ(true, report_service->is_ind_msg_ready());
  // Get RIC indication msg content.
  byte_buffer ind_hdr_bytes = report_service->get_indication_header();
  byte_buffer ind_msg_bytes = report_service->get_indication_message();

  // Decode RIC Indication and check the content.
  e2_sm_kpm_ind_msg_s ric_ind_msg;
  asn1::cbit_ref      ric_ind_bref(ind_msg_bytes);
  if (ric_ind_msg.unpack(ric_ind_bref) != asn1::SRSASN_SUCCESS) {
    test_logger.debug("e2sm_kpm: RIC indication msg could not be unpacked");
    return;
  }

  TESTASSERT_EQ(nof_ues, ric_ind_msg.ind_msg_formats.ind_msg_format3().ue_meas_report_list.size());
  for (unsigned ue_idx = 0; ue_idx < nof_ues; ++ue_idx) {
    TESTASSERT_EQ(
        nof_meas_data,
        ric_ind_msg.ind_msg_formats.ind_msg_format3().ue_meas_report_list[ue_idx].meas_report.meas_data.size());
    for (unsigned i = 0; i < nof_meas_data; ++i) {
      auto& meas_record = ric_ind_msg.ind_msg_formats.ind_msg_format3()
                              .ue_meas_report_list[ue_idx]
                              .meas_report.meas_data[i]
                              .meas_record;
      TESTASSERT_EQ(nof_records, meas_record.size());
      TESTASSERT_EQ(expected_drop_rate, meas_record[0].integer());
      if (nof_records >= 2) {
        TESTASSERT_EQ(expected_dl_vol[ue_idx], meas_record[1].integer());
      }
      if (nof_records >= 3) {
        TESTASSERT_EQ(expected_ul_vol[ue_idx], meas_record[2].integer());
      }
      if (nof_records >= 4) {
        TESTASSERT_EQ(expected_ul_success_rate, meas_record[3].integer());
      }
      if (nof_records >= 5) {
        TESTASSERT_EQ(expected_ul_throughput, meas_record[4].integer());
      }
    }
  }
#if PCAP_OUTPUT
  e2_message e2_msg = generate_e2_ind_msg(ind_hdr_bytes, ind_msg_bytes);
  packer->handle_message(e2_msg);
  save_msg_pcap(gw->last_pdu);
#endif
}

TEST_F(e2sm_kpm_meas_provider_test, e2sm_kpm_ind_e2_level_rlc_metrics)
{
  std::vector<uint32_t> ue_ids        = {31, 23, 152};
  std::vector<uint32_t> nof_drbs      = {3, 1, 2};
  uint32_t              nof_meas_data = 5;
  uint32_t              nof_records   = 3;

  uint32_t expected_drop_rate = 10;
  uint32_t expected_dl_vol    = 0;
  uint32_t expected_ul_vol    = 0;

  for (auto& d : nof_drbs) {
    // Unit is kbit.
    expected_dl_vol += (10 * 1000 * d * 8 / 1000);
    expected_ul_vol += (5 * 1000 * d * 8 / 1000);
  }

  // Define E2SM_KPM action format 1.
  e2_sm_kpm_action_definition_s action_def;
  action_def.ric_style_type = 1;
  e2_sm_kpm_action_definition_format1_s& action_def_f1 =
      action_def.action_definition_formats.set_action_definition_format1();
  action_def_f1.cell_global_id_present = false;
  action_def_f1.granul_period          = 100;

  meas_info_item_s meas_info_item;
  meas_info_item.meas_type.set_meas_name().from_string("DRB.RlcPacketDropRateDl"); // Dummy metric not supported.
  label_info_item_s label_info_item;
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_opts::true_value;
  meas_info_item.label_info_list.push_back(label_info_item);
  action_def_f1.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("DRB.RlcSduTransmittedVolumeDL");
  action_def_f1.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("DRB.RlcSduTransmittedVolumeUL");
  action_def_f1.meas_info_list.push_back(meas_info_item);

  asn1::e2ap::ri_caction_to_be_setup_item_s ric_action = generate_e2sm_kpm_ric_action(action_def);

#if PCAP_OUTPUT
  // Save E2 Subscription Request.
  e2_message e2_subscript_req = generate_e2sm_kpm_subscription_request(ric_action);
  packer->handle_message(e2_subscript_req);
  save_msg_pcap(gw->last_pdu);
#endif

  ASSERT_TRUE(e2sm_iface->action_supported(ric_action));
  auto report_service = e2sm_iface->get_e2sm_report_service(ric_action.ric_action_definition);

  for (unsigned i = 0; i < nof_meas_data; ++i) {
    // Push dummy metric measurements.
    rlc_metrics rlc_metrics;
    for (unsigned u = 0; u < ue_ids.size(); ++u) {
      for (unsigned b = 1; b < (nof_drbs[u] + 1); ++b) {
        rlc_metrics = generate_rlc_metrics(ue_ids[u], b);
        du_metrics->report_metrics(rlc_metrics);
      }
    }

    // Trigger measurement collection.
    report_service->collect_measurements();
  }

  TESTASSERT_EQ(true, report_service->is_ind_msg_ready());
  // Get RIC indication msg content.
  byte_buffer ind_hdr_bytes = report_service->get_indication_header();
  byte_buffer ind_msg_bytes = report_service->get_indication_message();

  // Decode RIC Indication and check the content.
  e2_sm_kpm_ind_msg_s ric_ind_msg;
  asn1::cbit_ref      ric_ind_bref(ind_msg_bytes);
  if (ric_ind_msg.unpack(ric_ind_bref) != asn1::SRSASN_SUCCESS) {
    test_logger.debug("e2sm_kpm: RIC indication msg could not be unpacked");
    return;
  }

  TESTASSERT_EQ(nof_meas_data, ric_ind_msg.ind_msg_formats.ind_msg_format1().meas_data.size());
  for (unsigned i = 0; i < nof_meas_data; ++i) {
    auto& meas_record = ric_ind_msg.ind_msg_formats.ind_msg_format1().meas_data[i].meas_record;
    TESTASSERT_EQ(nof_records, meas_record.size());
    TESTASSERT_EQ(expected_drop_rate, meas_record[0].integer());
    TESTASSERT_EQ(expected_dl_vol, meas_record[1].integer());
    TESTASSERT_EQ(expected_ul_vol, meas_record[2].integer());
  }

#if PCAP_OUTPUT
  e2_message e2_msg = generate_e2_ind_msg(ind_hdr_bytes, ind_msg_bytes);
  packer->handle_message(e2_msg);
  save_msg_pcap(gw->last_pdu);
#endif
}