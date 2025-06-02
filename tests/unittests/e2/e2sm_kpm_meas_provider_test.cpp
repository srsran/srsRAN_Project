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

class e2_rlc_metrics_notifier : public e2_du_metrics_notifier, public e2_du_metrics_interface
{
public:
  void report_metrics(const scheduler_cell_metrics& metrics) override
  {
    if (e2_meas_provider) {
      e2_meas_provider->report_metrics(metrics);
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

class e2_pdcp_metrics_notifier : public e2_cu_metrics_notifier, public e2_cu_metrics_interface
{
public:
  void report_metrics(const pdcp_metrics_container& metrics) override
  {
    if (e2_meas_provider) {
      e2_meas_provider->report_metrics(metrics);
    }
  }

  void connect_e2_cu_meas_provider(std::unique_ptr<e2_cu_metrics_notifier> meas_provider) override {}

  void connect_e2_cu_meas_provider(e2_cu_metrics_notifier* meas_provider) { e2_meas_provider = meas_provider; }

private:
  e2_cu_metrics_notifier* e2_meas_provider;
};

class e2_entity_test_with_pcap : public e2_test_base_with_pcap
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

class e2sm_kpm_meas_provider_test : public testing::TestWithParam<dlt_pcap*>
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
    gw                   = std::make_unique<dummy_sctp_association_sdu_notifier>();
    pcap                 = std::make_unique<dummy_e2ap_pcap>();
    e2                   = std::make_unique<dummy_e2_interface>();
    if (external_pcap_writer) {
      packer = std::make_unique<e2ap_asn1_packer>(*gw, *e2, *external_pcap_writer);
    } else {
      packer = std::make_unique<e2ap_asn1_packer>(*gw, *e2, *pcap);
    }
  }

  void TearDown() override
  {
    // Flush logger after each test.
    srslog::flush();
    pcap->close();
  }

  void tick()
  {
    timers.tick();
    task_worker.run_pending_tasks();
  }
  e2ap_configuration                                   cfg = {};
  timer_factory                                        factory;
  timer_manager                                        timers;
  std::unique_ptr<dummy_sctp_association_sdu_notifier> gw;
  std::unique_ptr<e2_interface>                        e2;
  std::unique_ptr<dummy_e2ap_pcap>                     pcap;
  std::unique_ptr<e2ap_asn1_packer>                    packer;
  std::unique_ptr<e2sm_interface>                      e2sm_iface;
  std::unique_ptr<e2sm_handler>                        e2sm_packer;
  std::unique_ptr<e2_subscription_manager>             e2_subscription_mngr;
  manual_task_worker                                   task_worker{64};
  std::unique_ptr<dummy_e2_pdu_notifier>               msg_notifier;
  std::unique_ptr<dummy_e2_connection_client>          e2_client;
  srslog::basic_logger&                                test_logger = srslog::fetch_basic_logger("TEST");
};

class e2sm_kpm_du_meas_provider_test : public e2sm_kpm_meas_provider_test
{
  void SetUp() override
  {
    e2sm_kpm_meas_provider_test::SetUp();
    f1ap_ue_id_mapper = std::make_unique<dummy_f1ap_ue_id_translator>();
    du_meas_provider  = std::make_unique<e2sm_kpm_du_meas_provider_impl>(*f1ap_ue_id_mapper, 30);
    e2sm_packer       = std::make_unique<e2sm_kpm_asn1_packer>(*du_meas_provider);
    e2sm_iface        = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_packer, *du_meas_provider);
    metrics           = std::make_unique<e2_rlc_metrics_notifier>();
    metrics->connect_e2_du_meas_provider(du_meas_provider.get());
  }

  void TearDown() override { e2sm_kpm_meas_provider_test::TearDown(); }

protected:
  std::unique_ptr<e2_rlc_metrics_notifier>        metrics;
  std::unique_ptr<dummy_f1ap_ue_id_translator>    f1ap_ue_id_mapper;
  std::unique_ptr<e2sm_kpm_du_meas_provider_impl> du_meas_provider;
};

class e2sm_kpm_cu_meas_provider_test : public e2sm_kpm_meas_provider_test
{
  void SetUp() override
  {
    e2sm_kpm_meas_provider_test::SetUp();
    cu_meas_provider = std::make_unique<e2sm_kpm_cu_up_meas_provider_impl>();
    e2sm_packer      = std::make_unique<e2sm_kpm_asn1_packer>(*cu_meas_provider);
    e2sm_iface       = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_packer, *cu_meas_provider);
    metrics          = std::make_unique<e2_pdcp_metrics_notifier>();
    metrics->connect_e2_cu_meas_provider(cu_meas_provider.get());
  }

  void TearDown() override { e2sm_kpm_meas_provider_test::TearDown(); }

protected:
  std::unique_ptr<e2_pdcp_metrics_notifier>          metrics;
  std::unique_ptr<e2sm_kpm_cu_up_meas_provider_impl> cu_meas_provider;
};

rlc_metrics generate_rlc_metrics(uint32_t ue_idx, uint32_t bearer_id)
{
  rlc_metrics rlc_metric;
  rlc_metric.metrics_period        = std::chrono::milliseconds(1000);
  rlc_metric.ue_index              = static_cast<du_ue_index_t>(ue_idx);
  rlc_metric.rb_id                 = rb_id_t(drb_id_t(bearer_id));
  rlc_metric.rx.mode               = rlc_mode::am;
  rlc_metric.rx.num_pdus           = 5;
  rlc_metric.rx.num_pdu_bytes      = rlc_metric.rx.num_pdus * 1000;
  rlc_metric.rx.num_sdus           = 5;
  rlc_metric.rx.num_sdu_bytes      = rlc_metric.rx.num_sdus * 1000;
  rlc_metric.rx.num_lost_pdus      = 1;
  rlc_metric.rx.num_malformed_pdus = 0;

  rlc_metric.tx.tx_high.num_sdus                     = 10;
  rlc_metric.tx.tx_high.num_sdu_bytes                = rlc_metric.tx.tx_high.num_sdus * 1000;
  rlc_metric.tx.tx_high.num_dropped_sdus             = 1;
  rlc_metric.tx.tx_high.num_discarded_sdus           = 0;
  rlc_metric.tx.tx_high.num_discard_failures         = 0;
  rlc_metric.tx.tx_low.num_pdus_no_segmentation      = 8;
  rlc_metric.tx.tx_low.num_pdu_bytes_no_segmentation = rlc_metric.tx.tx_low.num_pdus_no_segmentation * 1000;

  rlc_metric.tx.tx_low.mode_specific = rlc_am_tx_metrics_lower{};
  auto& am                           = std::get<rlc_am_tx_metrics_lower>(rlc_metric.tx.tx_low.mode_specific);
  am.num_pdus_with_segmentation      = 2;
  am.num_pdu_bytes_with_segmentation = am.num_pdus_with_segmentation * 1000;

  return rlc_metric;
}

scheduler_cell_metrics generate_sched_metrics(uint32_t                           nof_prbs,
                                              uint32_t                           nof_slots,
                                              std::vector<std::vector<uint32_t>> dl_grants,
                                              std::vector<std::vector<uint32_t>> ul_grants)
{
  scheduler_cell_metrics sched_metric;
  sched_metric.nof_prbs     = nof_prbs;
  sched_metric.nof_dl_slots = nof_slots;
  sched_metric.nof_ul_slots = nof_slots;

  assert(dl_grants[0].size() == nof_slots);
  assert(ul_grants[0].size() == nof_slots);

  for (uint32_t ue_idx = 0; ue_idx < nof_slots; ++ue_idx) {
    scheduler_ue_metrics ue_metrics = {0};
    ue_metrics.pci                  = 1;
    ue_metrics.rnti                 = static_cast<rnti_t>(0x1000 + ue_idx);
    ue_metrics.tot_pdsch_prbs_used =
        (ue_idx < dl_grants.size()) ? std::accumulate(dl_grants[ue_idx].begin(), dl_grants[ue_idx].end(), 0) : 0;
    ue_metrics.tot_pusch_prbs_used =
        (ue_idx < ul_grants.size()) ? std::accumulate(ul_grants[ue_idx].begin(), ul_grants[ue_idx].end(), 0) : 0;
    sched_metric.ue_metrics.push_back(ue_metrics);
  }

  return sched_metric;
}

// E2 Setup Request is needed for Wireshark to correctly decode the subsequent Subscription Requests
TEST_P(e2_entity_test_with_pcap, e2sm_kpm_generates_ran_func_desc)
{
  // We need this test to generate E2 Setup Request, so Wireshark can decode the following RIC indication messages.
  test_logger.info("Launch e2 setup request procedure with task worker...");
  e2agent->start();

  // Save E2 Setup Request
  packer->handle_message(e2_client->last_tx_e2_pdu);

  // Need to send setup response, so the transaction can be completed.
  unsigned   transaction_id    = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message e2_setup_response = generate_e2_setup_response(transaction_id);
  e2_setup_response.pdu.successful_outcome()
      .value.e2setup_resp()
      ->ran_functions_accepted[0]
      ->ran_function_id_item()
      .ran_function_id = e2sm_kpm_asn1_packer::ran_func_id;
  test_logger.info("Injecting E2SetupResponse");
  e2agent->get_e2_interface().handle_message(e2_setup_response);
  e2agent->stop();
}

TEST_P(e2sm_kpm_du_meas_provider_test, e2sm_kpm_ind_three_drb_rlc_metrics)
{
  std::vector<uint32_t> ue_ids        = {31, 23, 152};
  std::vector<uint32_t> nof_drbs      = {3, 1, 2};
  uint32_t              nof_ues       = ue_ids.size();
  uint32_t              nof_meas_data = 5;
  uint32_t              nof_records   = 1;

  uint32_t              expected_drop_rate     = 10;
  float                 expected_dl_throughput = 10000 / 1e3 * 8;
  float                 expected_ul_throughput = 5000 / 1e3 * 8;
  std::vector<uint32_t> expected_dl_vol;
  std::vector<uint32_t> expected_ul_vol;

  for (auto& d : nof_drbs) {
    // Unit is kbit.
    expected_dl_vol.push_back(10 * 1000 * d * 8 / 1000);
    expected_ul_vol.push_back(5 * 1000 * d * 8 / 1000);
  }

  // Define E2SM_KPM action format 5.
  asn1::e2sm::e2sm_kpm_action_definition_s action_def;
  action_def.ric_style_type = 5;
  asn1::e2sm::e2sm_kpm_action_definition_format5_s& action_def_f5 =
      action_def.action_definition_formats.set_action_definition_format5();

  action_def_f5.matching_ue_id_list.resize(nof_ues);
  for (unsigned i = 0; i < nof_ues; i++) {
    action_def_f5.matching_ue_id_list[i].ue_id.set_gnb_du_ue_id() = generate_ueid_gnb_du(ue_ids[i]);
  }

  asn1::e2sm::e2sm_kpm_action_definition_format1_s& subscript_info = action_def_f5.sub_info;
  subscript_info.cell_global_id_present                            = false;
  subscript_info.granul_period                                     = 100;

  asn1::e2sm::meas_info_item_s meas_info_item;
  meas_info_item.meas_type.set_meas_name().from_string("DRB.RlcPacketDropRateDl");
  asn1::e2sm::label_info_item_s label_info_item{};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = asn1::e2sm::meas_label_s::no_label_opts::true_value;
  meas_info_item.label_info_list.push_back(label_info_item);
  subscript_info.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("DRB.RlcSduTransmittedVolumeDL");
  subscript_info.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("DRB.RlcSduTransmittedVolumeUL");
  subscript_info.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("DRB.UEThpDl");
  subscript_info.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("DRB.UEThpUl");
  subscript_info.meas_info_list.push_back(meas_info_item);

  nof_records = subscript_info.meas_info_list.size();

  asn1::e2ap::ric_action_to_be_setup_item_s ric_action = generate_e2sm_kpm_ric_action(action_def);

  if (g_enable_pcap) {
    // Save E2 Subscription Request.
    e2_message e2_subscript_req = generate_e2sm_kpm_subscription_request(ric_action);
    packer->handle_message(e2_subscript_req);
  }

  ASSERT_TRUE(e2sm_iface->action_supported(ric_action));
  auto report_service = e2sm_iface->get_e2sm_report_service(ric_action.ric_action_definition);

  for (unsigned i = 0; i < nof_meas_data; ++i) {
    // Push dummy metric measurements.
    rlc_metrics rlc_metrics;
    for (unsigned u = 0; u < ue_ids.size(); ++u) {
      for (unsigned b = 1; b < (nof_drbs[u] + 1); ++b) {
        rlc_metrics = generate_rlc_metrics(ue_ids[u], b);
        metrics->report_metrics(rlc_metrics);
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
  asn1::e2sm::e2sm_kpm_ind_msg_s ric_ind_msg;
  asn1::cbit_ref                 ric_ind_bref(ind_msg_bytes);
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
        TESTASSERT_EQ((i + 1) * expected_dl_vol[ue_idx], meas_record[1].integer());
      }
      if (nof_records >= 3) {
        TESTASSERT_EQ((i + 1) * expected_ul_vol[ue_idx], meas_record[2].integer());
      }
      if (nof_records >= 4) {
        TESTASSERT_EQ(expected_dl_throughput, meas_record[3].real().value);
      }
      if (nof_records >= 5) {
        TESTASSERT_EQ(expected_ul_throughput, meas_record[4].real().value);
      }
    }
  }

  if (g_enable_pcap) {
    e2_message e2_msg = generate_e2_ind_msg(ind_hdr_bytes, ind_msg_bytes);
    packer->handle_message(e2_msg);
  }
}

TEST_P(e2sm_kpm_du_meas_provider_test, e2sm_kpm_ind_e2_level_rlc_metrics)
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
  asn1::e2sm::e2sm_kpm_action_definition_s action_def;
  action_def.ric_style_type = 1;
  asn1::e2sm::e2sm_kpm_action_definition_format1_s& action_def_f1 =
      action_def.action_definition_formats.set_action_definition_format1();
  action_def_f1.cell_global_id_present = false;
  action_def_f1.granul_period          = 100;

  asn1::e2sm::meas_info_item_s meas_info_item{};
  meas_info_item.meas_type.set_meas_name().from_string("DRB.RlcPacketDropRateDl"); // Dummy metric not supported.
  asn1::e2sm::label_info_item_s label_info_item{};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = asn1::e2sm::meas_label_s::no_label_opts::true_value;
  meas_info_item.label_info_list.push_back(label_info_item);
  action_def_f1.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("DRB.RlcSduTransmittedVolumeDL");
  action_def_f1.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("DRB.RlcSduTransmittedVolumeUL");
  action_def_f1.meas_info_list.push_back(meas_info_item);

  asn1::e2ap::ric_action_to_be_setup_item_s ric_action = generate_e2sm_kpm_ric_action(action_def);

  if (g_enable_pcap) {
    // Save E2 Subscription Request.
    e2_message e2_subscript_req = generate_e2sm_kpm_subscription_request(ric_action);
    packer->handle_message(e2_subscript_req);
  }

  ASSERT_TRUE(e2sm_iface->action_supported(ric_action));
  auto report_service = e2sm_iface->get_e2sm_report_service(ric_action.ric_action_definition);

  for (unsigned i = 0; i < nof_meas_data; ++i) {
    // Push dummy metric measurements.
    rlc_metrics rlc_metrics;
    for (unsigned u = 0; u < ue_ids.size(); ++u) {
      for (unsigned b = 1; b < (nof_drbs[u] + 1); ++b) {
        rlc_metrics = generate_rlc_metrics(ue_ids[u], b);
        metrics->report_metrics(rlc_metrics);
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
  asn1::e2sm::e2sm_kpm_ind_msg_s ric_ind_msg;
  asn1::cbit_ref                 ric_ind_bref(ind_msg_bytes);
  if (ric_ind_msg.unpack(ric_ind_bref) != asn1::SRSASN_SUCCESS) {
    test_logger.debug("e2sm_kpm: RIC indication msg could not be unpacked");
    return;
  }

  TESTASSERT_EQ(nof_meas_data, ric_ind_msg.ind_msg_formats.ind_msg_format1().meas_data.size());
  for (unsigned i = 0; i < nof_meas_data; ++i) {
    auto& meas_record = ric_ind_msg.ind_msg_formats.ind_msg_format1().meas_data[i].meas_record;
    TESTASSERT_EQ(nof_records, meas_record.size());
    TESTASSERT_EQ(expected_drop_rate, meas_record[0].integer());
    TESTASSERT_EQ((i + 1) * expected_dl_vol, meas_record[1].integer());
    TESTASSERT_EQ((i + 1) * expected_ul_vol, meas_record[2].integer());
  }

  if (g_enable_pcap) {
    e2_message e2_msg = generate_e2_ind_msg(ind_hdr_bytes, ind_msg_bytes);
    packer->handle_message(e2_msg);
  }
}

TEST_P(e2sm_kpm_du_meas_provider_test, e2sm_kpm_ind_e2_level_prb_metrics)
{
  std::vector<uint32_t>              grants_ue0 = {1, 0, 3, 4, 5, 6, 7, 8, 9, 0};
  std::vector<uint32_t>              grants_ue1 = {0, 0, 8, 7, 6, 5, 4, 3, 2, 1};
  std::vector<uint32_t>              grants_ue2 = {5, 0, 5, 0, 5, 0, 5, 0, 5, 0};
  std::vector<uint32_t>              grants_ue3 = {0, 0, 0, 0, 0, 5, 5, 5, 5, 5};
  std::vector<std::vector<uint32_t>> dl_grants  = {grants_ue0, grants_ue1, grants_ue2, grants_ue3};
  std::vector<std::vector<uint32_t>> ul_grants  = {grants_ue0, grants_ue1};

  uint32_t nof_prbs      = 25;
  uint32_t nof_slots     = grants_ue0.size();
  uint32_t nof_meas_data = 1;
  uint32_t nof_records   = 6;

  uint32_t tot_dl_prbs = 0;
  uint32_t tot_ul_prbs = 0;
  for (const auto& grants : dl_grants) {
    tot_dl_prbs += std::accumulate(grants.begin(), grants.end(), 0);
  }
  for (const auto& grants : ul_grants) {
    tot_ul_prbs += std::accumulate(grants.begin(), grants.end(), 0);
  }

  uint32_t expected_dl_used_prbs  = tot_dl_prbs / nof_slots;
  uint32_t expected_ul_used_prbs  = tot_ul_prbs / nof_slots;
  uint32_t expected_dl_avail_prbs = nof_prbs - expected_dl_used_prbs;
  uint32_t expected_ul_avail_prbs = nof_prbs - expected_ul_used_prbs;
  uint32_t expected_dl_tot_prbs   = 100.0 * expected_dl_used_prbs / nof_prbs;
  uint32_t expected_ul_tot_prbs   = 100.0 * expected_ul_used_prbs / nof_prbs;

  // Define E2SM_KPM action format 1.
  asn1::e2sm::e2sm_kpm_action_definition_s action_def;
  action_def.ric_style_type = 1;
  asn1::e2sm::e2sm_kpm_action_definition_format1_s& action_def_f1 =
      action_def.action_definition_formats.set_action_definition_format1();
  action_def_f1.cell_global_id_present = false;
  action_def_f1.granul_period          = 100;

  asn1::e2sm::meas_info_item_s  meas_info_item{};
  asn1::e2sm::label_info_item_s label_info_item{};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = asn1::e2sm::meas_label_s::no_label_opts::true_value;
  meas_info_item.label_info_list.push_back(label_info_item);

  meas_info_item.meas_type.set_meas_name().from_string("RRU.PrbUsedDl");
  action_def_f1.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("RRU.PrbUsedUl");
  action_def_f1.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("RRU.PrbAvailDl");
  action_def_f1.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("RRU.PrbAvailUl");
  action_def_f1.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("RRU.PrbTotDl");
  action_def_f1.meas_info_list.push_back(meas_info_item);
  meas_info_item.meas_type.set_meas_name().from_string("RRU.PrbTotUl");
  action_def_f1.meas_info_list.push_back(meas_info_item);

  asn1::e2ap::ric_action_to_be_setup_item_s ric_action = generate_e2sm_kpm_ric_action(action_def);

  if (g_enable_pcap) {
    // Save E2 Subscription Request.
    e2_message e2_subscript_req = generate_e2sm_kpm_subscription_request(ric_action);
    packer->handle_message(e2_subscript_req);
  }

  ASSERT_TRUE(e2sm_iface->action_supported(ric_action));
  auto report_service = e2sm_iface->get_e2sm_report_service(ric_action.ric_action_definition);

  // Push dummy metric measurements.
  scheduler_cell_metrics sched_metrics;
  sched_metrics = generate_sched_metrics(nof_prbs, nof_slots, dl_grants, ul_grants);
  metrics->report_metrics(sched_metrics);

  // Trigger measurement collection.
  report_service->collect_measurements();

  TESTASSERT_EQ(true, report_service->is_ind_msg_ready());
  // Get RIC indication msg content.
  byte_buffer ind_hdr_bytes = report_service->get_indication_header();
  byte_buffer ind_msg_bytes = report_service->get_indication_message();

  // Decode RIC Indication and check the content.
  asn1::e2sm::e2sm_kpm_ind_msg_s ric_ind_msg;
  asn1::cbit_ref                 ric_ind_bref(ind_msg_bytes);
  if (ric_ind_msg.unpack(ric_ind_bref) != asn1::SRSASN_SUCCESS) {
    test_logger.debug("e2sm_kpm: RIC indication msg could not be unpacked");
    return;
  }

  TESTASSERT_EQ(nof_meas_data, ric_ind_msg.ind_msg_formats.ind_msg_format1().meas_data.size());
  auto& meas_record = ric_ind_msg.ind_msg_formats.ind_msg_format1().meas_data[0].meas_record;
  TESTASSERT_EQ(nof_records, meas_record.size());
  TESTASSERT_EQ(expected_dl_used_prbs, meas_record[0].integer());
  TESTASSERT_EQ(expected_ul_used_prbs, meas_record[1].integer());
  TESTASSERT_EQ(expected_dl_avail_prbs, meas_record[2].integer());
  TESTASSERT_EQ(expected_ul_avail_prbs, meas_record[3].integer());
  TESTASSERT_EQ(expected_dl_tot_prbs, meas_record[4].integer());
  TESTASSERT_EQ(expected_ul_tot_prbs, meas_record[5].integer());

  if (g_enable_pcap) {
    e2_message e2_msg = generate_e2_ind_msg(ind_hdr_bytes, ind_msg_bytes);
    packer->handle_message(e2_msg);
  }
}

INSTANTIATE_TEST_SUITE_P(e2sm_kpm_du_meas_provider_test, e2_entity_test_with_pcap, testing::Values(g_pcap));
INSTANTIATE_TEST_SUITE_P(e2sm_kpm_du_meas_provider_test, e2sm_kpm_du_meas_provider_test, testing::Values(g_pcap));

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
    common_pcap_writer = create_e2ap_pcap("/tmp/e2sm_kpm_meas_provider_test.pcap", *pcap_exec);
    g_pcap             = common_pcap_writer.get();
  }

  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return ret;
}
