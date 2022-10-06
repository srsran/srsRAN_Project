/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "config_generators.h"
#include "dummy_components.h"
#include "lib/scheduler/common_scheduling/ra_scheduler.h"
#include "scheduler_test_suite.h"
#include "srsgnb/ran/resource_allocation/resource_allocation_frequency.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

namespace {

std::random_device rd;
std::mt19937       g(rd());

unsigned get_random_uint(unsigned min, unsigned max)
{
  return std::uniform_int_distribution<unsigned>{min, max}(g);
}

struct test_bench {
  cell_configuration      cell_cfg;
  cell_resource_allocator res_grid{cell_cfg};
  dummy_pdcch_scheduler   pdcch_sch;
  ra_scheduler            ra_sch{cell_cfg, pdcch_sch};

  test_bench(
      const sched_cell_configuration_request_message& cell_req = make_default_sched_cell_configuration_request()) :
    cell_cfg(cell_req)
  {
  }
};

// k0 and list of k2 values.
using test_params = std::tuple<uint8_t, std::vector<uint8_t>>;

class ra_scheduler_tester : public ::testing::TestWithParam<test_params>
{
protected:
  static constexpr unsigned tx_rx_delay = 4U;
  // We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  unsigned max_k_value = 0;

  ra_scheduler_tester() { set_random_slot(); }

  ~ra_scheduler_tester() override
  {
    // Log pending allocations before finishing test.
    for (unsigned i = 0; i != max_k_value; ++i) {
      run_slot();
    }
    srslog::flush();
  }

  void
  setup_sched(const sched_cell_configuration_request_message& msg = make_default_sched_cell_configuration_request())
  {
    bench.emplace(msg);

    auto& dl_lst = bench->cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    for (auto& pdsch : dl_lst) {
      if (pdsch.k0 > max_k_value) {
        max_k_value = pdsch.k0;
      }
    }
    auto& ul_lst = bench->cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    for (auto& pusch : ul_lst) {
      if (pusch.k2 > max_k_value) {
        max_k_value = pusch.k2;
      }
    }
  }

  void handle_rach(const rach_indication_message& rach) { bench->ra_sch.handle_rach_indication(rach); }

  void run_slot()
  {
    mac_logger.set_context(next_slot.to_uint());
    test_logger.set_context(next_slot.to_uint());

    bench->res_grid.slot_indication(next_slot);
    next_slot++;

    bench->ra_sch.run_slot(bench->res_grid);

    // Check sched result consistency.
    test_result_consistency();
  }

  /// \brief consistency checks that can be common to all test cases.
  void test_result_consistency()
  {
    test_scheduler_result_consistency(bench->cell_cfg, bench->res_grid);

    // Check all RARs have an associated PDCCH (the previous test already checks if all PDCCHs have an associated PDSCH,
    // but not the other way around).
    for (const rar_information& rar : scheduled_rars(0)) {
      TESTASSERT(std::any_of(scheduled_dl_pdcchs().begin(), scheduled_dl_pdcchs().end(), [&rar](const auto& pdcch) {
        return pdcch.ctx.rnti == rar.pdsch_cfg.rnti;
      }));
    }

    // Msg3 grant checks.
    unsigned nof_grants = 0, nof_puschs = 0;
    for (const rar_information& rar : scheduled_rars(0)) {
      nof_grants += rar.grants.size();
    }
    for (unsigned i = 0; i != bench->cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list.size();
         ++i) {
      unsigned nof_msg3_matches = 0;
      ASSERT_TRUE(msg3_consistent_with_rars(scheduled_rars(0), scheduled_puschs(i), nof_msg3_matches))
          << "Msg3 PUSCHs parameters must match the content of the RAR grants";
      nof_puschs += nof_msg3_matches;
    }
    ASSERT_EQ(nof_grants, nof_puschs);

    ASSERT_TRUE(bench->res_grid[0].result.dl.ue_grants.empty());
    ASSERT_TRUE(bench->res_grid[0].result.dl.bc.sibs.empty());
  }

  void set_random_slot() { next_slot = {0, get_random_uint(0, 10239)}; }

  rach_indication_message::preamble create_preamble()
  {
    static unsigned       next_rnti = get_random_uint(MIN_CRNTI, MAX_CRNTI);
    static const unsigned rnti_inc  = get_random_uint(1, 5);

    rach_indication_message::preamble preamble{};
    preamble.preamble_id  = get_random_uint(0, 63);
    preamble.time_advance = phy_time_unit::from_seconds(std::uniform_real_distribution<double>{0, 2005e-6}(g));
    preamble.tc_rnti      = to_rnti(next_rnti);
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

  slot_point next_slot_rx() const { return next_slot - tx_rx_delay; }

  const pusch_time_domain_resource_allocation& get_pusch_td_resource(uint8_t time_resource) const
  {
    return bench->cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[time_resource];
  }

  static sched_cell_configuration_request_message create_random_cell_config_request(duplex_mode        mode,
                                                                                    const test_params& params)
  {
    sched_cell_configuration_request_message msg = make_default_sched_cell_configuration_request();
    msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0 = std::get<0>(params);

    const auto& k2s        = std::get<1>(params);
    auto&       pusch_list = msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    if (k2s.empty()) {
      pusch_list[0].k2 = get_random_uint(2, 4);
    } else {
      pusch_list.resize(k2s.size(), pusch_list[0]);
      for (unsigned i = 0; i != k2s.size(); ++i) {
        pusch_list[i].k2 = k2s[i];
      }
    }
    if (mode == srsgnb::duplex_mode::TDD) {
      msg.tdd_ul_dl_cfg_common = config_helpers::make_default_tdd_ul_dl_config_common();
    }
    return msg;
  }

  span<const pdcch_dl_information> scheduled_dl_pdcchs() { return bench->res_grid[0].result.dl.dl_pdcchs; }

  span<const rar_information> scheduled_rars(uint8_t time_resource) const
  {
    unsigned k0 = bench->cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[time_resource].k0;
    return bench->res_grid[k0].result.dl.rar_grants;
  }

  span<const ul_sched_info> scheduled_puschs(uint8_t time_resource) const
  {
    return bench
        ->res_grid[get_msg3_delay(get_pusch_td_resource(time_resource),
                                  bench->cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs)]
        .result.ul.puschs;
  }

  bool no_rar_grants_scheduled() const
  {
    for (unsigned i = 0; i != max_k_value; ++i) {
      const auto& result = bench->res_grid[i].result;
      if (not(result.dl.dl_pdcchs.empty() and result.dl.rar_grants.empty() and result.dl.ul_pdcchs.empty() and
              result.ul.puschs.empty())) {
        return false;
      }
    }
    return true;
  }

  bool rar_ul_grant_consistent_with_rach_preamble(const rar_ul_grant&                      rar_grant,
                                                  const rach_indication_message::preamble& preamb)
  {
    return rar_grant.temp_crnti == preamb.tc_rnti and rar_grant.rapid == preamb.preamble_id and
           rar_grant.ta == preamb.time_advance.to_Ta(bench->cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs);
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
      rnti_t                 ra_rnti = to_rnti(get_ra_rnti(rach_ind.slot_rx, occ.start_symbol, occ.frequency_index));
      const rar_information* rar     = find_rar(rars, ra_rnti);
      if (rar == nullptr) {
        continue;
      }

      TESTASSERT(occ.preambles.size() >= rar->grants.size(),
                 "Cannot allocate more RARs than the number of detected preambles");

      for (const rar_ul_grant& grant : rar->grants) {
        auto it = std::find_if(occ.preambles.begin(),
                               occ.preambles.end(),
                               [crnti = grant.temp_crnti](const auto& preamble) { return preamble.tc_rnti == crnti; });
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

  bool msg3_consistent_with_rars(span<const rar_information> rars,
                                 span<const ul_sched_info>   ul_grants,
                                 unsigned&                   nof_matches)
  {
    nof_matches = 0;
    for (const rar_information& rar : rars) {
      for (const rar_ul_grant& grant : rar.grants) {
        auto it = std::find_if(ul_grants.begin(), ul_grants.end(), [&grant](const auto& ul_info) {
          return ul_info.pusch_cfg.rnti == grant.temp_crnti;
        });
        if (it != ul_grants.end()) {
          const pusch_information& pusch           = it->pusch_cfg;
          uint8_t                  freq_assignment = ra_frequency_type1_get_riv(
              ra_frequency_type1_configuration{bench->cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(),
                                               pusch.prbs.prbs().start(),
                                               pusch.prbs.prbs().length()});
          bool grant_matches = pusch.symbols == get_pusch_td_resource(grant.time_resource_assignment).symbols and
                               freq_assignment == grant.freq_resource_assignment and
                               *pusch.bwp_cfg == bench->cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
          if (not grant_matches) {
            return false;
          }
          nof_matches++;
        }
      }
    }
    return true;
  }

  /// \brief For a slot to be valid for RAR in TDD mode, the RAR PDCCH, RAR PDSCH and Msg3 PUSCH must fall in DL, DL
  /// and UL slots, respectively.
  bool is_slot_valid_for_rar() const
  {
    if (not bench->cell_cfg.is_tdd()) {
      // FDD case.
      return true;
    }
    slot_point pdcch_slot = bench->res_grid[0].slot;

    if (not bench->cell_cfg.is_dl_enabled(pdcch_slot)) {
      // slot for PDCCH is not DL slot.
      return false;
    }
    const auto& pdsch_list = bench->cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    if (std::none_of(pdsch_list.begin(), pdsch_list.end(), [this, &pdcch_slot](const auto& pdsch) {
          return bench->cell_cfg.is_dl_enabled(pdcch_slot + pdsch.k0);
        })) {
      // slot for PDSCH is not DL slot.
      return false;
    }
    const auto& pusch_list = bench->cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    if (std::none_of(pusch_list.begin(), pusch_list.end(), [this, &pdcch_slot](const auto& pusch) {
          return bench->cell_cfg.is_ul_enabled(
              pdcch_slot + get_msg3_delay(pusch, bench->cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs));
        })) {
      // slot for Msg3 PUSCH is not UL slot.
      return false;
    }
    return true;
  }

  bool is_in_rar_window(slot_point rach_slot_rx) const
  {
    slot_point rar_win_start;
    for (unsigned i = 1; i != rach_slot_rx.nof_slots_per_frame(); ++i) {
      if (bench->cell_cfg.is_dl_enabled(rach_slot_rx + i)) {
        rar_win_start = rach_slot_rx + i;
        break;
      }
    }
    slot_interval rar_win = {
        rar_win_start,
        rar_win_start + bench->cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window};
    return rar_win.contains(result_slot_tx());
  }

  const rar_information* find_rar(span<const rar_information> rars, rnti_t ra_rnti) const
  {
    auto rar_it =
        std::find_if(rars.begin(), rars.end(), [ra_rnti](const auto& rar) { return rar.pdsch_cfg.rnti == ra_rnti; });
    return rar_it != rars.end() ? &*rar_it : nullptr;
  }

  slot_point result_slot_tx() const { return bench->res_grid[0].slot; }

  slot_point            next_slot{0, 0};
  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("MAC");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("MAC");
  optional<test_bench>  bench;
};

/// This test verifies that the cell resource grid remains empty when no RACH indications arrive to the RA scheduler.
TEST_P(ra_scheduler_tester, no_output_if_no_rachs)
{
  setup_sched(
      create_random_cell_config_request(get_random_uint(0, 1) == 0 ? duplex_mode::FDD : duplex_mode::TDD, GetParam()));
  run_slot();
  ASSERT_TRUE(no_rar_grants_scheduled());
}

/// This test verifies the correct scheduling of a RAR and Msg3s in an FDD frame, when multiple RACH preambles
/// are received for the same PRACH occasion.
/// The scheduler is expected to allocate one RAR and multiple MSG3 grants.
TEST_P(ra_scheduler_tester, schedules_one_rar_per_slot_when_multi_preambles_with_same_prach_occasion)
{
  setup_sched(create_random_cell_config_request(duplex_mode::FDD, GetParam()));

  // Forward single RACH occasion with multiple preambles.
  rach_indication_message one_rach = create_rach_indication(get_random_uint(1, prach_constants::MAX_NUM_PREAMBLES));
  handle_rach(one_rach);

  for (unsigned nof_sched_grants = 0, slot_count = 0; nof_sched_grants < one_rach.occasions[0].preambles.size();
       ++slot_count) {
    ASSERT_TRUE(++slot_count < 20);
    run_slot();
    // RAR PDSCH allocated.
    ASSERT_EQ(scheduled_rars(0).size(), 1);
    unsigned nof_grants = 0;
    ASSERT_TRUE(rars_consistent_with_rach_indication(scheduled_rars(0), one_rach, nof_grants));
    ASSERT_EQ(nof_grants, scheduled_rars(0)[0].grants.size())
        << "All scheduled RAR grants must be for the provided occasion";

    // Msg3 allocated.
    ASSERT_EQ(scheduled_puschs(0).size(), nof_grants);

    // PDCCH allocated.
    ASSERT_EQ(scheduled_dl_pdcchs().size(), 1);

    nof_sched_grants += nof_grants;
  }
}

/// This test verifies the correct scheduling of a RAR and Msg3 in an FDD frame, when multiple RACH Preambles are
/// received, each in a different PRACH occasion.
/// The scheduler is expected to allocate several RARs (with different RA-RNTIs), each composed by one Msg3.
TEST_P(ra_scheduler_tester, schedules_multiple_rars_per_slot_when_multiple_prach_occasions)
{
  setup_sched(create_random_cell_config_request(duplex_mode::FDD, GetParam()));

  // Forward multiple RACH occasions with one preamble.
  unsigned                nof_occasions = get_random_uint(1, prach_constants::MAX_PRACH_OCCASIONS);
  rach_indication_message rach_ind      = create_rach_indication(0);
  for (unsigned i = 0; i != nof_occasions; ++i) {
    rach_ind.occasions.emplace_back();
    rach_ind.occasions.back().start_symbol    = 0;
    rach_ind.occasions.back().frequency_index = i;
    rach_ind.occasions.back().preambles.emplace_back(create_preamble());
  }
  handle_rach(rach_ind);

  unsigned nof_sched_grants = 0;
  for (unsigned slot_count = 0; nof_sched_grants < rach_ind.occasions[0].preambles.size(); ++slot_count) {
    ASSERT_TRUE(++slot_count < 20);
    run_slot();

    // RAR PDSCHs allocated.
    ASSERT_LE(scheduled_rars(0).size(), rach_ind.occasions.size());
    unsigned nof_grants = 0;
    ASSERT_TRUE(rars_consistent_with_rach_indication(scheduled_rars(0), rach_ind, nof_grants));
    ASSERT_EQ(nof_grants, rach_ind.occasions.size())
        << "The number of scheduled RARs must match the number of (single-preamble) RACH occasions";

    // Since each RAR contains one single Msg3 grant, the number of scheduled Msg3 PUSCHs must match the number of RARs.
    ASSERT_EQ(scheduled_puschs(0).size(), nof_grants) << "Number of Msg3 PUSCHs must match number of RARs";

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
TEST_P(ra_scheduler_tester, schedules_rar_in_valid_slots_when_tdd)
{
  // This k2 configuration ensures that can schedule the RAR+Msg3 in the TDD frame regardless of the PRACH slot.
  setup_sched(create_random_cell_config_request(duplex_mode::TDD, GetParam()));

  // Forward single RACH occasion with multiple preambles.
  // Note: The number of preambles is small enough to fit all grants in one slot.
  rach_indication_message rach_ind = create_rach_indication(get_random_uint(1, 10));
  handle_rach(rach_ind);

  for (unsigned nof_sched_grants = 0; nof_sched_grants < rach_ind.occasions[0].preambles.size();) {
    run_slot();

    if (not is_slot_valid_for_rar()) {
      ASSERT_TRUE(scheduled_dl_pdcchs().empty())
          << fmt::format("RAR PDCCH allocated in invalid slot {}", result_slot_tx());
      continue;
    }
    if (not is_in_rar_window(rach_ind.slot_rx)) {
      ASSERT_TRUE(scheduled_dl_pdcchs().empty()) << fmt::format("RAR PDCCH allocated after RAR window");
      break;
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

INSTANTIATE_TEST_SUITE_P(ra_scheduler,
                         ra_scheduler_tester,
                         ::testing::Combine(::testing::Range<uint8_t>(1, 4), // k0
                                            ::testing::Values(std::vector<uint8_t>{2},
                                                              std::vector<uint8_t>{2, 4}))); // k2

} // namespace

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
