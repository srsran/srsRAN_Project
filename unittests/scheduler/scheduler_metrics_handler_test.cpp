/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/logging/scheduler_metrics_handler.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

class test_scheduler_ue_metrics_notifier : public scheduler_ue_metrics_notifier
{
public:
  std::vector<scheduler_ue_metrics> last_report;

  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override
  {
    last_report.assign(ue_metrics.begin(), ue_metrics.end());
  }
};

class scheduler_metrics_handler_tester : public ::testing::Test
{
protected:
  scheduler_metrics_handler_tester(
      std::chrono::milliseconds period = std::chrono::milliseconds{test_rgen::uniform_int<unsigned>(1, 100)}) :
    report_period(period), metrics(period, metrics_notif)
  {
    metrics.handle_ue_creation(test_ue_index, to_rnti(0x4601), pci_t{0});
  }

  void run_slot(const sched_result& sched_res)
  {
    metrics.push_result(next_sl_tx, sched_res);
    ++next_sl_tx;
    slot_count++;
  }

  void get_next_metric()
  {
    metrics_notif.last_report.clear();
    sched_result sched_res;
    while (metrics_notif.last_report.empty()) {
      run_slot(sched_res);
    }
  }

  std::chrono::milliseconds          report_period;
  test_scheduler_ue_metrics_notifier metrics_notif;
  scheduler_metrics_handler          metrics;
  du_ue_index_t test_ue_index = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_NOF_DU_UES - 1));

  slot_point next_sl_tx{0, test_rgen::uniform_int<unsigned>(0, 10239)};
  unsigned   slot_count = 0;
};

TEST_F(scheduler_metrics_handler_tester, metrics_sent_with_defined_periodicity)
{
  unsigned nof_reports = test_rgen::uniform_int<unsigned>(1, 10);
  ASSERT_TRUE(metrics_notif.last_report.empty());
  for (unsigned i = 0; i != nof_reports; ++i) {
    get_next_metric();
    ASSERT_EQ(metrics_notif.last_report.size(), 1);
    ASSERT_EQ(slot_count, report_period.count() * (i + 1));

    ASSERT_EQ(metrics_notif.last_report[0].rnti, to_rnti(0x4601));
  }
}

TEST_F(scheduler_metrics_handler_tester, when_no_events_took_place_then_metrics_are_zero)
{
  this->get_next_metric();
  scheduler_ue_metrics ue_metrics = metrics_notif.last_report[0];

  ASSERT_EQ(ue_metrics.rnti, to_rnti(0x4601));
  ASSERT_EQ(ue_metrics.dl_brate_kbps, 0);
  ASSERT_EQ(ue_metrics.ul_brate_kbps, 0);
  ASSERT_EQ(ue_metrics.dl_mcs, 0);
  ASSERT_EQ(ue_metrics.ul_mcs, 0);
  ASSERT_EQ(ue_metrics.dl_nof_ok, 0);
  ASSERT_EQ(ue_metrics.dl_nof_nok, 0);
  ASSERT_EQ(ue_metrics.ul_nof_ok, 0);
  ASSERT_EQ(ue_metrics.ul_nof_nok, 0);
  ASSERT_EQ(ue_metrics.pusch_snr_db, 0);
  ASSERT_EQ(ue_metrics.pucch_snr_db, 0);
  ASSERT_EQ(ue_metrics.cqi, 0);
  ASSERT_EQ(ue_metrics.bsr, 0);
}

TEST_F(scheduler_metrics_handler_tester, compute_nof_dl_oks_and_noks)
{
  unsigned nof_acks  = test_rgen::uniform_int<unsigned>(1, 100);
  unsigned nof_nacks = test_rgen::uniform_int<unsigned>(1, 100);

  for (unsigned i = 0; i != nof_acks; ++i) {
    metrics.handle_dl_harq_ack(test_ue_index, true);
  }
  for (unsigned i = 0; i != nof_nacks; ++i) {
    metrics.handle_dl_harq_ack(test_ue_index, false);
  }

  this->get_next_metric();
  scheduler_ue_metrics ue_metrics = metrics_notif.last_report[0];
  ASSERT_EQ(ue_metrics.rnti, to_rnti(0x4601));
  ASSERT_EQ(ue_metrics.dl_nof_ok, nof_acks);
  ASSERT_EQ(ue_metrics.dl_nof_nok, nof_nacks);

  this->get_next_metric();
  ue_metrics = metrics_notif.last_report[0];
  ASSERT_EQ(ue_metrics.rnti, to_rnti(0x4601));
  ASSERT_EQ(ue_metrics.dl_nof_ok, 0) << "Nof DL OKs not reset";
  ASSERT_EQ(ue_metrics.dl_nof_nok, 0) << "Nof DL NOKs not reset";
}

TEST_F(scheduler_metrics_handler_tester, compute_nof_ul_oks_and_noks)
{
  unsigned nof_acks  = test_rgen::uniform_int<unsigned>(1, 100);
  unsigned nof_nacks = test_rgen::uniform_int<unsigned>(1, 100);

  ul_crc_pdu_indication crc_pdu;
  crc_pdu.rnti           = to_rnti(0x4601);
  crc_pdu.ue_index       = test_ue_index;
  crc_pdu.tb_crc_success = true;
  for (unsigned i = 0; i != nof_acks; ++i) {
    metrics.handle_crc_indication(crc_pdu);
  }
  crc_pdu.tb_crc_success = false;
  for (unsigned i = 0; i != nof_nacks; ++i) {
    metrics.handle_crc_indication(crc_pdu);
  }

  this->get_next_metric();
  scheduler_ue_metrics ue_metrics = metrics_notif.last_report[0];
  ASSERT_EQ(ue_metrics.rnti, to_rnti(0x4601));
  ASSERT_EQ(ue_metrics.pci, pci_t{0});
  ASSERT_EQ(ue_metrics.ul_nof_ok, nof_acks);
  ASSERT_EQ(ue_metrics.ul_nof_nok, nof_nacks);

  this->get_next_metric();
  ue_metrics = metrics_notif.last_report[0];
  ASSERT_EQ(ue_metrics.rnti, to_rnti(0x4601));
  ASSERT_EQ(ue_metrics.ul_nof_ok, 0) << "Nof DL OKs not reset";
  ASSERT_EQ(ue_metrics.ul_nof_nok, 0) << "Nof DL NOKs not reset";
}

TEST_F(scheduler_metrics_handler_tester, compute_mcs_and_brate)
{
  sch_mcs_index dl_mcs{test_rgen::uniform_int<uint8_t>(1, 28)};
  sch_mcs_index ul_mcs{test_rgen::uniform_int<uint8_t>(1, 28)};

  unsigned tot_dl_tbs = 0, tot_ul_tbs = 0;

  sched_result  res;
  dl_msg_alloc& dl_msg  = res.dl.ue_grants.emplace_back();
  dl_msg.pdsch_cfg.rnti = to_rnti(0x4601);
  auto&          cw     = dl_msg.pdsch_cfg.codewords.emplace_back();
  ul_sched_info& ul_msg = res.ul.puschs.emplace_back();
  ul_msg.pusch_cfg.rnti = to_rnti(0x4601);

  for (unsigned i = 0; i != report_period.count(); ++i) {
    cw.mcs_index     = dl_mcs;
    cw.tb_size_bytes = test_rgen::uniform_int<unsigned>(1, 1000);
    tot_dl_tbs += cw.tb_size_bytes;
    ul_msg.pusch_cfg.mcs_index     = ul_mcs;
    ul_msg.pusch_cfg.tb_size_bytes = test_rgen::uniform_int<unsigned>(1, 1000);
    tot_ul_tbs += ul_msg.pusch_cfg.tb_size_bytes;
    run_slot(res);
  }

  scheduler_ue_metrics ue_metrics = metrics_notif.last_report[0];
  ASSERT_EQ(ue_metrics.rnti, to_rnti(0x4601));
  ASSERT_EQ(ue_metrics.dl_mcs, dl_mcs);
  ASSERT_EQ(ue_metrics.ul_mcs, ul_mcs);
  ASSERT_NEAR(ue_metrics.dl_brate_kbps, static_cast<double>(tot_dl_tbs * 8U) / report_period.count(), 0.001);
  ASSERT_NEAR(ue_metrics.ul_brate_kbps, static_cast<double>(tot_ul_tbs * 8U) / report_period.count(), 0.001);

  this->get_next_metric();
  ue_metrics = metrics_notif.last_report[0];
  ASSERT_EQ(ue_metrics.rnti, to_rnti(0x4601));
  ASSERT_EQ(ue_metrics.dl_mcs, 0) << "DL MCS not reset";
  ASSERT_EQ(ue_metrics.ul_mcs, 0) << "UL MCS not reset";
  ASSERT_EQ(ue_metrics.dl_brate_kbps, 0) << "DL bit rate not reset";
  ASSERT_EQ(ue_metrics.ul_brate_kbps, 0) << "UL bit rate not reset";
}
