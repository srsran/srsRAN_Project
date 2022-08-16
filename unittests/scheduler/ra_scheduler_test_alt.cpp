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

    bench->ra_sch.run_slot(bench->res_grid);
    check_result_consistency();

    next_slot++;
  }

  void set_random_slot() { next_slot = {0, get_random_uint(0, 10240)}; }

  rach_indication_message::preamble create_preamble(rnti_t rnti)
  {
    rach_indication_message::preamble preamble{};
    preamble.preamble_id  = get_random_uint(0, 63);
    preamble.time_advance = phy_time_unit::from_seconds(std::uniform_real_distribution<double>{0, 2005e-6}(g));
    preamble.tc_rnti      = rnti;
    return preamble;
  }

  rach_indication_message create_rach_indication(unsigned nof_preambles)
  {
    rach_indication_message rach_ind{};
    rach_ind.cell_index = to_du_cell_index(0);
    rach_ind.slot_rx    = next_slot_rx();
    rach_ind.occasions.emplace_back();
    rach_ind.occasions.back().start_symbol    = 0;
    rach_ind.occasions.back().frequency_index = 0;

    unsigned next_rnti = get_random_uint(MIN_CRNTI, MAX_CRNTI);
    unsigned rnti_inc  = get_random_uint(1, 10);
    for (unsigned i = 0; i != nof_preambles; ++i) {
      rach_ind.occasions.back().preambles.emplace_back(create_preamble(to_rnti(next_rnti)));
      next_rnti += rnti_inc;
    }
    return rach_ind;
  }

  slot_point next_slot_rx() const { return next_slot - tx_rx_delay; }

  span<const pdcch_dl_information> scheduled_dl_pdcchs() { return bench->res_grid[0].result.dl.dl_pdcchs; }

  span<const rar_information> scheduled_rars(uint8_t time_resource)
  {
    unsigned k0 = bench->cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[time_resource].k0;
    return bench->res_grid[k0].result.dl.rar_grants;
  }

  void check_result_consistency()
  {
    // Account k0,k1,k2>0 by checking also consistency of future slots.
    for (unsigned i = 0; i != max_k_value; ++i) {
      test_scheduler_result_consistency(bench->cell_cfg, bench->res_grid[i].result);
    }
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

  unsigned nof_grants_assigned_to_occasion(span<const rar_information>              rars,
                                           slot_point                               slot_rx,
                                           const rach_indication_message::occasion& occasion)
  {
    rnti_t ra_rnti = to_rnti(get_ra_rnti(slot_rx, occasion.start_symbol, occasion.frequency_index));
    for (const rar_information& rar : rars) {
      if (rar.pdsch_cfg.rnti == ra_rnti) {
        return rar.grants.size();
      }
    }
    return 0;
  }

  bool grants_match_occasion_preambles(span<const rar_information>              rars,
                                       slot_point                               slot_rx,
                                       const rach_indication_message::occasion& occasion)
  {
    rnti_t ra_rnti = to_rnti(get_ra_rnti(slot_rx, occasion.start_symbol, occasion.frequency_index));
    for (const rar_information& rar : rars) {
      if (rar.pdsch_cfg.rnti == ra_rnti) {
        for (const rar_ul_grant& grant : rar.grants) {
          auto it =
              std::find_if(occasion.preambles.begin(),
                           occasion.preambles.end(),
                           [crnti = grant.temp_crnti](const auto& preamble) { return preamble.tc_rnti == crnti; });
          if (it != occasion.preambles.end()) {
            const rach_indication_message::preamble& preamble = *it;
            if (not(preamble.tc_rnti == grant.temp_crnti and preamble.preamble_id == grant.rapid)) {
              // TODO: Check TA as well.
              return false;
            }
          }
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
  setup_sched();
  run_slot();
  ASSERT_TRUE(no_rar_grants_scheduled());
}

TEST_F(ra_scheduler_tester, schedules_one_rar_per_slot_for_multi_preambles_with_same_rach_occ)
{
  sched_cell_configuration_request_message msg = make_default_sched_cell_configuration_request();
  msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0 = get_random_uint(0, 2);
  setup_sched(msg);
  rach_indication_message one_rach = create_rach_indication(get_random_uint(1, prach_constants::MAX_NUM_PREAMBLES));
  handle_rach(one_rach);

  unsigned nof_sched_grants = 0, slot_count = 0;
  while (nof_sched_grants < one_rach.occasions[0].preambles.size()) {
    run_slot();
    TESTASSERT_EQ(scheduled_rars(0).size(), 1);
    ASSERT_TRUE(grants_match_occasion_preambles(scheduled_rars(0), one_rach.slot_rx, one_rach.occasions[0]));
    ASSERT_LE(scheduled_dl_pdcchs().size(), 1);
    if (not scheduled_dl_pdcchs().empty()) {
      test_pdcch_pdsch_common_consistency(bench->cell_cfg, scheduled_dl_pdcchs()[0], scheduled_rars(0)[0].pdsch_cfg);
    }
    unsigned n = nof_grants_assigned_to_occasion(scheduled_rars(0), one_rach.slot_rx, one_rach.occasions[0]);
    ASSERT_EQ(n, scheduled_rars(0)[0].grants.size()) << "All scheduled RARs must be for the provided occasion";
    nof_sched_grants += n;
    ASSERT_TRUE(++slot_count < 20);
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