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

#include "lib/scheduler/srs/srs_scheduler_impl.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/ran/srs/srs_bandwidth_configuration.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {

class dummy_harq_timeout_notifier : public harq_timeout_notifier
{
public:
  void on_harq_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) override {}
};

} // namespace

static unsigned compute_c_srs(unsigned nof_ul_crbs)
{
  // In this test, we only consider the case where B_SRS is 0.
  const uint8_t b_srs           = 0U;
  unsigned      candidate_c_srs = 0U;
  unsigned      candidate_m_srs = 0U;
  // Spans over Table 6.4.1.4.3-1 in TS 38.211 and find the smallest C_SRS that maximizes m_srs_0 under the
  // constraint of m_SRS <= nof_BW_RBs.
  for (unsigned c_srs_it = 0; c_srs_it != 64; ++c_srs_it) {
    std::optional<srs_configuration> srs_cfg = srs_configuration_get(c_srs_it, b_srs);
    srsran_assert(srs_cfg.has_value(), "C_SRS is required for this unittest");
    if (srs_cfg.value().m_srs <= nof_ul_crbs and srs_cfg.value().m_srs > candidate_m_srs) {
      candidate_m_srs = srs_cfg->m_srs;
      candidate_c_srs = c_srs_it;
    }
  }
  return candidate_c_srs;
}

static sched_cell_configuration_request_message make_custom_sched_cell_configuration_request(bool is_tdd = false)
{
  sched_cell_configuration_request_message req = sched_config_helper::make_default_sched_cell_configuration_request(
      cell_config_builder_params{.scs_common     = is_tdd ? subcarrier_spacing::kHz30 : subcarrier_spacing::kHz15,
                                 .channel_bw_mhz = bs_channel_bandwidth::MHz20,
                                 .dl_f_ref_arfcn = is_tdd ? 520000U : 365000U});
  return req;
}

static bool is_ul_slot(unsigned offset, const tdd_ul_dl_config_common& tdd_cfg)
{
  const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
  return srsran::get_active_tdd_ul_symbols(tdd_cfg, slot_index, cyclic_prefix::NORMAL).length() != 0;
}

static sched_ue_creation_request_message
create_sched_ue_creation_request_for_srs_cfg(srs_periodicity                              srs_period,
                                             unsigned                                     nof_ul_crbs,
                                             const std::optional<tdd_ul_dl_config_common> tdd_cfg)
{
  sched_ue_creation_request_message ue_req = sched_config_helper::create_default_sched_ue_creation_request();
  auto& ue_srs_cfg = ue_req.cfg.cells.value().front().serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.value();

  // Set SRS resource set periodic.
  ue_srs_cfg.srs_res_set_list.front().res_type.emplace<srs_config::srs_resource_set::periodic_resource_type>();

  // Set SRS resource periodic.
  srs_config::srs_resource& srs_res = ue_srs_cfg.srs_res_list.front();
  srs_res.res_type                  = srs_resource_type::periodic;
  uint16_t srs_offset               = 0U;
  // The offset needs to be an UL slot.
  if (tdd_cfg.has_value()) {
    srsran_assert(static_cast<unsigned>(srs_period) >= tdd_cfg.value().pattern1.dl_ul_tx_period_nof_slots,
                  "SRS period cannot be smaller than the TDD period");

    srs_offset = test_rgen::uniform_int<uint16_t>(0, static_cast<uint16_t>(srs_period) - 1);
    while (not is_ul_slot(srs_offset, tdd_cfg.value())) {
      srs_offset = test_rgen::uniform_int<uint16_t>(0, static_cast<uint16_t>(srs_period) - 1);
    }
  } else {
    srs_offset = test_rgen::uniform_int<uint16_t>(0, static_cast<uint16_t>(srs_period) - 1);
  }
  srs_res.periodicity_and_offset.emplace(srs_config::srs_periodicity_and_offset{srs_period, srs_offset});

  // This is for 1 UE only. If more UEs are added, this should be updated.
  srs_res.tx_comb.size                 = srsran::tx_comb_size::n4;
  srs_res.tx_comb.tx_comb_offset       = 0U;
  srs_res.tx_comb.tx_comb_cyclic_shift = 0U;

  // Set SRS in symbols [13, 14).
  srs_res.res_mapping.start_pos   = 0U;
  srs_res.res_mapping.nof_symb    = n1;
  srs_res.res_mapping.rept_factor = n1;

  // Frequency hopping is disabled.
  srs_res.freq_domain_pos = 0U;
  srs_res.freq_hop.b_hop  = 0U;
  srs_res.freq_hop.b_srs  = 0U;
  srs_res.freq_hop.c_srs  = compute_c_srs(nof_ul_crbs);

  // SRS placed in the middle of the BW.
  std::optional<srs_configuration> srs_bw_cfg = srs_configuration_get(srs_res.freq_hop.c_srs, srs_res.freq_hop.b_srs);
  srsran_assert(srs_bw_cfg.has_value(), "C_SRS is required for this unittest");
  srs_res.freq_domain_shift = (nof_ul_crbs - srs_bw_cfg->m_srs) / 2U;

  srs_res.sequence_id = 0U;

  return ue_req;
}

namespace srs_periodicity_test {

struct srs_test_params {
  bool            is_tdd;
  srs_periodicity period;
};

std::ostream& operator<<(std::ostream& os, const srs_test_params& params)
{
  // Make sure there are no spaces nor colons in this string, otherwise Gtest complains about the test name being
  // invalid.
  os << "Is_tdd_" << params.is_tdd << "_srs_period_slots_" << static_cast<unsigned>(params.period);
  return os;
}

} // namespace srs_periodicity_test

using namespace srs_periodicity_test;

class test_bench
{
public:
  test_bench(srs_test_params params) :
    expert_cfg{config_helpers::make_default_scheduler_expert_config()},
    cell_cfg{[this, &params]() -> const cell_configuration& {
      auto cell_req = make_custom_sched_cell_configuration_request(params.is_tdd);
      cfg_pool.add_cell(cell_req);
      cell_cfg_list.emplace(to_du_cell_index(0), std::make_unique<cell_configuration>(expert_cfg, cell_req));
      return *cell_cfg_list[to_du_cell_index(0)];
    }()},
    cell_harqs{MAX_NOF_DU_UES, MAX_NOF_HARQS, std::make_unique<dummy_harq_timeout_notifier>()},
    srs_sched(cell_cfg, ues),
    current_sl_tx{to_numerology_value(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs), 0}
  {
    slot_indication(current_sl_tx);
    mac_logger.set_level(srslog::basic_levels::debug);
  }

  // Class members.
  scheduler_expert_config        expert_cfg;
  du_cell_group_config_pool      cfg_pool;
  cell_common_configuration_list cell_cfg_list{};
  const cell_configuration&      cell_cfg;
  cell_harq_manager              cell_harqs;
  ue_repository                  ues;
  std::vector<ue_configuration>  ue_ded_cfgs;
  cell_resource_allocator        res_grid{cell_cfg};
  srs_scheduler_impl             srs_sched;
  slot_point                     current_sl_tx;

  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  // Class methods.
  void add_ue(srs_periodicity srs_period)
  {
    sched_ue_creation_request_message ue_req = create_sched_ue_creation_request_for_srs_cfg(
        srs_period,
        cell_cfg_list[to_du_cell_index(0)]->ul_cfg_common.init_ul_bwp.generic_params.crbs.length(),
        cell_cfg.tdd_cfg_common);
    ue_ded_cfgs.emplace_back(ue_req.ue_index, ue_req.crnti, cell_cfg_list, cfg_pool.add_ue(ue_req));
    ues.add_ue(std::make_unique<ue>(ue_creation_command{ue_ded_cfgs.back(), ue_req.starts_in_fallback, cell_harqs}));
    srs_sched.add_ue(ues[ue_req.ue_index].get_pcell().cfg());
  }

  void slot_indication(slot_point slot_tx)
  {
    mac_logger.set_context(slot_tx.sfn(), slot_tx.slot_index());
    test_logger.set_context(slot_tx.sfn(), slot_tx.slot_index());
    res_grid.slot_indication(slot_tx);
  }

  expected<bool, std::string> test_srs_pdu(const srs_info& srs_pdu) const
  {
    const auto& srs_cfg     = ues[to_du_ue_index(0)].get_pcell().cfg().init_bwp().ul_ded->srs_cfg.value();
    const auto& srs_res_cfg = srs_cfg.srs_res_list.front();

    if (srs_pdu.crnti != ues[to_du_ue_index(0)].crnti) {
      return make_unexpected(std::string("RNTI mismatch"));
    }
    if (srs_pdu.bwp_cfg != &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params) {
      return make_unexpected(std::string("BWP mismatch"));
    }
    if (srs_pdu.nof_antenna_ports != cell_cfg.ul_carrier.nof_ant) {
      return make_unexpected(std::string("Nof antenna ports mismatch"));
    }
    if (srs_pdu.symbols != ofdm_symbol_range{NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - 1 - srs_res_cfg.res_mapping.start_pos,
                                             NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - 1 - srs_res_cfg.res_mapping.start_pos +
                                                 static_cast<uint8_t>(srs_res_cfg.res_mapping.nof_symb)}) {
      return make_unexpected(std::string("Symbols mismatch"));
    }
    if (srs_pdu.nof_repetitions != srs_res_cfg.res_mapping.rept_factor) {
      return make_unexpected(std::string("Repetition factor mismatch"));
    }
    if (srs_pdu.config_index != srs_res_cfg.freq_hop.c_srs) {
      return make_unexpected(std::string("Frequency domain shift mismatch"));
    }
    if (srs_pdu.sequence_id != static_cast<unsigned>(srs_res_cfg.sequence_id)) {
      return make_unexpected(std::string("Sequence ID mismatch"));
    }
    if (srs_pdu.bw_index != srs_res_cfg.freq_hop.b_srs) {
      return make_unexpected(std::string("B_srs mismatch"));
    }
    if (srs_pdu.tx_comb != srs_res_cfg.tx_comb.size) {
      return make_unexpected(std::string("TX comb size mismatch"));
    }
    if (srs_pdu.comb_offset != srs_res_cfg.tx_comb.tx_comb_offset) {
      return make_unexpected(std::string("TX comb offset mismatch"));
    }
    if (srs_pdu.cyclic_shift != srs_res_cfg.tx_comb.tx_comb_cyclic_shift) {
      return make_unexpected(std::string("TX comb cyclic shift mismatch"));
    }
    if (srs_pdu.freq_position != srs_res_cfg.freq_domain_pos) {
      return make_unexpected(std::string("Freq. domain position mismatch"));
    }
    if (srs_pdu.freq_shift != static_cast<unsigned>(srs_res_cfg.freq_domain_shift)) {
      return make_unexpected(std::string("Freq. domain position mismatch"));
    }
    if (srs_pdu.freq_hopping != srs_res_cfg.freq_hop.b_hop) {
      return make_unexpected(std::string("b_hop mismatch"));
    }
    if (srs_pdu.group_or_seq_hopping != srs_res_cfg.grp_or_seq_hop) {
      return make_unexpected(std::string("group_or_seq_hopping mismatch"));
    }
    if (srs_pdu.resource_type != srs_res_cfg.res_type) {
      return make_unexpected(std::string("group_or_seq_hopping mismatch"));
    }
    if (srs_res_cfg.res_type != srsran::srs_resource_type::aperiodic) {
      srsran_assert(srs_res_cfg.periodicity_and_offset.has_value(), "Periodicity and offset is required for this test");
      if (srs_pdu.t_srs_period != srs_res_cfg.periodicity_and_offset.value().period) {
        return make_unexpected(std::string("Periodicity mismatch"));
      }
      if (srs_pdu.t_offset != static_cast<unsigned>(srs_res_cfg.periodicity_and_offset.value().offset)) {
        return make_unexpected(std::string("Offset mismatch"));
      }
    }

    return true;
  }

  uint16_t get_offset() const
  {
    if (ues.empty()) {
      return 0;
    }
    return ues[to_du_ue_index(0U)]
        .get_pcell()
        .cfg()
        .init_bwp()
        .ul_ded->srs_cfg.value()
        .srs_res_list.front()
        .periodicity_and_offset->offset;
  }
};

class srs_scheduler_tester : public ::testing::TestWithParam<srs_test_params>, public test_bench
{
protected:
  srs_scheduler_tester() : test_bench(GetParam()) {}
};

TEST_P(srs_scheduler_tester, test_different_periods)
{
  auto srs_period_uint = static_cast<unsigned>(GetParam().period);

  const auto add_ue_slot = test_rgen::uniform_int<unsigned>(0, res_grid.RING_ALLOCATOR_SIZE);
  // Check at the allocation for at least 4 the size of the resource grid.
  const unsigned nof_slots_to_test =
      add_ue_slot + std::max(srs_period_uint * 4, static_cast<unsigned>(res_grid.RING_ALLOCATOR_SIZE) * 4);

  for (unsigned sl_cnt = 0; sl_cnt < nof_slots_to_test + add_ue_slot; ++sl_cnt) {
    // Add the UE at the specified slot.
    if (sl_cnt == add_ue_slot) {
      add_ue(GetParam().period);
    }

    srs_sched.run_slot(res_grid);
    // Only check the results once the UE has been added.
    if (not ues.empty() and (current_sl_tx - get_offset()).to_uint() % srs_period_uint == 0) {
      ASSERT_EQ(1, res_grid[0].result.ul.srss.size());
      expected<bool, std::string> pdu_test = test_srs_pdu(res_grid[0].result.ul.srss.front());
      ASSERT_TRUE(pdu_test.has_value()) << pdu_test.error();
    } else {
      ASSERT_TRUE(res_grid[0].result.ul.srss.empty());
    }

    // Update the slot indicator.
    slot_indication(++current_sl_tx);
  }
}

INSTANTIATE_TEST_SUITE_P(test_srs_scheduler_for_different_periods,
                         srs_scheduler_tester,
                         testing::Values(srs_test_params{.is_tdd = false, .period = srs_periodicity::sl1},
                                         srs_test_params{.is_tdd = false, .period = srs_periodicity::sl2},
                                         srs_test_params{.is_tdd = false, .period = srs_periodicity::sl4},
                                         srs_test_params{.is_tdd = false, .period = srs_periodicity::sl5},
                                         srs_test_params{.is_tdd = false, .period = srs_periodicity::sl8},
                                         srs_test_params{.is_tdd = false, .period = srs_periodicity::sl10},
                                         srs_test_params{.is_tdd = false, .period = srs_periodicity::sl20},
                                         srs_test_params{.is_tdd = false, .period = srs_periodicity::sl40},
                                         srs_test_params{.is_tdd = false, .period = srs_periodicity::sl80},
                                         srs_test_params{.is_tdd = true, .period = srs_periodicity::sl10},
                                         srs_test_params{.is_tdd = true, .period = srs_periodicity::sl20},
                                         srs_test_params{.is_tdd = true, .period = srs_periodicity::sl40},
                                         srs_test_params{.is_tdd = true, .period = srs_periodicity::sl80},
                                         srs_test_params{.is_tdd = true, .period = srs_periodicity::sl160},
                                         srs_test_params{.is_tdd = true, .period = srs_periodicity::sl320},
                                         srs_test_params{.is_tdd = true, .period = srs_periodicity::sl640}));

class srs_positioning_scheduler_test : public test_bench, public ::testing::Test
{
public:
  srs_positioning_scheduler_test() : test_bench(srs_test_params{true, srs_periodicity::sl20}) {}

  const srs_info* next_srs_info(rnti_t rnti, unsigned max_slots = 0)
  {
    max_slots = max_slots == 0 ? static_cast<unsigned>(srs_period) * 2 : max_slots;

    for (unsigned sl_cnt = 0; sl_cnt < max_slots; ++sl_cnt) {
      this->srs_sched.run_slot(this->res_grid);

      if (not res_grid[0].result.ul.srss.empty()) {
        EXPECT_EQ(res_grid[0].result.ul.srss.size(), 1);
        EXPECT_TRUE(res_grid[0].result.ul.srss[0].normalized_channel_iq_matrix_requested);
        if (res_grid[0].result.ul.srss[0].crnti == rnti) {
          const auto* ret = res_grid[0].result.ul.srss.data();
          slot_indication(++current_sl_tx);
          return ret;
        }
      }

      // Update the slot indicator.
      slot_indication(++current_sl_tx);
    }

    return nullptr;
  }

  const srs_periodicity srs_period = srs_periodicity::sl20;
};

TEST_F(srs_positioning_scheduler_test, when_connected_ue_positioning_is_requested_then_srs_pdu_contains_request)
{
  add_ue(srs_period);
  rnti_t rnti = to_rnti(0x4601);

  auto is_positioning_being_requested = [&]() -> bool {
    const auto* pdu = this->next_srs_info(rnti);
    report_fatal_error_if_not(pdu != nullptr, "No SRS scheduled");
    return pdu->positioning_report_requested;
  };

  // Positioning not requested.
  ASSERT_FALSE(is_positioning_being_requested());

  // Positioning requested.
  auto& ue_srs_cfg = ues[to_du_ue_index(0)].ue_cfg_dedicated()->pcell_cfg().init_bwp().ul_ded->srs_cfg.value();
  this->srs_sched.handle_positioning_measurement_request(
      positioning_measurement_request{rnti, to_du_ue_index(0), to_du_cell_index(0), ue_srs_cfg});
  ASSERT_TRUE(is_positioning_being_requested());

  // Positioning stops being requested.
  this->srs_sched.handle_positioning_measurement_stop(to_du_cell_index(0), rnti);
  ASSERT_FALSE(is_positioning_being_requested());
}

TEST_F(srs_positioning_scheduler_test, when_neighbor_cell_ue_positioning_is_requested_then_srs_pdu_contains_request)
{
  rnti_t pos_rnti = rnti_t::MIN_RESERVED_RNTI;

  // Initiate Positioning measurement.
  sched_ue_creation_request_message dummy_ue_req = create_sched_ue_creation_request_for_srs_cfg(
      srs_period,
      cell_cfg_list[to_du_cell_index(0)]->ul_cfg_common.init_ul_bwp.generic_params.crbs.length(),
      cell_cfg.tdd_cfg_common);
  auto& ue_srs_cfg = dummy_ue_req.cfg.cells.value().front().serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.value();
  this->srs_sched.handle_positioning_measurement_request(
      positioning_measurement_request{pos_rnti, std::nullopt, to_du_cell_index(0), ue_srs_cfg});

  // Positioning is being requested.
  const auto* pdu = this->next_srs_info(pos_rnti);
  ASSERT_NE(pdu, nullptr);
  ASSERT_TRUE(pdu->positioning_report_requested);

  // Stop positioning.
  this->srs_sched.handle_positioning_measurement_stop(to_du_cell_index(0), pos_rnti);

  // Positioning stops being requested.
  // Note: given that some SRS were already scheduled in the grid, we have a transition period where SRSs are scheduled
  // but without positioning.
  const unsigned max_pdu_count = 6;
  unsigned       count         = 0;
  for (; count != max_pdu_count; ++count) {
    pdu = this->next_srs_info(pos_rnti);
    if (pdu != nullptr) {
      ASSERT_FALSE(pdu->positioning_report_requested);
    } else {
      break;
    }
  }
  ASSERT_LT(count, max_pdu_count);
}
