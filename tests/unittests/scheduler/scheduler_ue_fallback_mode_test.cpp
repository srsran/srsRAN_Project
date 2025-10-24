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

/// \file
/// \brief In this file, we test the correct behaviour of the scheduler when handling UE contention resolution, and
/// TC-RNTI to C-RNTI transitions.

#include "test_utils/config_generators.h"
#include "test_utils/indication_generators.h"
#include "test_utils/result_test_helpers.h"
#include "test_utils/scheduler_test_simulator.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "srsran/ran/duplex_mode.h"
#include <gtest/gtest.h>

using namespace srsran;

class base_scheduler_conres_test : public scheduler_test_simulator
{
public:
  base_scheduler_conres_test(duplex_mode duplx_mode = duplex_mode::FDD) :
    scheduler_test_simulator(4, duplx_mode == duplex_mode::FDD ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz30)
  {
    builder_params =
        duplx_mode == duplex_mode::TDD ? cell_config_builder_profiles::tdd() : cell_config_builder_profiles::fdd();
    builder_params.channel_bw_mhz = bs_channel_bandwidth::MHz20;

    // Create cell config with space for two PDCCHs in the SearchSpace#1.
    add_cell(sched_config_helper::make_default_sched_cell_configuration_request(builder_params));

    srsran_assert(not this->cell_cfg(to_du_cell_index(0)).nzp_csi_rs_list.empty(),
                  "This test assumes a setup with NZP CSI-RS enabled");
    srsran_assert(not this->cell_cfg(to_du_cell_index(0)).zp_csi_rs_list.empty(),
                  "This test assumes a setup with ZP CSI-RS enabled");

    // Create a UE with a DRB active.
    auto ue_cfg               = sched_config_helper::create_default_sched_ue_creation_request(builder_params, {});
    ue_cfg.ue_index           = ue_index;
    ue_cfg.crnti              = rnti;
    ue_cfg.starts_in_fallback = true;
    scheduler_test_simulator::add_ue(ue_cfg, true);
  }

  ~base_scheduler_conres_test() { srslog::flush(); }

  cell_config_builder_params builder_params{};

  du_ue_index_t ue_index = to_du_ue_index(0);
  rnti_t        rnti     = to_rnti(0x4601);
};

/// \brief Test to verify that when a PDSCH with ConRes CE is scheduled even when there is no LCID0/LCID1 PDU passed
/// down to the scheduler.
class scheduler_conres_without_pdu_test : public base_scheduler_conres_test, public ::testing::Test
{};

TEST_F(scheduler_conres_without_pdu_test, when_conres_ce_is_enqueued_and_no_msg4_is_enqueued_then_pdsch_is_scheduled)
{
  // Enqueue ConRes CE.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});

  // Ensure the ConRes CE is scheduled without a Msg4 SDU.
  ASSERT_TRUE(this->run_slot_until([this]() { return find_ue_pdsch(rnti, *this->last_sched_result()) != nullptr; }));
}

// ------------------------------------------------------------------------------------------------------------------ //

struct conres_test_params {
  lcid_t      msg4_lcid;
  duplex_mode duplx_mode;
};

/// Formatter for test params.
void PrintTo(const conres_test_params& value, ::std::ostream* os)
{
  *os << fmt::format(
      "LCID={}, mode={}", fmt::underlying(value.msg4_lcid), value.duplx_mode == duplex_mode::TDD ? "TDD" : "FDD");
}

/// \brief Test to verify the correct scheduling of the ConRes CE and Msg4 LCID0/1 PDU even when multiple PRACH
/// preambles are detected.
class scheduler_con_res_msg4_test : public base_scheduler_conres_test,
                                    public ::testing::TestWithParam<conres_test_params>
{
public:
  scheduler_con_res_msg4_test() : base_scheduler_conres_test(GetParam().duplx_mode), params(GetParam()) {}

  void enqueue_random_number_of_rach_indications()
  {
    rach_indication_message rach_ind{to_du_cell_index(0), next_slot_rx(), {{0, 0, {}}}};
    auto                    nof_preambles = test_rgen::uniform_int<unsigned>(1, 10);
    for (unsigned i = 0; i != nof_preambles; ++i) {
      rach_ind.occasions[0].preambles.push_back({i, to_rnti((uint16_t)rnti + 1 + i), phy_time_unit{}});
    }
    this->sched->handle_rach_indication(rach_ind);
  }

  conres_test_params params;
};

TEST_P(scheduler_con_res_msg4_test,
       when_conres_ce_and_srb_pdu_are_enqueued_then_tc_rnti_is_used_and_multiplexing_with_csi_rs_is_avoided)
{
  static const unsigned msg4_size = 128;

  // Enqueue several RACH indications, so that RARs that need to be scheduled may fight for RB space with the Msg4.
  enqueue_random_number_of_rach_indications();

  // Run until all RARs are scheduled.
  this->run_slot_until([this]() { return this->last_sched_result()->dl.rar_grants.empty(); });

  // Enqueue ConRes CE.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});

  // Enqueue Msg4 in SRB0/SRB1.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, params.msg4_lcid, msg4_size});

  // Ensure the Msg4 is scheduled together with the ConRes CE.
  ASSERT_TRUE(this->run_slot_until([this]() { return find_ue_pdsch(rnti, *this->last_sched_result()) != nullptr; }));
  const dl_msg_alloc* msg4_alloc = find_ue_pdsch(rnti, *this->last_sched_result());
  ASSERT_EQ(msg4_alloc->tb_list.size(), 1);
  ASSERT_EQ(msg4_alloc->tb_list[0].lc_chs_to_sched.size(), 2);
  ASSERT_EQ(msg4_alloc->tb_list[0].lc_chs_to_sched[0].lcid, lcid_dl_sch_t::UE_CON_RES_ID);
  ASSERT_EQ(msg4_alloc->tb_list[0].lc_chs_to_sched[1].lcid, params.msg4_lcid);

  // In case of Msg4 with SRB0, ensure that enough bytes are scheduled to fit the full Msg4, as RLC-TM doesn't support
  // segmentation.
  if (params.msg4_lcid == LCID_SRB0) {
    ASSERT_GE(msg4_alloc->tb_list[0].lc_chs_to_sched[1].sched_bytes, msg4_size);
  }

  // Ensure TC-RNTI is used for Msg4.
  ASSERT_EQ(find_ue_dl_pdcch(rnti)->dci.type, dci_dl_rnti_config_type::tc_rnti_f1_0);

  // Ensure no PDSCH multiplexing with CSI-RS.
  ASSERT_TRUE(this->last_sched_result()->dl.csi_rs.empty());
}

static bool is_f1_pucch(const pucch_info& pucch, bool is_common, bool has_sr)
{
  return pucch.format() == pucch_format::FORMAT_1 and ((pucch.uci_bits.sr_bits != sr_nof_bits::no_sr) == has_sr) and
         pucch.uci_bits.harq_ack_nof_bits > 0 and (pucch.resources.second_hop_prbs.empty() != is_common);
}

TEST_P(scheduler_con_res_msg4_test, while_ue_is_in_fallback_then_common_pucch_is_used)
{
  static const unsigned msg4_size = 128;
  // TODO: Increase the crnti message size, once PUCCH scheduler handles multiple HARQ-ACKs falling in the same slot
  //  in fallback mode.
  static const unsigned crnti_msg_size = 8;

  // Enqueue ConRes CE + Msg4.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, params.msg4_lcid, msg4_size});

  // Wait for ConRes + Msg4 PDCCH, PDSCH and PUCCH to be scheduled.
  ASSERT_TRUE(this->run_slot_until([this]() {
    return std::any_of(this->last_sched_result()->ul.pucchs.begin(),
                       this->last_sched_result()->ul.pucchs.end(),
                       [rnti = this->rnti](const pucch_info& pucch) {
                         return pucch.crnti == rnti and pucch.format() == pucch_format::FORMAT_1 and
                                pucch.uci_bits.harq_ack_nof_bits > 0;
                       });
  })) << "Failed to schedule ConRes CE and Msg4 PDCCH, PDSCH and PUCCH for UE in fallback mode";

  slot_point uci_slot = next_slot;
  // Decrease the slot by one, as the the \ref run_slot() function increase the slot at the end of the function.
  uci_slot -= 1;
  // Push an ACK to trigger the Contention Resolution completion in the scheduler.
  this->push_uci_indication(to_du_cell_index(0), uci_slot);
  run_slot();

  // Enqueue SRB1 data; with the UE in fallback mode, and after the MSG4 has been delivered, both common and dedicated
  // resources should be used.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, LCID_SRB1, crnti_msg_size});

  // Ensure common resources for PDCCH and PDSCH are used rather than UE-dedicated.
  ASSERT_TRUE(this->run_slot_until([this]() { return find_ue_pdsch(rnti, *this->last_sched_result()) != nullptr; }));
  const pdcch_dl_information& dl_pdcch = *find_ue_dl_pdcch(rnti);
  ASSERT_EQ(dl_pdcch.dci.type, dci_dl_rnti_config_type::c_rnti_f1_0) << "Invalid format used for UE in fallback mode";
  ASSERT_EQ(dl_pdcch.ctx.coreset_cfg->id, to_coreset_id(0));
  const dl_msg_alloc& pdsch = *find_ue_pdsch(rnti, *this->last_sched_result());
  ASSERT_EQ(pdsch.pdsch_cfg.dci_fmt, dci_dl_format::f1_0);

  // Ensure both common and PUCCH resources are used.
  struct pucch_ptrs {
    const pucch_info* f1_common_ptr = nullptr;
    const pucch_info* f1_ded_ptr    = nullptr;
    const pucch_info* f1_ded_sr_ptr = nullptr;
    const pucch_info* f2_ptr        = nullptr;
  } pucch_res_ptrs;

  ASSERT_TRUE(this->run_slot_until([this, &pucch_res_ptrs]() {
    // Depending on the SR and CSI slots, we can have different combinations of PUCCH grants. There must be at least one
    // PUCCH F1 grant using common resources, and:
    // - No PUCCH F1 ded.
    // - 1 PUCCH F1 ded. with 1 HARQ-ACK bit and NO SR.
    // - 1 PUCCH F1 ded. with 1 HARQ-ACK bit and NO SR and 1 PUCCH F1 ded. with 1 HARQ-ACK bit and SR.
    // - 1 PUCCH F2 ded. with 1 HARQ-ACK bit and CSI, with optional SR.

    const auto& pucchs     = this->last_sched_result()->ul.pucchs;
    unsigned    nof_pucchs = pucchs.size();

    if (nof_pucchs == 1) {
      if (pucchs[0].crnti == rnti and is_f1_pucch(pucchs[0], true, false)) {
        pucch_res_ptrs.f1_common_ptr = &pucchs[0];
      }
    } else if (nof_pucchs == 2) {
      for (const auto& pucch : pucchs) {
        if (pucch.crnti != rnti) {
          continue;
        }
        if (is_f1_pucch(pucch, true, false)) {
          pucch_res_ptrs.f1_common_ptr = &pucch;
        } else if (is_f1_pucch(pucch, false, false)) {
          pucch_res_ptrs.f1_ded_ptr = &pucch;
        } else if (pucch.format() == pucch_format::FORMAT_2 and pucch.uci_bits.harq_ack_nof_bits > 0) {
          pucch_res_ptrs.f2_ptr = &pucch;
        }
      }
    } else if (nof_pucchs == 3) {
      for (const auto& pucch : pucchs) {
        if (pucch.crnti == rnti and pucch.format() == pucch_format::FORMAT_1) {
          if (is_f1_pucch(pucch, true, false)) {
            pucch_res_ptrs.f1_common_ptr = &pucch;
          } else if (is_f1_pucch(pucch, false, true)) {
            pucch_res_ptrs.f1_ded_sr_ptr = &pucch;
          } else if (is_f1_pucch(pucch, false, false)) {
            pucch_res_ptrs.f1_ded_ptr = &pucch;
          }
        }
      }
    }

    return pucch_res_ptrs.f1_common_ptr != nullptr;
  }));

  ASSERT_TRUE(pucch_res_ptrs.f1_common_ptr != nullptr);
  if (pucch_res_ptrs.f1_ded_sr_ptr != nullptr) {
    ASSERT_TRUE(pucch_res_ptrs.f1_ded_ptr != nullptr);
  } else {
    ASSERT_FALSE((pucch_res_ptrs.f1_ded_ptr != nullptr) and (pucch_res_ptrs.f2_ptr != nullptr));
  }
}

TEST_P(scheduler_con_res_msg4_test, while_ue_is_in_fallback_then_common_ss_is_used)
{
  static const unsigned msg4_size = 128;

  // Enqueue ConRes CE + Msg4.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, params.msg4_lcid, msg4_size});

  // Wait for ConRes + Msg4 PDCCH to be scheduled.
  ASSERT_TRUE(this->run_slot_until([this]() { return find_ue_dl_pdcch(rnti) != nullptr; }));

  const pdcch_dl_information&       dl_pdcch          = *find_ue_dl_pdcch(rnti);
  bool                              is_common_ss_used = false;
  const search_space_configuration* ss_used           = nullptr;
  for (const search_space_configuration& ss :
       cell_cfg(to_du_cell_index(0)).dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
    if (dl_pdcch.ctx.context.ss_id == ss.get_id()) {
      is_common_ss_used = true;
      ss_used           = &ss;
      break;
    }
  }
  ASSERT_TRUE(is_common_ss_used) << "UE in fallback should use common SS";
  // PDCCH monitoring must be active in this slot.
  ASSERT_TRUE(ss_used != nullptr and pdcch_helper::is_pdcch_monitoring_active(next_slot, *ss_used)) << fmt::format(
      "Common SS id={} is not monitored at slot={}", fmt::underlying(ss_used->get_id()), next_slot.slot_index());
}

INSTANTIATE_TEST_SUITE_P(scheduler_con_res_msg4_test,
                         scheduler_con_res_msg4_test,
                         ::testing::Values(conres_test_params{LCID_SRB0, duplex_mode::FDD},
                                           conres_test_params{LCID_SRB0, duplex_mode::TDD},
                                           conres_test_params{LCID_SRB1, duplex_mode::FDD},
                                           conres_test_params{LCID_SRB1, duplex_mode::TDD}));

struct conres_expiry_params {
  subcarrier_spacing max_scs        = subcarrier_spacing::kHz15;
  unsigned           ntn_cs_koffset = 0;
};

/// Formatter for test params.
void PrintTo(const conres_expiry_params& value, ::std::ostream* os)
{
  *os << fmt::format("SCS={}, Cell-Specific-K-Offset={}", to_string(value.max_scs), value.ntn_cs_koffset);
}

class scheduler_conres_expiry_test : public scheduler_test_simulator,
                                     public ::testing::TestWithParam<conres_expiry_params>
{
protected:
  scheduler_conres_expiry_test() :
    scheduler_test_simulator(
        scheduler_test_sim_config{.max_scs = GetParam().max_scs, .ntn_cs_koffset = GetParam().ntn_cs_koffset})
  {
    // Create cell.
    auto cell_cfg_req           = sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
    cell_cfg_req.ntn_cs_koffset = ntn_cs_koffset;
    add_cell(cell_cfg_req);

    // Create a UE.
    auto ue_cfg               = sched_config_helper::create_default_sched_ue_creation_request(builder_params, {});
    ue_cfg.ue_index           = ue_index;
    ue_cfg.crnti              = rnti;
    ue_cfg.starts_in_fallback = true;
    ue_cfg.ul_ccch_slot_rx    = next_slot;
    scheduler_test_simulator::add_ue(ue_cfg, true);
    nof_rtt_slots      = cell_cfg_req.ntn_cs_koffset * next_slot.nof_slots_per_subframe();
    ul_ccch_slot_rx    = next_slot;
    conres_expiry_slot = ul_ccch_slot_rx +
                         cell_cfg_req.ul_cfg_common.init_ul_bwp.rach_cfg_common->ra_con_res_timer.count() *
                             next_slot.nof_slots_per_subframe() +
                         nof_rtt_slots;
  }

  cell_config_builder_params builder_params{cell_config_builder_profiles::tdd()};
  const du_cell_index_t      cell_index = to_du_cell_index(0);
  const du_ue_index_t        ue_index   = to_du_ue_index(0);
  const rnti_t               rnti       = to_rnti(0x4601);
  unsigned                   nof_rtt_slots;
  slot_point                 ul_ccch_slot_rx;
  slot_point                 conres_expiry_slot;
};

TEST_P(scheduler_conres_expiry_test, when_conres_ce_arrives_after_conres_timer_expires_then_no_pdsch_is_scheduled)
{
  // CE is enqueued after the ConRes timer expires.
  auto ce_enqueue_slot = conres_expiry_slot + test_rgen::uniform_int<unsigned>(0, 10);
  while (next_slot < ce_enqueue_slot) {
    run_slot();
    ASSERT_EQ(find_ue_pdsch(rnti, *last_sched_result(cell_index)), nullptr)
        << "PDSCH scheduled but there is no pending data";
  }
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});

  // Ensure the ConRes CE is not scheduled.
  ASSERT_FALSE(this->run_slot_until(
      [this]() { return find_ue_pdsch(rnti, *this->last_sched_result(cell_index)) != nullptr; }, 100));
}

TEST_P(scheduler_conres_expiry_test, when_conres_retx_goes_after_conres_timer_expiry_it_is_not_scheduled)
{
  auto pdsch_is_sched = [this]() { return find_ue_pdsch(rnti, *this->last_sched_result(cell_index)) != nullptr; };

  // Get closer to the conRes expiry slot.
  while (next_slot < conres_expiry_slot - 10) {
    run_slot();
    ASSERT_FALSE(pdsch_is_sched());
  }

  // Wait for newTx with ConRes CE to be scheduled.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});
  ASSERT_TRUE(this->run_slot_until(pdsch_is_sched, 100));

  // Wait for common PUCCH.
  ASSERT_TRUE(this->run_slot_until(
      [this]() { return find_ue_pucch(rnti, *this->last_sched_result(cell_index)) != nullptr; }, 100));

  // Enqueue a NACK after ConRes timer expires.
  uci_indication uci_ind;
  uci_ind.cell_index = cell_index;
  uci_ind.slot_rx    = next_slot - 1;
  for (const pucch_info& pucch : this->last_sched_result(cell_index)->ul.pucchs) {
    uci_ind.ucis.push_back(test_helper::create_uci_indication_pdu(ue_index, pucch, mac_harq_ack_report_status::nack));
  }
  while (next_slot < conres_expiry_slot) {
    run_slot();
    ASSERT_FALSE(pdsch_is_sched());
  }
  this->sched->handle_uci_indication(uci_ind);

  // No PDSCH should be scheduled, as the ConRes timer has expired.
  ASSERT_FALSE(this->run_slot_until(pdsch_is_sched, 100));
}

TEST_P(scheduler_conres_expiry_test, when_ntn_cell_conres_timer_extended_with_rtt)
{
  static const unsigned msg4_size = 128;
  auto pdsch_is_sched = [this]() { return find_ue_pdsch(rnti, *this->last_sched_result(cell_index)) != nullptr; };

  // Advance by link RTT.
  while (next_slot < ul_ccch_slot_rx + nof_rtt_slots) {
    run_slot();
    ASSERT_FALSE(pdsch_is_sched());
  }

  // Enqueue ConRes CE + Msg4.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, LCID_SRB1, msg4_size});

  // Wait for ConRes + Msg4 PDCCH to be scheduled.
  // PDSCH should be scheduled, as the ConRes timer is not expired.
  ASSERT_TRUE(this->run_slot_until(pdsch_is_sched, 100));
}

INSTANTIATE_TEST_SUITE_P(scheduler_conres_expiry_test,
                         scheduler_conres_expiry_test,
                         ::testing::Values(conres_expiry_params{subcarrier_spacing::kHz30, 0},
                                           conres_expiry_params{subcarrier_spacing::kHz30, 240},
                                           conres_expiry_params{subcarrier_spacing::kHz30, 480}));
