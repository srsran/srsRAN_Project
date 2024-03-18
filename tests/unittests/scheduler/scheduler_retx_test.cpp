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

/// \file
/// \brief In this file, we verify the correct operation of the MAC scheduler, as a whole, in case of DL/UL HARQ
/// retransmissions. The objective is to cover and verify the integration of the scheduler building blocks.

#include "test_utils/config_generators.h"
#include "test_utils/indication_generators.h"
#include "test_utils/result_test_helpers.h"
#include "test_utils/scheduler_test_bench.h"
#include <gtest/gtest.h>

using namespace srsran;

class base_scheduler_retx_tester
{
protected:
  base_scheduler_retx_tester() { bench.add_cell(test_helpers::make_default_sched_cell_configuration_request()); }

  const ul_sched_info* run_until_next_pusch_alloc(unsigned max_slot_delay)
  {
    for (unsigned i = 0; i != max_slot_delay; ++i) {
      bench.run_slot();
      const ul_sched_info* grant = find_ue_pusch(ue_rnti, bench.last_sched_res_list[to_du_cell_index(0)]->ul.puschs);
      if (grant != nullptr) {
        return grant;
      }
    }
    return nullptr;
  }

  const pdcch_dl_information* run_until_next_dl_pdcch_alloc(unsigned max_slot_delay)
  {
    for (unsigned i = 0; i != max_slot_delay; ++i) {
      bench.run_slot();
      const pdcch_dl_information* pdcch = bench.find_ue_dl_pdcch(ue_rnti);
      if (pdcch != nullptr) {
        return pdcch;
      }
    }
    return nullptr;
  }

  const pdcch_ul_information* run_until_next_ul_pdcch_alloc(unsigned max_slot_delay)
  {
    for (unsigned i = 0; i != max_slot_delay; ++i) {
      bench.run_slot();
      const pdcch_ul_information* pdcch = bench.find_ue_ul_pdcch(ue_rnti);
      if (pdcch != nullptr) {
        return pdcch;
      }
    }
    return nullptr;
  }

  void notify_crc_ind(bool is_msg3, unsigned harq_id, bool ack)
  {
    ul_crc_indication crc_ind;
    crc_ind.cell_index = to_du_cell_index(0);
    crc_ind.sl_rx      = bench.next_slot_rx() - 1;
    crc_ind.crcs.resize(1);
    crc_ind.crcs[0].rnti           = ue_rnti;
    crc_ind.crcs[0].ue_index       = is_msg3 ? INVALID_DU_UE_INDEX : to_du_ue_index(0);
    crc_ind.crcs[0].harq_id        = to_harq_id(harq_id);
    crc_ind.crcs[0].tb_crc_success = ack;
    bench.sched->handle_crc_indication(crc_ind);
  }

  const rnti_t ue_rnti = to_rnti(0x4601);

  scheduler_test_bench bench;
};

struct test_params {
  unsigned nof_retxs = 0;
};

/// Formatter for test params.
void PrintTo(const test_params& value, ::std::ostream* os)
{
  *os << fmt::format("nof_retx={}", value.nof_retxs);
}

class scheduler_retx_tester : public base_scheduler_retx_tester, public ::testing::TestWithParam<test_params>
{};

TEST_P(scheduler_retx_tester, msg3_gets_retx_if_nacked)
{
  test_params  params          = GetParam();
  const size_t MAX_PUSCH_DELAY = 16, MAX_RETX_DELAY = 16;

  bench.sched->handle_rach_indication(
      create_rach_indication(bench.next_slot_rx(), {create_preamble(0, this->ue_rnti)}));

  const ul_sched_info* grant = run_until_next_pusch_alloc(MAX_PUSCH_DELAY);
  ASSERT_NE(grant, nullptr) << "No Msg3 was scheduled";
  ASSERT_EQ(grant->pusch_cfg.rnti, ue_rnti);
  ASSERT_TRUE(grant->pusch_cfg.new_data);
  const ul_sched_info msg3_newtx_grant = *grant;

  // Run until CRC slot.
  slot_point crc_slot = bench.last_result_slot();
  while (bench.next_slot_rx() != crc_slot) {
    bench.run_slot();
    ASSERT_EQ(find_ue_pusch(this->ue_rnti, *bench.last_sched_res_list[to_du_cell_index(0)]), nullptr);
  }

  // Notify ACK/NACK.
  notify_crc_ind(true, msg3_newtx_grant.pusch_cfg.harq_id, params.nof_retxs == 0);

  for (unsigned nof_retx_count = 0; nof_retx_count != params.nof_retxs; ++nof_retx_count) {
    // Ensure Msg3 retx is scheduled.
    grant = run_until_next_pusch_alloc(MAX_RETX_DELAY);
    ASSERT_NE(grant, nullptr) << "No Msg3 retx was scheduled";
    ASSERT_EQ(grant->pusch_cfg.rnti, ue_rnti);
    ASSERT_FALSE(grant->pusch_cfg.new_data);
    ASSERT_EQ(grant->pusch_cfg.harq_id, msg3_newtx_grant.pusch_cfg.harq_id);

    // Run until CRC slot.
    crc_slot = bench.last_result_slot();
    while (bench.next_slot_rx() != crc_slot) {
      bench.run_slot();
      ASSERT_EQ(find_ue_pusch(this->ue_rnti, *bench.last_sched_res_list[to_du_cell_index(0)]), nullptr);
    }

    // Notify ACK/NACK.
    notify_crc_ind(true, 0, params.nof_retxs - nof_retx_count - 1 == 0);
  }

  // No Msg3 retx should be scheduled after the HARQ is ACKed.
  grant = run_until_next_pusch_alloc(MAX_RETX_DELAY);
  ASSERT_EQ(grant, nullptr) << "Msg3 HARQ should be empty";
}

class scheduler_missing_ack_tester : public base_scheduler_retx_tester, public ::testing::Test
{
protected:
  static const unsigned MAX_HARQ_TIMEOUT = 256;
};

TEST_F(scheduler_missing_ack_tester, when_no_harq_ack_arrives_then_harq_eventually_becomes_available)
{
  static constexpr unsigned         nof_harqs     = 16;
  static constexpr rnti_t           rnti          = to_rnti(0x4601);
  sched_ue_creation_request_message ue_create_req = test_helpers::create_default_sched_ue_creation_request();
  ue_create_req.crnti                             = rnti;
  ue_create_req.ue_index                          = to_du_ue_index(0);
  (*ue_create_req.cfg.cells)[0].serv_cell_cfg.pdsch_serv_cell_cfg->nof_harq_proc =
      (pdsch_serving_cell_config::nof_harq_proc_for_pdsch)nof_harqs;
  bench.add_ue(ue_create_req);

  // Push enough bytes so that all HARQs get allocated.
  bench.push_dl_buffer_state(dl_buffer_state_indication_message{ue_create_req.ue_index, LCID_SRB1, 10000000});

  // Allocate all HARQs.
  for (unsigned i = 0; i != nof_harqs; ++i) {
    const pdcch_dl_information* pdcch = this->run_until_next_dl_pdcch_alloc(2);
    ASSERT_NE(pdcch, nullptr) << "Failed to allocate DL HARQ newtxs";
  }

  // Set buffer state to zero, so that no newtxs get allocated once the current latest_harq_states become empty.
  srslog::fetch_basic_logger("TEST").info("-- Setting DL BS=0, which will stop new DL grants --");
  bench.push_dl_buffer_state(dl_buffer_state_indication_message{ue_create_req.ue_index, LCID_SRB1, 0});
  bench.run_slot(to_du_cell_index(0));
  ASSERT_EQ(bench.find_ue_dl_pdcch(rnti), nullptr) << "No HARQs should be available at this point";

  // After several slots without HARQ-ACK, the HARQ should auto reset.
  {
    srslog::fetch_basic_logger("TEST").info("-- DL HARQs should be auto-reset --");
    const pdcch_dl_information* pdcch = this->run_until_next_dl_pdcch_alloc(MAX_HARQ_TIMEOUT);
    ASSERT_EQ(pdcch, nullptr) << "Retransmission took place without an HARQ-ACK";
  }

  // At this point, all HARQs should be free once again. Push enough bytes and verify that all HARQs get re-allocated.
  bench.push_dl_buffer_state(dl_buffer_state_indication_message{ue_create_req.ue_index, LCID_SRB1, 10000000});
  for (unsigned i = 0; i != nof_harqs; ++i) {
    const pdcch_dl_information* pdcch = this->run_until_next_dl_pdcch_alloc(2);
    ASSERT_NE(pdcch, nullptr) << "Failed to reallocate DL HARQs after timeout";
  }
}

TEST_F(scheduler_missing_ack_tester, when_no_crc_arrives_then_ul_harq_eventually_becomes_available)
{
  static constexpr unsigned         nof_harqs     = 16;
  static constexpr rnti_t           rnti          = to_rnti(0x4601);
  sched_ue_creation_request_message ue_create_req = test_helpers::create_default_sched_ue_creation_request();
  ue_create_req.crnti                             = rnti;
  ue_create_req.ue_index                          = to_du_ue_index(0);
  bench.add_ue(ue_create_req);

  // Push enough bytes so that all HARQs get allocated.
  ul_bsr_indication_message bsr{to_du_cell_index(0),
                                to_du_ue_index(0),
                                rnti,
                                bsr_format::SHORT_BSR,
                                ul_bsr_lcg_report_list{ul_bsr_lcg_report{uint_to_lcg_id(0), 100000}}};
  bench.push_bsr(bsr);

  // Allocate all UL HARQs.
  for (unsigned i = 0; i != nof_harqs; ++i) {
    const pdcch_ul_information* pdcch = this->run_until_next_ul_pdcch_alloc(2);
    ASSERT_NE(pdcch, nullptr) << "Failed to allocate UL HARQ newtxs";
  }

  // Set buffer state to zero, so that no newtxs get allocated once the current latest_harq_states become empty.
  srslog::fetch_basic_logger("TEST").info("-- Setting BSR=0, which will stop new UL grants --");
  bsr.reported_lcgs[0].nof_bytes = 0;
  bench.push_bsr(bsr);
  bench.run_slot(to_du_cell_index(0));
  ASSERT_EQ(bench.find_ue_ul_pdcch(rnti), nullptr) << "No HARQs should be available at this point";

  // After several slots without HARQ-ACK, the HARQ should auto reset.
  srslog::fetch_basic_logger("TEST").info("-- UL HARQs should be auto-reset --");
  {
    const pdcch_ul_information* pdcch = this->run_until_next_ul_pdcch_alloc(MAX_HARQ_TIMEOUT);
    ASSERT_EQ(pdcch, nullptr) << "No retx should have taken place without CRC";
  }

  // At this point, all HARQs should be free once again. Push enough bytes and verify that all HARQs get re-allocated.
  srslog::fetch_basic_logger("TEST").info("-- Pushing BSR > 0 and ensuring HARQs get reallocated. --");
  bsr.reported_lcgs[0].nof_bytes = 100000000;
  bench.push_bsr(bsr);
  for (unsigned i = 0; i != nof_harqs; ++i) {
    const pdcch_ul_information* pdcch = this->run_until_next_ul_pdcch_alloc(2);
    ASSERT_NE(pdcch, nullptr) << "Failed to reuse UL HARQs";
  }
}

class scheduler_error_indication_tester : public base_scheduler_retx_tester, public ::testing::Test
{};

TEST_F(scheduler_error_indication_tester, when_uci_lost_due_to_error_indication_then_dl_harq_is_retx)
{
  static constexpr rnti_t           rnti          = to_rnti(0x4601);
  sched_ue_creation_request_message ue_create_req = test_helpers::create_default_sched_ue_creation_request();
  ue_create_req.crnti                             = rnti;
  ue_create_req.ue_index                          = to_du_ue_index(0);
  bench.add_ue(ue_create_req);

  // Push bytes so that an HARQ gets allocated.
  bench.push_dl_buffer_state(dl_buffer_state_indication_message{ue_create_req.ue_index, LCID_SRB1, 10});

  // Search for DL PDCCH.
  const unsigned              PDSCH_TIMEOUT = 20;
  const pdcch_dl_information* pdcch         = nullptr;
  auto                        pdcch_found   = [&]() {
    pdcch = bench.find_ue_dl_pdcch(rnti);
    return pdcch != nullptr;
  };
  ASSERT_TRUE(this->bench.run_slot_until(pdcch_found, PDSCH_TIMEOUT));
  ASSERT_EQ(pdcch->dci.type, dci_dl_rnti_config_type::c_rnti_f1_1);
  harq_id_t h_id = to_harq_id(pdcch->dci.c_rnti_f1_1.harq_process_number);
  bool      ndi  = pdcch->dci.c_rnti_f1_1.tb1_new_data_indicator;

  // Wait for HARQ-ACK.
  const unsigned       ACK_TIMEOUT = 20;
  const pucch_info*    pucch       = nullptr;
  const ul_sched_info* pusch       = nullptr;
  ASSERT_TRUE(this->bench.run_slot_until(
      [&]() {
        pucch = find_ue_pucch_with_harq_ack(rnti, *this->bench.last_sched_res_list[0]);
        pusch = find_ue_pusch_with_harq_ack(rnti, *this->bench.last_sched_res_list[0]);
        return pucch != nullptr or pusch != nullptr;
      },
      ACK_TIMEOUT));

  // Error Indication should mark the HARQ as ready for reTx.
  scheduler_slot_handler::error_outcome err{};
  err.pusch_and_pucch_discarded = true;
  this->bench.sched->handle_error_indication(this->bench.last_result_slot(), to_du_cell_index(0), err);

  // The HARQ should not timeout and should be retransmitted.
  ASSERT_TRUE(this->bench.run_slot_until(pdcch_found, PDSCH_TIMEOUT));
  ASSERT_EQ(h_id, to_harq_id(pdcch->dci.c_rnti_f1_1.harq_process_number));
  ASSERT_EQ(ndi, pdcch->dci.c_rnti_f1_1.tb1_new_data_indicator) << "A HARQ retransmission should have taken place";
}

TEST_F(scheduler_error_indication_tester,
       when_pusch_with_new_data_is_lost_due_to_error_indication_then_ul_harq_is_reset)
{
  static constexpr rnti_t           rnti          = to_rnti(0x4601);
  sched_ue_creation_request_message ue_create_req = test_helpers::create_default_sched_ue_creation_request();
  ue_create_req.crnti                             = rnti;
  ue_create_req.ue_index                          = to_du_ue_index(0);
  bench.add_ue(ue_create_req);

  // Push bytes so that an UL HARQ gets allocated.
  ul_bsr_indication_message bsr{to_du_cell_index(0),
                                to_du_ue_index(0),
                                rnti,
                                bsr_format::SHORT_BSR,
                                ul_bsr_lcg_report_list{ul_bsr_lcg_report{uint_to_lcg_id(0), 10}}};
  bench.push_bsr(bsr);

  // Search for UL PDCCH.
  const unsigned              PDCCH_TIMEOUT = 20;
  const pdcch_ul_information* pdcch         = nullptr;
  auto                        pdcch_found   = [&]() {
    pdcch = bench.find_ue_ul_pdcch(rnti);
    return pdcch != nullptr;
  };
  ASSERT_TRUE(this->bench.run_slot_until(pdcch_found, PDCCH_TIMEOUT));
  ASSERT_EQ(pdcch->dci.type, dci_ul_rnti_config_type::c_rnti_f0_1);
  harq_id_t h_id = to_harq_id(pdcch->dci.c_rnti_f0_1.harq_process_number);
  bool      ndi  = pdcch->dci.c_rnti_f0_1.new_data_indicator;

  // Search for PUSCH.
  const unsigned       PUSCH_TIMEOUT = 10;
  const ul_sched_info* pusch         = nullptr;
  auto                 pusch_found   = [&]() {
    pusch = find_ue_pusch(rnti, *bench.last_sched_res_list[to_du_cell_index(0)]);
    return pusch != nullptr;
  };
  ASSERT_TRUE(this->bench.run_slot_until(pusch_found, PUSCH_TIMEOUT));
  ASSERT_TRUE(pusch->pusch_cfg.new_data);

  // Error Indication should flush the HARQ state.
  scheduler_slot_handler::error_outcome err{};
  err.pusch_and_pucch_discarded = true;
  this->bench.sched->handle_error_indication(this->bench.last_result_slot(), to_du_cell_index(0), err);

  // Search for UL PDCCH.
  ASSERT_TRUE(this->bench.run_slot_until(pdcch_found, PDCCH_TIMEOUT));
  ASSERT_EQ(h_id, to_harq_id(pdcch->dci.c_rnti_f0_1.harq_process_number));
  ASSERT_NE(pdcch->dci.c_rnti_f0_1.new_data_indicator, ndi) << "UL HARQ should have been newTx";

  // Search for PUSCH.
  ASSERT_TRUE(this->bench.run_slot_until(pusch_found, PUSCH_TIMEOUT));
  ASSERT_TRUE(pusch->pusch_cfg.new_data);
}

INSTANTIATE_TEST_SUITE_P(msg3_retx,
                         scheduler_retx_tester,
                         testing::Values(test_params{0}, test_params{1}, test_params{2}, test_params{3}));
