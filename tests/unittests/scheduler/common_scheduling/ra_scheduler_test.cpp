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

#include "lib/scheduler/common_scheduling/ra_scheduler.h"
#include "lib/scheduler/logging/scheduler_event_logger.h"
#include "lib/scheduler/logging/scheduler_metrics_handler.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/support/csi_rs_helpers.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/adt/noop_functor.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/ran/resource_allocation/resource_allocation_frequency.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <ostream>

using namespace srsran;

namespace {

/// Parameters used by FDD and TDD tests.
struct test_params {
  subcarrier_spacing                     scs;
  uint8_t                                k0;
  std::vector<uint8_t>                   k2s;
  std::optional<tdd_ul_dl_config_common> tdd_config;
};

std::ostream& operator<<(std::ostream& out, const test_params& params)
{
  out << fmt::format("scs={}kHz, k0={}, k2s=[{}]", scs_to_khz(params.scs), params.k0, fmt::join(params.k2s, ", "));
  return out;
}

/// Common tester class used by FDD and TDD unit tests for the RA scheduler.
class base_ra_scheduler_test
{
protected:
  static constexpr unsigned tx_rx_delay = 2U;

  base_ra_scheduler_test(duplex_mode dplx_mode, const test_params& params_) :
    params(params_),
    cell_cfg(sched_cfg, get_sched_req(dplx_mode, params)),
    ev_logger(to_du_cell_index(0), cell_cfg.pci),
    metrics_hdlr(cell_cfg, std::nullopt)
  {
    mac_logger.set_level(srslog::basic_levels::debug);
    test_logger.set_level(srslog::basic_levels::info);
    srslog::init();

    const auto& dl_lst = cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    for (const auto& pdsch : dl_lst) {
      if (pdsch.k0 > max_k_value) {
        max_k_value = pdsch.k0;
      }
    }
    const auto& ul_lst = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    for (const auto& pusch : ul_lst) {
      if (pusch.k2 > max_k_value) {
        constexpr unsigned max_msg3_delta = 6;
        max_k_value                       = pusch.k2 + max_msg3_delta;
      }
    }

    // Run slot once so that the resource grid gets initialized with the initial slot.
    run_slot();
  }

  ~base_ra_scheduler_test()
  {
    // Log pending allocations before finishing test.
    for (unsigned i = 0; i != max_k_value; ++i) {
      run_slot();
    }
    srslog::flush();
  }

  void run_slot()
  {
    mac_logger.set_context(next_slot.sfn(), next_slot.slot_index());
    test_logger.set_context(next_slot.sfn(), next_slot.slot_index());

    res_grid.slot_indication(next_slot);
    next_slot++;

    ra_sch.run_slot(res_grid);

    result_logger.on_scheduler_result(res_grid[0].result);

    // Check sched result consistency.
    ASSERT_NO_FATAL_FAILURE(test_result_consistency());
  }

  static sched_cell_configuration_request_message get_sched_req(duplex_mode dplx_mode, const test_params& t_params)
  {
    cell_config_builder_params builder_params{};
    builder_params.scs_common = t_params.scs;
    if (dplx_mode == srsran::duplex_mode::TDD) {
      builder_params.dl_f_ref_arfcn = 520000;
      builder_params.band           = nr_band::n41;
    }
    if (t_params.scs == srsran::subcarrier_spacing::kHz30) {
      builder_params.channel_bw_mhz = srsran::bs_channel_bandwidth::MHz20;
    }

    sched_cell_configuration_request_message req =
        sched_config_helper::make_default_sched_cell_configuration_request(builder_params);

    if (dplx_mode == srsran::duplex_mode::TDD and t_params.tdd_config.has_value()) {
      req.tdd_ul_dl_cfg_common = t_params.tdd_config;
    }

    req.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list = config_helpers::make_pdsch_time_domain_resource(
        req.searchspace0, req.dl_cfg_common.init_dl_bwp.pdcch_common, std::nullopt, t_params.tdd_config);

    for (auto& pdsch_td_alloc : req.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list) {
      pdsch_td_alloc.k0 = t_params.k0;
    }

    auto& pusch_list = req.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    if (t_params.k2s.empty()) {
      pusch_list[0].k2 = test_rgen::uniform_int<unsigned>(2, 4);
    } else {
      pusch_list.resize(t_params.k2s.size(), pusch_list[0]);
      for (unsigned i = 0; i != t_params.k2s.size(); ++i) {
        pusch_list[i].k2 = t_params.k2s[i];
      }
    }

    return req;
  }

  /// \brief consistency checks that can be common to all test cases.
  void test_result_consistency()
  {
    test_scheduler_result_consistency(cell_cfg, res_grid);

    // Check all RARs have an associated PDCCH (the previous test already checks if all PDCCHs have an associated PDSCH,
    // but not the other way around).
    for (const rar_information& rar : scheduled_rars(0)) {
      ASSERT_TRUE(std::any_of(scheduled_dl_pdcchs().begin(), scheduled_dl_pdcchs().end(), [&rar](const auto& pdcch) {
        return pdcch.ctx.rnti == rar.pdsch_cfg.rnti;
      }));
    }

    // Msg3 grant checks.
    unsigned nof_grants = 0;
    unsigned nof_puschs = 0;
    for (const rar_information& rar : scheduled_rars(0)) {
      nof_grants += rar.grants.size();
    }
    for (unsigned i = 0; i != cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list.size(); ++i) {
      unsigned nof_msg3_matches = 0;
      ASSERT_TRUE(msg3_consistent_with_rars(scheduled_rars(0), scheduled_msg3_newtxs(i), nof_msg3_matches))
          << "Msg3 PUSCHs parameters must match the content of the RAR grants";
      nof_puschs += nof_msg3_matches;
    }
    ASSERT_EQ(nof_grants, nof_puschs);

    ASSERT_TRUE(res_grid[0].result.dl.ue_grants.empty());
    ASSERT_TRUE(res_grid[0].result.dl.bc.sibs.empty());
  }

  slot_point next_slot_rx() const { return next_slot - tx_rx_delay; }

  template <typename Func = noop_operation>
  void run_slot_until_next_rach_opportunity(const Func& func = {})
  {
    while (not cell_cfg.is_fully_ul_enabled(this->next_slot_rx() - 1)) {
      run_slot();
      func();
    }
  }

  void handle_rach_indication(const rach_indication_message& rach) { ra_sch.handle_rach_indication(rach); }

  static rach_indication_message::preamble create_preamble()
  {
    static auto next_rnti = test_rgen::uniform_int<unsigned>(to_value(rnti_t::MIN_CRNTI), to_value(rnti_t::MAX_CRNTI));
    static const auto rnti_inc = test_rgen::uniform_int<unsigned>(1, 5);

    rach_indication_message::preamble preamble{};
    preamble.preamble_id = test_rgen::uniform_int<unsigned>(0, 63);
    preamble.time_advance =
        phy_time_unit::from_seconds(std::uniform_real_distribution<double>{0, 2005e-6}(test_rgen::get()));
    preamble.tc_rnti = to_rnti(next_rnti);
    next_rnti += rnti_inc;
    return preamble;
  }

  rach_indication_message create_rach_indication(unsigned nof_preambles)
  {
    rach_indication_message rach_ind{};
    rach_ind.cell_index = to_du_cell_index(0);
    rach_ind.slot_rx    = next_slot_rx() - 1;
    if (nof_preambles == 0) {
      return rach_ind;
    }
    rach_ind.occasions.emplace_back();
    rach_ind.occasions.back().start_symbol    = 0;
    rach_ind.occasions.back().frequency_index = 0;

    for (unsigned i = 0; i != nof_preambles; ++i) {
      rach_ind.occasions.back().preambles.emplace_back(create_preamble());
    }
    return rach_ind;
  }

  ul_crc_indication create_crc_indication(span<const ul_sched_info> puschs, bool ack)
  {
    ul_crc_indication crc_ind;
    crc_ind.cell_index = to_du_cell_index(0);
    crc_ind.sl_rx      = this->next_slot_rx() - 1;
    for (const ul_sched_info& ul : puschs) {
      crc_ind.crcs.emplace_back();
      crc_ind.crcs.back().ue_index       = ul.context.ue_index;
      crc_ind.crcs.back().rnti           = ul.pusch_cfg.rnti;
      crc_ind.crcs.back().harq_id        = to_harq_id(0);
      crc_ind.crcs.back().tb_crc_success = ack;
    }
    return crc_ind;
  }

  const pusch_time_domain_resource_allocation& get_pusch_td_resource(uint8_t time_resource) const
  {
    return cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[time_resource];
  }

  span<const pdcch_dl_information> scheduled_dl_pdcchs() const { return res_grid[0].result.dl.dl_pdcchs; }

  span<const rar_information> scheduled_rars(uint8_t time_resource) const
  {
    unsigned k0 = cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[time_resource].k0;
    return res_grid[k0].result.dl.rar_grants;
  }

  span<const ul_sched_info> scheduled_msg3_newtxs(uint8_t time_resource) const
  {
    return res_grid[get_msg3_delay(get_pusch_td_resource(time_resource),
                                   cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs)]
        .result.ul.puschs;
  }

  bool no_rar_grants_scheduled() const
  {
    for (unsigned i = 0; i != max_k_value; ++i) {
      const auto& result = res_grid[i].result;
      if (not(result.dl.dl_pdcchs.empty() and result.dl.rar_grants.empty() and result.dl.ul_pdcchs.empty() and
              result.ul.puschs.empty())) {
        return false;
      }
    }
    return true;
  }

  bool rar_ul_grant_consistent_with_rach_preamble(const rar_ul_grant&                      rar_grant,
                                                  const rach_indication_message::preamble& preamb) const
  {
    return rar_grant.temp_crnti == preamb.tc_rnti and rar_grant.rapid == preamb.preamble_id and
           rar_grant.ta == preamb.time_advance.to_Ta(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs);
  }

  /// \brief Checks whether the parameters of the scheduled RAR grants, namely "rapid" and "TA", match those found
  /// in the detected preambles that compose the provided RACH indication.
  ///
  /// \param rars scheduled RARs.
  /// \param rach_ind RACH indication for a given slot and cell, that may contain multiple detected preambles.
  /// \param nof_detected number of RAR grants of the rand_ind that were detected in the scheduled RARs.
  /// \return true if RAR grants parameters match those of the detected preambles. False otherwise.
  bool rars_consistent_with_rach_indication(span<const rar_information>    rars,
                                            const rach_indication_message& rach_ind,
                                            unsigned&                      nof_detected)
  {
    nof_detected = 0;
    for (const rach_indication_message::occasion& occ : rach_ind.occasions) {
      // As per Section 5.1.3, TS 38.321, and from Section 5.3.2, TS 38.211, slot_idx uses as the numerology of
      // reference 15kHz for long PRACH Formats (i.e, slot_idx = subframe index); whereas, for short PRACH formats, it
      // uses the same numerology as the SCS common (i.e, slot_idx = actual slot index within the frame).
      const unsigned slot_idx =
          is_long_preamble(prach_configuration_get(
                               band_helper::get_freq_range(cell_cfg.band),
                               band_helper::get_duplex_mode(cell_cfg.band),
                               cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index)
                               .format)
              ? rach_ind.slot_rx.subframe_index()
              : rach_ind.slot_rx.slot_index();
      rnti_t                 ra_rnti = to_rnti(get_ra_rnti(slot_idx, occ.start_symbol, occ.frequency_index));
      const rar_information* rar     = find_rar(rars, ra_rnti);
      if (rar == nullptr) {
        continue;
      }

      TESTASSERT(occ.preambles.size() >= rar->grants.size(),
                 "Cannot allocate more RARs than the number of detected preambles");

      for (const rar_ul_grant& grant : rar->grants) {
        const auto* it =
            std::find_if(occ.preambles.begin(), occ.preambles.end(), [crnti = grant.temp_crnti](const auto& preamble) {
              return preamble.tc_rnti == crnti;
            });
        TESTASSERT(it != occ.preambles.end(), "RAR grant with no associated preamble was allocated");
        const rach_indication_message::preamble& preamble = *it;

        if (not rar_ul_grant_consistent_with_rach_preamble(grant, preamble)) {
          return false;
        }
        nof_detected++;
      }
    }
    return true;
  }

  /// \brief For a slot to be valid for RAR in TDD mode, the RAR PDCCH, RAR PDSCH and Msg3 PUSCH must fall in DL, DL
  /// and UL slots, respectively.
  bool is_slot_valid_for_rar_pdcch(unsigned delay = 0) const
  {
    if (not cell_cfg.is_tdd()) {
      // FDD case.
      return true;
    }
    slot_point pdcch_slot = res_grid[delay].slot;

    if (not cell_cfg.is_dl_enabled(pdcch_slot)) {
      // slot for PDCCH is not DL slot.
      return false;
    }
    const auto& pdsch_list = cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    if (std::none_of(pdsch_list.begin(), pdsch_list.end(), [this, &pdcch_slot](const auto& pdsch) {
          return cell_cfg.is_dl_enabled(pdcch_slot + pdsch.k0) and
                 not csi_helper::is_csi_rs_slot(cell_cfg, pdcch_slot + pdsch.k0);
        })) {
      // slot for PDSCH is not DL slot.
      return false;
    }
    const auto& pusch_list = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    return not std::none_of(pusch_list.begin(), pusch_list.end(), [this, &pdcch_slot](const auto& pusch) {
      return cell_cfg.is_fully_ul_enabled(pdcch_slot +
                                          get_msg3_delay(pusch, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs));
    });
  }

  bool is_slot_valid_for_msg3_retx_pdcch() const
  {
    if (not cell_cfg.is_tdd()) {
      // FDD case.
      return true;
    }
    slot_point pdcch_slot = res_grid[0].slot;

    if (not cell_cfg.is_dl_enabled(pdcch_slot)) {
      // slot for PDCCH is not DL slot.
      return false;
    }

    const auto& pusch_list = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    return not std::none_of(pusch_list.begin(), pusch_list.end(), [this, &pdcch_slot](const auto& pusch) {
      return cell_cfg.is_fully_ul_enabled(pdcch_slot + pusch.k2);
    });
  }

  bool msg3_consistent_with_rars(span<const rar_information> rars,
                                 span<const ul_sched_info>   ul_grants,
                                 unsigned&                   nof_matches)
  {
    nof_matches = 0;
    for (const rar_information& rar : rars) {
      for (const rar_ul_grant& grant : rar.grants) {
        const auto* it = std::find_if(ul_grants.begin(), ul_grants.end(), [&grant](const auto& ul_info) {
          return ul_info.pusch_cfg.rnti == grant.temp_crnti;
        });
        if (it != ul_grants.end()) {
          const pusch_information& pusch           = it->pusch_cfg;
          uint8_t                  freq_assignment = ra_frequency_type1_get_riv(
              ra_frequency_type1_configuration{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(),
                                               pusch.rbs.type1().start(),
                                               pusch.rbs.type1().length()});
          bool grant_matches = pusch.symbols == get_pusch_td_resource(grant.time_resource_assignment).symbols and
                               freq_assignment == grant.freq_resource_assignment and
                               *pusch.bwp_cfg == cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
          if (not grant_matches) {
            return false;
          }
          nof_matches++;
        }
      }
    }
    return true;
  }

  slot_interval get_rar_window(slot_point rach_slot_rx) const
  {
    slot_point rar_win_start;
    for (unsigned i = 1; i != rach_slot_rx.nof_slots_per_frame(); ++i) {
      if (cell_cfg.is_dl_enabled(rach_slot_rx + i)) {
        rar_win_start = rach_slot_rx + i;
        break;
      }
    }
    return {rar_win_start,
            rar_win_start + cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window};
  }

  bool is_in_rar_window(slot_point rach_slot_rx) const
  {
    slot_interval rar_win = get_rar_window(rach_slot_rx);
    return rar_win.contains(result_slot_tx());
  }

  static const rar_information* find_rar(span<const rar_information> rars, rnti_t ra_rnti)
  {
    const auto* rar_it =
        std::find_if(rars.begin(), rars.end(), [ra_rnti](const auto& rar) { return rar.pdsch_cfg.rnti == ra_rnti; });
    return rar_it != rars.end() ? &*rar_it : nullptr;
  }

  slot_point result_slot_tx() const { return res_grid[0].slot; }

  test_params           params;
  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  scheduler_expert_config             sched_cfg{config_helpers::make_default_scheduler_expert_config()};
  cell_configuration                  cell_cfg;
  scheduler_event_logger              ev_logger;
  scheduler_ue_metrics_dummy_notifier metrics_notifier;
  cell_metrics_handler                metrics_hdlr;
  cell_resource_allocator             res_grid{cell_cfg};
  dummy_pdcch_resource_allocator      pdcch_sch;
  ra_scheduler                        ra_sch{sched_cfg.ra, cell_cfg, pdcch_sch, ev_logger, metrics_hdlr};
  scheduler_result_logger             result_logger{false, cell_cfg.pci};

  slot_point next_slot{to_numerology_value(params.scs),
                       test_rgen::uniform_int<unsigned>(0, (10240 << to_numerology_value(params.scs)) - 1)};

  // We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  unsigned max_k_value = 0;
};

class ra_scheduler_fdd_test : public base_ra_scheduler_test, public ::testing::TestWithParam<test_params>
{
protected:
  ra_scheduler_fdd_test() : base_ra_scheduler_test(duplex_mode::FDD, GetParam()) {}
};

class ra_scheduler_tdd_test : public base_ra_scheduler_test, public ::testing::TestWithParam<test_params>
{
protected:
  ra_scheduler_tdd_test() : base_ra_scheduler_test(duplex_mode::TDD, GetParam()) {}
};

/// This test verifies that the cell resource grid remains empty when no RACH indications arrive to the RA scheduler.
TEST_P(ra_scheduler_fdd_test, when_no_rach_indication_received_then_no_rar_allocated)
{
  run_slot();
  ASSERT_TRUE(no_rar_grants_scheduled());
}
TEST_P(ra_scheduler_tdd_test, when_no_rach_indication_received_then_no_rar_allocated)
{
  run_slot();
  ASSERT_TRUE(no_rar_grants_scheduled());
}

/// This test verifies the correct scheduling of a RAR and Msg3s in an FDD frame, when multiple RACH preambles
/// are received for the same PRACH occasion.
/// The scheduler is expected to allocate one RAR and multiple MSG3 grants.
TEST_P(ra_scheduler_fdd_test, schedules_one_rar_per_slot_when_multi_preambles_with_same_prach_occasion)
{
  // Forward single RACH occasion with multiple preambles.
  rach_indication_message one_rach =
      create_rach_indication(test_rgen::uniform_int<unsigned>(1, MAX_PREAMBLES_PER_PRACH_OCCASION));
  handle_rach_indication(one_rach);

  unsigned   ra_win_size  = cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window;
  slot_point last_slot_tx = next_slot_rx() + ra_win_size;

  unsigned nof_sched_grants = 0;
  for (; nof_sched_grants < one_rach.occasions[0].preambles.size() and next_slot < last_slot_tx;) {
    run_slot();

    slot_point last_alloc_slot = res_grid[0].slot;

    if (csi_helper::is_csi_rs_slot(cell_cfg, last_alloc_slot)) {
      // CSI-RS slot, skip it.
      continue;
    }
    // RAR PDSCH allocated.
    ASSERT_EQ(scheduled_rars(0).size(), 1) << fmt::format("Only {}<{} RACHs were handled at slot={}",
                                                          nof_sched_grants,
                                                          one_rach.occasions[0].preambles.size(),
                                                          last_alloc_slot);
    unsigned nof_grants = 0;
    ASSERT_TRUE(rars_consistent_with_rach_indication(scheduled_rars(0), one_rach, nof_grants));
    ASSERT_EQ(nof_grants, scheduled_rars(0)[0].grants.size())
        << fmt::format("All scheduled RAR grants must be for the provided occasion at slot {}", last_alloc_slot);

    // Msg3 allocated.
    ASSERT_EQ(scheduled_msg3_newtxs(0).size(), nof_grants);

    // PDCCH allocated.
    ASSERT_EQ(scheduled_dl_pdcchs().size(), 1);

    nof_sched_grants += nof_grants;
  }

  ASSERT_LE(nof_sched_grants, one_rach.occasions[0].preambles.size()) << "Too many RARs allocated";
  if (nof_sched_grants < one_rach.occasions[0].preambles.size()) {
    ASSERT_EQ(next_slot, last_slot_tx) << "Not all RARs have been allocated but the RA window hasn't finished";
  }

  // For the next slot, no RAR should be scheduled.
  run_slot();
  ASSERT_TRUE(scheduled_rars(0).empty());
  ASSERT_TRUE(scheduled_msg3_newtxs(0).empty());
  ASSERT_TRUE(scheduled_dl_pdcchs().empty());
}

/// This test verifies the correct scheduling of a RAR and Msg3 in an FDD frame, when multiple RACH Preambles are
/// received, each in a different PRACH occasion.
/// The scheduler is expected to allocate several RARs (with different RA-RNTIs), each composed by one Msg3.
TEST_P(ra_scheduler_fdd_test, schedules_multiple_rars_per_slot_when_multiple_prach_occasions)
{
  // Forward multiple RACH occasions with one preamble.
  auto                    nof_occasions = test_rgen::uniform_int<unsigned>(1, MAX_PRACH_OCCASIONS_PER_SLOT);
  rach_indication_message rach_ind      = create_rach_indication(0);
  for (unsigned i = 0; i != nof_occasions; ++i) {
    rach_ind.occasions.emplace_back();
    rach_ind.occasions.back().start_symbol    = 0;
    rach_ind.occasions.back().frequency_index = i;
    rach_ind.occasions.back().preambles.emplace_back(create_preamble());
  }
  handle_rach_indication(rach_ind);

  unsigned nof_sched_grants = 0;
  for (unsigned slot_count = 0; nof_sched_grants < rach_ind.occasions[0].preambles.size(); ++slot_count) {
    ASSERT_TRUE(++slot_count < 20) << fmt::format(
        "Scheduled {} out of {} RAR grants", nof_sched_grants, rach_ind.occasions[0].preambles.size());
    run_slot();
    if (scheduled_rars(0).empty()) {
      continue;
    }

    // RAR PDSCHs allocated.
    ASSERT_LE(scheduled_rars(0).size(), rach_ind.occasions.size());
    unsigned nof_grants = 0;
    ASSERT_TRUE(rars_consistent_with_rach_indication(scheduled_rars(0), rach_ind, nof_grants));
    ASSERT_EQ(nof_grants, rach_ind.occasions.size())
        << "The number of scheduled RARs must match the number of (single-preamble) RACH occasions";

    // Since each RAR contains one single Msg3 grant, the number of scheduled Msg3 PUSCHs must match the number of RARs.
    ASSERT_EQ(scheduled_msg3_newtxs(0).size(), nof_grants) << "Number of Msg3 PUSCHs must match number of RARs";

    // PDCCH allocated.
    ASSERT_EQ(scheduled_dl_pdcchs().size(), nof_grants) << "Number of PDCCHs must match number of RARs";

    nof_sched_grants += nof_grants;
  }
  ASSERT_EQ(nof_sched_grants, nof_occasions);
}

/// This test verifies the correct scheduling of a RAR and Msg3 in an TDD frame. The scheduler has to find
/// scheduling opportunities where the RAR PDCCH and PDSCH fall in a DL slot and the Msg3 falls in an UL slot.
/// The scheduler will need to search in the PUSCH-TimeDomainResourceList provided in the cell config for a k2
/// value that allows it to fit the Msg3 in an UL slot.
TEST_P(ra_scheduler_tdd_test, schedules_rar_in_valid_slots_when_tdd)
{
  // Forward single RACH occasion with multiple preambles.
  // Note: The number of preambles is small enough to fit all grants in one slot.
  run_slot_until_next_rach_opportunity();
  rach_indication_message rach_ind = create_rach_indication(test_rgen::uniform_int<unsigned>(1, 10));
  handle_rach_indication(rach_ind);

  for (unsigned nof_sched_grants = 0; nof_sched_grants < rach_ind.occasions[0].preambles.size();) {
    run_slot();

    if (not is_slot_valid_for_rar_pdcch()) {
      ASSERT_TRUE(scheduled_dl_pdcchs().empty())
          << fmt::format("RAR PDCCH allocated in invalid slot {}", result_slot_tx());
      continue;
    }
    if (not is_in_rar_window(rach_ind.slot_rx)) {
      ASSERT_TRUE(scheduled_dl_pdcchs().empty()) << fmt::format("RAR PDCCH allocated after RAR window");
      break;
    }

    slot_point last_alloc_slot = res_grid[0].slot;

    if (csi_helper::is_csi_rs_slot(cell_cfg, last_alloc_slot)) {
      // CSI-RS slot, skip it.
      continue;
    }
    // RAR PDSCH allocated.
    ASSERT_EQ(scheduled_rars(0).size(), 1);
    unsigned nof_grants = 0;
    ASSERT_TRUE(rars_consistent_with_rach_indication(scheduled_rars(0), rach_ind, nof_grants));
    ASSERT_EQ(nof_grants, scheduled_rars(0)[0].grants.size())
        << "All scheduled RAR grants must be for the provided occasion";

    nof_sched_grants += nof_grants;
  }
}

TEST_P(ra_scheduler_tdd_test, schedules_msg3_retx_in_valid_slots_when_tdd)
{
  // Forward single RACH occasion with multiple preambles.
  // Note: The number of preambles is small enough to fit all grants in one slot.
  run_slot_until_next_rach_opportunity();
  rach_indication_message rach_ind = create_rach_indication(test_rgen::uniform_int<unsigned>(1, 10));
  handle_rach_indication(rach_ind);

  unsigned       msg3_retx_count = 0;
  const unsigned MAX_COUNT       = 100;
  for (unsigned count = 0; count != MAX_COUNT; ++count) {
    run_slot();

    if (not res_grid[0].result.dl.ul_pdcchs.empty()) {
      ASSERT_TRUE(is_slot_valid_for_msg3_retx_pdcch());
    }

    if (not res_grid[0].result.ul.puschs.empty()) {
      // Forward CRC=KO for Msg3s.
      ul_crc_indication crc_ind = create_crc_indication(res_grid[0].result.ul.puschs, false);
      this->ra_sch.handle_crc_indication(crc_ind);

      // count retxs.
      msg3_retx_count += std::count_if(res_grid[0].result.ul.puschs.begin(),
                                       res_grid[0].result.ul.puschs.end(),
                                       [](const ul_sched_info& e) { return not e.pusch_cfg.new_data; });
    }
  }

  ASSERT_GT(msg3_retx_count, 0);
}

TEST_P(ra_scheduler_tdd_test, when_no_rbs_are_available_then_rar_is_scheduled_in_following_slot)
{
  // Forward RACH indication to scheduler.
  run_slot_until_next_rach_opportunity();
  rach_indication_message rach_ind = create_rach_indication(1);
  handle_rach_indication(rach_ind);
  slot_interval rar_win = get_rar_window(rach_ind.slot_rx);

  // Forbid PDCCH alloc in the next slot.
  this->pdcch_sch.fail_pdcch_alloc_cond = [next_sl = res_grid[1].slot](slot_point pdcch_slot) {
    return pdcch_slot == next_sl;
  };

  // Process slot and schedule RAR in a future slot.
  run_slot();

  // Given that the resource grid was already filled for this slot, no RAR should be scheduled.
  ASSERT_TRUE(res_grid[0].result.dl.dl_pdcchs.empty());

  // Note: There is a small chance that the RAR was not scheduled because the next "n" (< max RA DL sched delay) slots
  // were not valid for DL RAR scheduling (not DL slots or they contain CSI-RS). For this reason, we may need to
  // call "run_slot" again.
  const unsigned attempts_remaining = 2;
  int            td_res             = -1;
  for (unsigned a = 0; a != attempts_remaining and td_res == -1; ++a) {
    unsigned       n     = 1;
    const unsigned max_n = 6;
    for (; rar_win.contains(res_grid[0].slot + n) and n != max_n; ++n) {
      if (not is_slot_valid_for_rar_pdcch(n)) {
        ASSERT_TRUE(scheduled_dl_pdcchs().empty())
            << fmt::format("RAR PDCCH allocated in invalid slot {}", result_slot_tx());
        continue;
      }

      // RAR PDCCH scheduled.
      ASSERT_EQ(res_grid[n].result.dl.dl_pdcchs.size(), 1) << fmt::format("No RAR PDCCH in slot {}", res_grid[n].slot);
      td_res = res_grid[n].result.dl.dl_pdcchs[0].dci.ra_f1_0.time_resource;
      break;
    }
    for (unsigned i = 0; i != n; ++i) {
      // Update current slot to the slot when PDCCH was scheduled.
      run_slot();
    }
  }

  ASSERT_GE(td_res, 0) << "RAR PDCCH not found";
  // RAR PDSCH allocated.
  span<const rar_information> rars = scheduled_rars(td_res);
  ASSERT_EQ(rars.size(), 1);
  unsigned nof_grants = 0;
  ASSERT_TRUE(rars_consistent_with_rach_indication(rars, rach_ind, nof_grants));
  ASSERT_EQ(nof_grants, 1);
  ASSERT_EQ(nof_grants, rars[0].grants.size()) << "All scheduled RAR grants must be for the provided occasion";
  // Msg3 scheduled.
  ASSERT_EQ(scheduled_msg3_newtxs(rars[0].grants[0].time_resource_assignment).size(), nof_grants)
      << "Number of Msg3 PUSCHs must match number of RARs";
}

INSTANTIATE_TEST_SUITE_P(ra_scheduler,
                         ra_scheduler_fdd_test,
                         ::testing::Values(test_params{.scs = subcarrier_spacing::kHz15, .k0 = 0, .k2s = {2}},
                                           test_params{.scs = subcarrier_spacing::kHz30, .k0 = 0, .k2s = {2}}));

INSTANTIATE_TEST_SUITE_P(ra_scheduler,
                         ra_scheduler_tdd_test,
                         ::testing::Values(test_params{.scs = subcarrier_spacing::kHz15, .k0 = 0, .k2s = {2, 4}},
                                           test_params{.scs = subcarrier_spacing::kHz30, .k0 = 0, .k2s = {2, 4}}));

class failed_rar_scheduler_test : public base_ra_scheduler_test, public ::testing::Test
{
protected:
  failed_rar_scheduler_test() :
    base_ra_scheduler_test(duplex_mode::TDD, test_params{.scs = subcarrier_spacing::kHz30, .k0 = 0, .k2s = {2, 4}})
  {
  }
};

TEST_F(failed_rar_scheduler_test, when_rar_grants_not_scheduled_within_window_then_future_rars_can_still_be_scheduled)
{
  unsigned grants_to_fail = MAX_NOF_DU_UES;
  for (unsigned i = 0; i * MAX_PREAMBLES_PER_PRACH_OCCASION < grants_to_fail; ++i) {
    // Keep pushing RACH indications which will lead to failed to be scheduled RARs.
    run_slot_until_next_rach_opportunity();
    rach_indication_message rach_ind = create_rach_indication(MAX_PREAMBLES_PER_PRACH_OCCASION);
    handle_rach_indication(rach_ind);
    slot_interval rar_win = get_rar_window(rach_ind.slot_rx);

    // Forbid PDCCH alloc within the RAR window to force failure.
    this->pdcch_sch.fail_pdcch_alloc_cond = [](slot_point pdcch_slot) { return true; };
    while (this->res_grid[1].slot < rar_win.stop()) {
      run_slot();

      ASSERT_TRUE(res_grid[0].result.ul.puschs.empty());
      ASSERT_TRUE(res_grid[0].result.dl.rar_grants.empty());
      ASSERT_TRUE(res_grid[0].result.dl.dl_pdcchs.empty());
    }
  }

  // Now, we should be able to schedule RARs again.
  this->pdcch_sch.fail_pdcch_alloc_cond = [](slot_point) { return false; };
  auto ack_msg3s                        = [this]() {
    if (not this->res_grid[0].result.ul.puschs.empty()) {
      // Forward CRC=OK for Msg3s.
      ul_crc_indication crc_ind = create_crc_indication(res_grid[0].result.ul.puschs, true);
      this->ra_sch.handle_crc_indication(crc_ind);
    }
  };
  unsigned grant_count = 0;
  while (grant_count < MAX_NOF_DU_UES) {
    run_slot_until_next_rach_opportunity(ack_msg3s);
    rach_indication_message rach_ind = create_rach_indication(test_rgen::uniform_int<unsigned>(1, 10));
    handle_rach_indication(rach_ind);

    for (unsigned nof_sched_grants = 0; nof_sched_grants < rach_ind.occasions[0].preambles.size();) {
      run_slot();
      ack_msg3s();

      if (not is_slot_valid_for_rar_pdcch()) {
        ASSERT_TRUE(scheduled_dl_pdcchs().empty())
            << fmt::format("RAR PDCCH allocated in invalid slot {}", result_slot_tx());
        continue;
      }
      if (not is_in_rar_window(rach_ind.slot_rx)) {
        ASSERT_TRUE(scheduled_dl_pdcchs().empty()) << fmt::format("RAR PDCCH allocated after RAR window");
        break;
      }

      slot_point last_alloc_slot = res_grid[0].slot;

      if (csi_helper::is_csi_rs_slot(cell_cfg, last_alloc_slot)) {
        // CSI-RS slot, skip it.
        continue;
      }
      // RAR PDSCH allocated.
      ASSERT_EQ(scheduled_rars(0).size(), 1);
      unsigned nof_grants = 0;
      ASSERT_TRUE(rars_consistent_with_rach_indication(scheduled_rars(0), rach_ind, nof_grants));
      ASSERT_EQ(nof_grants, scheduled_rars(0)[0].grants.size())
          << "All scheduled RAR grants must be for the provided occasion";

      nof_sched_grants += nof_grants;
    }

    grant_count += rach_ind.occasions[0].preambles.size();
  }
}

} // namespace
