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

#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/indication_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_simulator.h"
#include "srsran/ran/duplex_mode.h"
#include <gtest/gtest.h>

using namespace srsran;

struct multi_cell_scheduler_test_params {
  duplex_mode dplx_mode;
  unsigned    nof_cells = 2;
};

class base_multi_cell_scheduler_tester : public scheduler_test_simulator
{
protected:
  base_multi_cell_scheduler_tester(const multi_cell_scheduler_test_params& test_params) :
    scheduler_test_simulator(4,
                             test_params.dplx_mode == duplex_mode::FDD ? subcarrier_spacing::kHz15
                                                                       : subcarrier_spacing::kHz30)
  {
    cell_config_builder_params params = test_params.dplx_mode == duplex_mode::FDD ? cell_config_builder_profiles::fdd()
                                                                                  : cell_config_builder_profiles::tdd();

    // Add Cells.
    for (unsigned cell_idx = 0; cell_idx < test_params.nof_cells; ++cell_idx) {
      cell_cfg_builder_params_list.emplace_back(params);
      auto& added_cell = cell_cfg_builder_params_list.back();
      added_cell.pci   = cell_idx + 1;

      auto sched_cell_cfg_req = sched_config_helper::make_default_sched_cell_configuration_request(added_cell);
      sched_cell_cfg_req.cell_group_index = static_cast<du_cell_group_index_t>(cell_idx);
      sched_cell_cfg_req.cell_index       = to_du_cell_index(cell_idx);
      this->add_cell(sched_cell_cfg_req);
    }
  }

  template <typename StopCondition>
  bool run_slot_until(const StopCondition& cond_func, uint16_t cell_idx, unsigned slot_timeout = 1000)
  {
    unsigned count = 0;
    for (; count < slot_timeout; ++count) {
      run_slot();
      if (cond_func(cell_idx)) {
        break;
      }
    }
    return count < slot_timeout;
  }

  static rnti_t get_ue_crnti(uint16_t ue_idx) { return to_rnti(0x4601 + ue_idx); }

  void add_ue(uint16_t cell_idx, uint16_t ue_idx)
  {
    // Add UE
    auto ue_cfg = sched_config_helper::create_default_sched_ue_creation_request(cell_cfg_builder_params_list[cell_idx],
                                                                                {LCID_MIN_DRB});
    ue_cfg.ue_index                                 = to_du_ue_index(ue_idx);
    ue_cfg.crnti                                    = get_ue_crnti(ue_idx);
    (*ue_cfg.cfg.cells)[0].serv_cell_cfg.cell_index = to_du_cell_index(cell_idx);
    (*ue_cfg.cfg.cells)[0].serv_cell_idx            = to_serv_cell_index(cell_idx);

    scheduler_test_simulator::add_ue(ue_cfg);
  }

  static rach_indication_message::preamble create_preamble()
  {
    static auto next_rnti = test_rgen::uniform_int<unsigned>(to_value(rnti_t::MIN_CRNTI), to_value(rnti_t::MAX_CRNTI));
    static const auto rnti_inc = test_rgen::uniform_int<unsigned>(1, 5);

    rach_indication_message::preamble preamble =
        test_helper::create_preamble(test_rgen::uniform_int<unsigned>(0, 63), to_rnti(next_rnti));
    preamble.time_advance =
        phy_time_unit::from_seconds(std::uniform_real_distribution<double>{0, 2005e-6}(test_rgen::get()));

    next_rnti += rnti_inc;
    return preamble;
  }

  rach_indication_message create_rach_indication(unsigned nof_preambles, du_cell_index_t cell_idx)
  {
    std::vector<rach_indication_message::preamble> preambles(nof_preambles);
    for (unsigned i = 0; i != nof_preambles; ++i) {
      preambles[i] = this->create_preamble();
    }
    rach_indication_message rach_ind = test_helper::create_rach_indication(next_slot_rx() - 1, preambles);
    rach_ind.cell_index              = cell_idx;

    return rach_ind;
  }

  static uci_indication
  create_sr_uci_ind(uint16_t cell_idx, uint16_t ue_idx, slot_point sl_tx, const pucch_info& pucch_res)
  {
    uci_indication::uci_pdu pdu{};
    pdu.crnti    = get_ue_crnti(ue_idx);
    pdu.ue_index = to_du_ue_index(ue_idx);

    switch (pucch_res.format()) {
      case pucch_format::FORMAT_1: {
        uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu pucch_pdu{};
        pucch_pdu.sr_detected = true;
        pdu.pdu               = pucch_pdu;
      } break;
      case pucch_format::FORMAT_2: {
        uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu pucch_pdu{};
        pucch_pdu.sr_info.resize(sr_nof_bits_to_uint(pucch_res.uci_bits.sr_bits));
        pucch_pdu.sr_info.fill(0, sr_nof_bits_to_uint(pucch_res.uci_bits.sr_bits), true);
      } break;
      default:
        report_fatal_error("Not handling SR grant over PUCCH format 0, 3 and 4");
    }

    uci_indication uci_ind{};
    uci_ind.slot_rx    = sl_tx;
    uci_ind.cell_index = to_du_cell_index(cell_idx);
    uci_ind.ucis.push_back(pdu);

    return uci_ind;
  }

  std::optional<pucch_info> get_pucch_sr_scheduled(uint16_t cell_idx, uint16_t ue_idx)
  {
    if (last_sched_result(to_du_cell_index(cell_idx)) == nullptr) {
      return {};
    }
    auto* pucch_res = std::find_if(last_sched_result(to_du_cell_index(cell_idx))->ul.pucchs.begin(),
                                   last_sched_result(to_du_cell_index(cell_idx))->ul.pucchs.end(),
                                   [ue_idx](const pucch_info& pucch) { return pucch.crnti == get_ue_crnti(ue_idx); });
    if (pucch_res == last_sched_result(to_du_cell_index(cell_idx))->ul.pucchs.end()) {
      return {};
    }

    return pucch_res->uci_bits.sr_bits != sr_nof_bits::no_sr ? std::optional(*pucch_res) : std::nullopt;
  }

  std::vector<config_helpers::cell_config_builder_params_extended> cell_cfg_builder_params_list;
};

/// Formatter for test params.
void PrintTo(const multi_cell_scheduler_test_params& value, ::std::ostream* os)
{
  *os << fmt::format(
      "duplex_mode={} nof_cells={}", value.dplx_mode == duplex_mode::FDD ? "FDD" : "TDD", value.nof_cells);
}

class multi_cell_scheduler_tester : public base_multi_cell_scheduler_tester,
                                    public ::testing::TestWithParam<multi_cell_scheduler_test_params>
{
public:
  multi_cell_scheduler_tester() : base_multi_cell_scheduler_tester(GetParam()) {}
};

TEST_P(multi_cell_scheduler_tester, test_ssb_allocation_for_multiple_cells)
{
  const auto ssb_period_slots =
      ssb_periodicity_to_value(cell_cfg(to_du_cell_index(0)).ssb_cfg.ssb_period) *
      get_nof_slots_per_subframe(cell_cfg_builder_params_list[to_du_cell_index(0)].scs_common);

  std::vector<bool> is_ssb_scheduled_atleast_once(cell_cfg_builder_params_list.size(), false);
  for (unsigned slot_count = 0; slot_count < ssb_period_slots * 10; ++slot_count) {
    for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
      if (last_sched_result(to_du_cell_index(cell_idx)) != nullptr and not is_ssb_scheduled_atleast_once[cell_idx]) {
        is_ssb_scheduled_atleast_once[cell_idx] =
            not last_sched_result(to_du_cell_index(cell_idx))->dl.bc.ssb_info.empty();
      }
    }
    run_slot();
  }
  for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
    ASSERT_TRUE(is_ssb_scheduled_atleast_once[cell_idx])
        << fmt::format("SSB not scheduled for cell with index={}", cell_idx);
  }
}

TEST_P(multi_cell_scheduler_tester, test_sib1_allocation_for_multiple_cells)
{
  const auto sib1_period_slots =
      std::max(ssb_periodicity_to_value(cell_cfg(to_du_cell_index(0)).ssb_cfg.ssb_period),
               sib1_rtx_periodicity_to_value(sched_cfg.si.sib1_retx_period)) *
      get_nof_slots_per_subframe(cell_cfg_builder_params_list[to_du_cell_index(0)].scs_common);

  std::vector<bool> is_sib1_scheduled_atleast_once(cell_cfg_builder_params_list.size(), false);
  for (unsigned slot_count = 0; slot_count < sib1_period_slots * 2; ++slot_count) {
    for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
      if (last_sched_result(to_du_cell_index(cell_idx)) != nullptr and not is_sib1_scheduled_atleast_once[cell_idx]) {
        is_sib1_scheduled_atleast_once[cell_idx] =
            not last_sched_result(to_du_cell_index(cell_idx))->dl.bc.sibs.empty();
      }
    }
    run_slot();
  }
  for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
    ASSERT_TRUE(is_sib1_scheduled_atleast_once[cell_idx])
        << fmt::format("SIB1 not scheduled for cell with index={}", cell_idx);
  }
}

TEST_P(multi_cell_scheduler_tester, test_rar_scheduling_for_ues_in_different_cells)
{
  // Number of slots to run the test.
  static const unsigned test_run_nof_slots                = 200;
  static const unsigned nof_preambles_per_rach_indication = 1;

  auto next_ul_opportunity_condition = [this](uint16_t cell_index) {
    return not cell_cfg(to_du_cell_index(cell_index)).is_fully_ul_enabled(next_slot_rx() - 1);
  };
  for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
    run_slot_until(next_ul_opportunity_condition, cell_idx);
  }

  // Send one RACH indication per cell.
  for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
    sched->handle_rach_indication(
        create_rach_indication(nof_preambles_per_rach_indication, to_du_cell_index(cell_idx)));
  }

  std::vector<bool> is_rar_scheduled(cell_cfg_builder_params_list.size(), false);
  for (unsigned slot_count = 0; slot_count < test_run_nof_slots; ++slot_count) {
    for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
      if (last_sched_result(to_du_cell_index(cell_idx)) != nullptr and not is_rar_scheduled[cell_idx]) {
        is_rar_scheduled[cell_idx] = std::any_of(last_sched_result(to_du_cell_index(cell_idx))->dl.dl_pdcchs.begin(),
                                                 last_sched_result(to_du_cell_index(cell_idx))->dl.dl_pdcchs.end(),
                                                 [](const pdcch_dl_information& dl_pdcch) {
                                                   return dl_pdcch.dci.type == srsran::dci_dl_rnti_config_type::ra_f1_0;
                                                 });
      }
    }
    run_slot();
  }
  for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
    ASSERT_TRUE(is_rar_scheduled[cell_idx]) << fmt::format("RAR not scheduled for cell with index={}", cell_idx);
  }
}

TEST_P(multi_cell_scheduler_tester, test_dl_scheduling_for_ues_in_different_cells)
{
  // Number of slots to run the test.
  static const unsigned test_run_nof_slots = 100;

  // Add one UE per cell and enqueue bytes for DL tx.
  // NOTE: We add DU UE index 0 to DU cell index 0 and DU UE index 1 to DU cell index 1 and so forth for easier
  // validation of scheduling results.
  for (unsigned cell_idx = 0, ue_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++ue_idx, ++cell_idx) {
    add_ue(cell_idx, ue_idx);
    dl_buffer_state_indication_message dl_buf_st{to_du_ue_index(ue_idx), LCID_MIN_DRB, 100};
    push_dl_buffer_state(dl_buf_st);
  }

  std::vector<bool> is_ue_dl_scheduled_in_cell(cell_cfg_builder_params_list.size(), false);
  for (unsigned slot_count = 0; slot_count < test_run_nof_slots; ++slot_count) {
    for (unsigned cell_idx = 0, ue_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++ue_idx, ++cell_idx) {
      if (last_sched_result(to_du_cell_index(cell_idx)) != nullptr) {
        const bool is_ue_scheduled = std::any_of(
            last_sched_result(to_du_cell_index(cell_idx))->dl.dl_pdcchs.begin(),
            last_sched_result(to_du_cell_index(cell_idx))->dl.dl_pdcchs.end(),
            [ue_idx](const pdcch_dl_information& dl_pdcch) { return dl_pdcch.ctx.rnti == get_ue_crnti(ue_idx); });
        // Since only one UE is added by per cell, no other UEs should be scheduled in this cell i.e. ue_idx != cell_idx
        // should not be scheduled in this cell.
        if (ue_idx != cell_idx) {
          ASSERT_FALSE(is_ue_scheduled) << fmt::format(
              "DL data for UE with index={} should not be scheduled in cell with index={}", ue_idx, cell_idx);
        } else if (not is_ue_dl_scheduled_in_cell[cell_idx]) {
          is_ue_dl_scheduled_in_cell[cell_idx] = is_ue_scheduled;
        }
      }
    }
    run_slot();
  }
  // Check whether all UEs DL traffic got scheduled in their respective cell.
  for (unsigned cell_idx = 0, ue_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++ue_idx, ++cell_idx) {
    ASSERT_TRUE(is_ue_dl_scheduled_in_cell[cell_idx])
        << fmt::format("DL data for UE with index={} was not scheduled in cell with index={}", ue_idx, cell_idx);
  }
}

TEST_P(multi_cell_scheduler_tester, test_sr_indication_for_ues_in_different_cells)
{
  // Number of slots to run the test.
  static const unsigned test_run_nof_slots = 200;

  // Add one UE per cell.
  // NOTE: We add DU UE index 0 to DU cell index 0 and DU UE index 1 to DU cell index and so forth for easier validation
  // of scheduling results.
  for (unsigned cell_idx = 0, ue_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++ue_idx, ++cell_idx) {
    add_ue(cell_idx, ue_idx);
  }

  std::vector<bool> is_ue_sr_scheduled_in_cell(cell_cfg_builder_params_list.size(), false);
  for (unsigned slot_count = 0; slot_count < test_run_nof_slots; ++slot_count) {
    for (unsigned cell_idx = 0, ue_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++ue_idx, ++cell_idx) {
      // Push SR indication based on PUCCH scheduled for UE.
      const auto pucch_res = get_pucch_sr_scheduled(cell_idx, ue_idx);
      if (pucch_res.has_value() and not is_ue_sr_scheduled_in_cell[cell_idx]) {
        sched->handle_uci_indication(create_sr_uci_ind(cell_idx, ue_idx, next_slot, *pucch_res));
      }
      if (last_sched_result(to_du_cell_index(cell_idx)) != nullptr) {
        const bool is_ue_scheduled = std::any_of(
            last_sched_result(to_du_cell_index(cell_idx))->dl.ul_pdcchs.begin(),
            last_sched_result(to_du_cell_index(cell_idx))->dl.ul_pdcchs.end(),
            [ue_idx](const pdcch_ul_information& ul_pdcch) { return ul_pdcch.ctx.rnti == get_ue_crnti(ue_idx); });
        // Since only one UE is added by per cell, no other UEs should be scheduled in this cell i.e. ue_idx != cell_idx
        // should not be scheduled in this cell.
        if (ue_idx != cell_idx) {
          ASSERT_FALSE(is_ue_scheduled) << fmt::format(
              "SR for UE with index={} should not be scheduled in cell with index={}", ue_idx, cell_idx);
        } else if (not is_ue_sr_scheduled_in_cell[cell_idx]) {
          is_ue_sr_scheduled_in_cell[cell_idx] = is_ue_scheduled;
        }
      }
    }
    run_slot();
  }
  // Check whether all UE with SR got scheduled in their respective cell.
  for (unsigned cell_idx = 0, ue_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++ue_idx, ++cell_idx) {
    ASSERT_TRUE(is_ue_sr_scheduled_in_cell[cell_idx])
        << fmt::format("SR for UE with index={} was not scheduled in cell with index={}", ue_idx, cell_idx);
  }
}

TEST_P(multi_cell_scheduler_tester, test_ul_scheduling_for_ues_in_different_cells)
{
  // Number of slots to run the test.
  static const unsigned test_run_nof_slots = 100;

  // Add one UE per cell and enqueue bytes for UL tx.
  // NOTE: We add DU UE index 0 to DU cell index 0 and DU UE index 1 to DU cell index and so forth for easier validation
  // of scheduling results.
  for (unsigned cell_idx = 0, ue_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++ue_idx, ++cell_idx) {
    add_ue(cell_idx, ue_idx);
    ul_bsr_indication_message bsr{to_du_cell_index(cell_idx),
                                  to_du_ue_index(ue_idx),
                                  get_ue_crnti(ue_idx),
                                  bsr_format::SHORT_BSR,
                                  {ul_bsr_lcg_report{uint_to_lcg_id(2), 100}}};
    push_bsr(bsr);
  }

  std::vector<bool> is_ue_ul_scheduled_in_cell(cell_cfg_builder_params_list.size(), false);
  for (unsigned slot_count = 0; slot_count < test_run_nof_slots; ++slot_count) {
    for (unsigned cell_idx = 0, ue_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++ue_idx, ++cell_idx) {
      if (last_sched_result(to_du_cell_index(cell_idx)) != nullptr) {
        const bool is_ue_scheduled = std::any_of(
            last_sched_result(to_du_cell_index(cell_idx))->dl.ul_pdcchs.begin(),
            last_sched_result(to_du_cell_index(cell_idx))->dl.ul_pdcchs.end(),
            [ue_idx](const pdcch_ul_information& ul_pdcch) { return ul_pdcch.ctx.rnti == get_ue_crnti(ue_idx); });
        // Since only one UE is added by per cell, no other UEs should be scheduled in this cell i.e. ue_idx != cell_idx
        // should not be scheduled in this cell.
        if (ue_idx != cell_idx) {
          ASSERT_FALSE(is_ue_scheduled) << fmt::format(
              "UL data for UE with index={} should not be scheduled in cell with index={}", ue_idx, cell_idx);
        } else if (not is_ue_ul_scheduled_in_cell[cell_idx]) {
          is_ue_ul_scheduled_in_cell[cell_idx] = is_ue_scheduled;
        }
      }
    }
    run_slot();
  }
  // Check whether all UEs UL traffic got scheduled in their respective cell.
  for (unsigned cell_idx = 0, ue_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++ue_idx, ++cell_idx) {
    ASSERT_TRUE(is_ue_ul_scheduled_in_cell[cell_idx])
        << fmt::format("UL data for UE with index={} was not scheduled in cell with index={}", ue_idx, cell_idx);
  }
}

INSTANTIATE_TEST_SUITE_P(multi_cell_scheduler_test,
                         multi_cell_scheduler_tester,
                         testing::Values(multi_cell_scheduler_test_params{srsran::duplex_mode::FDD, 3},
                                         multi_cell_scheduler_test_params{srsran::duplex_mode::TDD, 2}));
