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

class ra_scheduler_tester : public ::testing::Test
{
protected:
  static constexpr unsigned tx_rx_delay = 4U;
  // We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  static constexpr unsigned max_k_value = cell_resource_allocator::MAXIMUM_SLOT_DIFF;

  ra_scheduler_tester() { set_random_slot(); }

  ~ra_scheduler_tester() override { srslog::flush(); }

  void
  setup_sched(const sched_cell_configuration_request_message& msg = make_default_sched_cell_configuration_request())
  {
    bench.emplace(msg);
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
    unsigned nof_msg3_matches = 0;
    ASSERT_TRUE(msg3_consistent_with_rars(scheduled_rars(0), scheduled_puschs(0), nof_msg3_matches))
        << "Msg3 PUSCHs parameters must match the content of the RAR grants";
    ASSERT_EQ(scheduled_puschs(0).size(), nof_msg3_matches) << "All scheduled PUSCHs must be Msg3 grants";

    TESTASSERT(bench->res_grid[0].result.dl.ue_grants.empty());
    TESTASSERT(bench->res_grid[0].result.dl.bc.sibs.empty());
  }

  void set_random_slot() { next_slot = {0, get_random_uint(0, 10240)}; }

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

  static sched_cell_configuration_request_message create_random_cell_config_request()
  {
    sched_cell_configuration_request_message msg = make_default_sched_cell_configuration_request();
    msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0      = get_random_uint(0, 2);
    msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].k2 = get_random_uint(2, 4);
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
      rnti_t ra_rnti = to_rnti(get_ra_rnti(rach_ind.slot_rx, occ.start_symbol, occ.frequency_index));
      for (const rar_information& rar : rars) {
        if (rar.pdsch_cfg.rnti == ra_rnti) {
          TESTASSERT(occ.preambles.size() >= rar.grants.size(),
                     "Cannot allocate more RARs than the number of detected preambles");
          for (const rar_ul_grant& grant : rar.grants) {
            auto it =
                std::find_if(occ.preambles.begin(),
                             occ.preambles.end(),
                             [crnti = grant.temp_crnti](const auto& preamble) { return preamble.tc_rnti == crnti; });
            if (it != occ.preambles.end()) {
              const rach_indication_message::preamble& preamble = *it;
              bool                                     preamble_matches =
                  preamble.preamble_id == grant.rapid and
                  preamble.time_advance.to_Ta(bench->cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs) == grant.ta;
              if (not preamble_matches) {
                return false;
              }
              nof_detected++;
            }
          }
        }
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

  slot_point            next_slot{0, 0};
  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("MAC");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("MAC");
  optional<test_bench>  bench;
};

TEST_F(ra_scheduler_tester, no_output_if_no_rachs)
{
  setup_sched(create_random_cell_config_request());
  run_slot();
  ASSERT_TRUE(no_rar_grants_scheduled());
}

TEST_F(ra_scheduler_tester, schedules_one_rar_per_slot_when_multi_preambles_with_same_prach_occasion)
{
  setup_sched(create_random_cell_config_request());

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

TEST_F(ra_scheduler_tester, schedules_multiple_rars_per_slot_when_multiple_prach_occasions)
{
  setup_sched(create_random_cell_config_request());

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

  for (unsigned nof_sched_grants = 0, slot_count = 0; nof_sched_grants < rach_ind.occasions[0].preambles.size();
       ++slot_count) {
    ASSERT_TRUE(++slot_count < 20);
    run_slot();

    // RAR PDSCHs allocated.
    ASSERT_LE(scheduled_rars(0).size(), rach_ind.occasions.size());
    unsigned nof_grants = 0;
    ASSERT_TRUE(rars_consistent_with_rach_indication(scheduled_rars(0), rach_ind, nof_grants));
    ASSERT_EQ(nof_grants, scheduled_rars(0).size())
        << "The number of scheduled RARs must match the number of (single-preamble) RACH occasions";

    ASSERT_EQ(scheduled_puschs(0).size(), nof_grants) << "Number of Msg3 PUSCHs must match number of RARs";

    // PDCCH allocated.
    ASSERT_EQ(scheduled_dl_pdcchs().size(), nof_grants) << "Number of PDCCHs must match number of RARs";

    nof_sched_grants += nof_grants;
  }
}

} // namespace

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}