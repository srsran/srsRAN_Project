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
#include "lib/scheduler/cell/sib_scheduler.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

enum class ssb_coreset0_mplex_pattern { mplx_pattern1 = 0, mplx_pattern2, mplx_pattern3, mplex_invalid };

class dummy_pdcch_scheduler : public pdcch_scheduler
{
public:
  pdcch_dl_information* alloc_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                           rnti_t                        rnti,
                                           search_space_id               ss_id,
                                           aggregation_level             aggr_lvl) override
  {
    TESTASSERT_EQ(ss_id, slot_alloc.cfg.dl_cfg_common.init_dl_bwp.pdcch_common.sib1_search_space_id);
    slot_alloc.result.dl.dl_pdcchs.emplace_back();
    slot_alloc.result.dl.dl_pdcchs.back().ctx.rnti = rnti;
    return &slot_alloc.result.dl.dl_pdcchs[0];
  }

  pdcch_dl_information* alloc_dl_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          bwp_id_t                      bwp_id,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl,
                                          dci_dl_format                 dci_fmt) override
  {
    srsran_terminate("UE-dedicated PDCCHs should not be called while allocating RARs");
    return nullptr;
  }

  pdcch_ul_information* alloc_ul_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          bwp_id_t                      bwp_id,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl,
                                          dci_ul_format                 dci_fmt) override
  {
    srsran_terminate("UE-dedicated PDCCHs should not be called while allocating RARs");
    return nullptr;
  }
};

//  ------   Helper functions   ------ .

// Helper function that returns whether slot n0 (where UE should monitor Type0-PDCCH CSS) is in an even/odd frame:
// - 0 if slot n0 is located in an even frame.
// - 1 if slot n0 (where UE should monitor Type0-PDCCH CSS) is located in an odd frame.
static unsigned sib1_is_even_frame(unsigned sib1_offset, double sib1_M, uint8_t numerology_mu, unsigned ssb_index)
{
  // This is only used to retrieve the nof_slots_per_frame.
  slot_point sl_point{numerology_mu, 0};

  // Compute floor( ( O * 2^mu + floor(i*M) ) / nof_slots_per_frame  ) mod 2, as per TS 38.213, Section 13.
  unsigned is_even = static_cast<unsigned>(floor(static_cast<double>(sib1_offset << numerology_mu) +
                                                 floor(static_cast<double>(ssb_index) * sib1_M)) /
                                           sl_point.nof_slots_per_frame()) %
                     2;
  return is_even;
}

// Helper function that returns slot n0 (where UE should monitor Type0-PDCCH CSS) for a given SSB (beam) index.
static slot_point get_sib1_n0(unsigned sib1_offset, double sib1_M, uint8_t numerology_mu, unsigned ssb_index)
{
  // Initialize n0 to a slot_point = 0.
  slot_point sib_1_n0{numerology_mu, 0};

  // Compute n0 = ( O * 2^mu + floor(i*M)  )  % nof_slots_per_frame, as per TS 38.213, Section 13.
  double tmp1 = static_cast<double>(sib1_offset << numerology_mu);
  double tmp2 = static_cast<double>(ssb_index) * sib1_M;
  sib_1_n0 += static_cast<unsigned>(tmp1 + floor(tmp2)) % sib_1_n0.nof_slots_per_frame();

  // We want to express n0 as a value from 0 to max_nof_slots. Since the mod operation above cap n0 to
  // (nof_slots_per_frame - 1), we need to add nof_slots_per_frame to n0 if this falls into an odd frame.
  sib_1_n0 += sib1_is_even_frame(sib1_offset, sib1_M, numerology_mu, ssb_index) * sib_1_n0.nof_slots_per_frame();

  return sib_1_n0;
}

#if 0
// Dummy function that returns O and M values given pdcch_config_sib1 from Table 13-11, TS 38.213.
// TODO: replace this with proper function from PHY team.
static void
get_sib1_offset_M(unsigned& offset, double& sib1_M, ssb_coreset0_mplex_pattern mplex_pattern, uint8_t pdcch_config_sib1)
{
  srsran_sanity_check(mplex_pattern == ssb_coreset0_mplex_pattern::mplx_pattern1,
                      "Only SSB/Coreset0 multiplexing pattern 1 is currently supported");
  switch (mplex_pattern) {
    case ssb_coreset0_mplex_pattern::mplx_pattern1:
      offset = 5;
      sib1_M = 2;
      break;
    default:
      // Only ssb_coreset0_mplex_pattern::mplx_pattern1 is currently supported.
      return;
  }
}
#endif

void test_sib1_is_even_frame()
{
  // unsigned sib1_is_even_frame(unsigned sib1_offset, double sib1_M, uint8_t numerology_mu, unsigned ssb_index)
  unsigned sib1_offset   = 5;
  double   sib1_M        = 0.5;
  uint8_t  numerology_mu = 0;

  unsigned test_results[] = {0, 0, 0, 0, 0, 0, 0, 0};
  TESTASSERT_EQ(test_results[0], sib1_is_even_frame(sib1_offset, sib1_M, numerology_mu, 0));
  TESTASSERT_EQ(test_results[1], sib1_is_even_frame(sib1_offset, sib1_M, numerology_mu, 1));
  TESTASSERT_EQ(test_results[2], sib1_is_even_frame(sib1_offset, sib1_M, numerology_mu, 2));
  TESTASSERT_EQ(test_results[3], sib1_is_even_frame(sib1_offset, sib1_M, numerology_mu, 3));
  TESTASSERT_EQ(test_results[4], sib1_is_even_frame(sib1_offset, sib1_M, numerology_mu, 4));
  TESTASSERT_EQ(test_results[5], sib1_is_even_frame(sib1_offset, sib1_M, numerology_mu, 5));
  TESTASSERT_EQ(test_results[6], sib1_is_even_frame(sib1_offset, sib1_M, numerology_mu, 6));
  TESTASSERT_EQ(test_results[7], sib1_is_even_frame(sib1_offset, sib1_M, numerology_mu, 7));
}

void test_get_sib1_n0()
{
  // unsigned sib1_is_even_frame(unsigned sib1_offset, double sib1_M, uint8_t numerology_mu, unsigned ssb_index)
  unsigned sib1_offset   = 7;
  double   sib1_M        = 0.5;
  uint8_t  numerology_mu = 0;

  unsigned test_results[] = {7, 7, 8, 8, 9, 9, 10, 10};
  TESTASSERT(slot_point(numerology_mu, test_results[0]) == get_sib1_n0(sib1_offset, sib1_M, numerology_mu, 0));
  TESTASSERT(slot_point(numerology_mu, test_results[1]) == get_sib1_n0(sib1_offset, sib1_M, numerology_mu, 1));
  TESTASSERT(slot_point(numerology_mu, test_results[2]) == get_sib1_n0(sib1_offset, sib1_M, numerology_mu, 2));
  TESTASSERT(slot_point(numerology_mu, test_results[3]) == get_sib1_n0(sib1_offset, sib1_M, numerology_mu, 3));
  TESTASSERT(slot_point(numerology_mu, test_results[4]) == get_sib1_n0(sib1_offset, sib1_M, numerology_mu, 4));
  TESTASSERT(slot_point(numerology_mu, test_results[5]) == get_sib1_n0(sib1_offset, sib1_M, numerology_mu, 5));
  TESTASSERT(slot_point(numerology_mu, test_results[6]) == get_sib1_n0(sib1_offset, sib1_M, numerology_mu, 6));
  TESTASSERT(slot_point(numerology_mu, test_results[7]) == get_sib1_n0(sib1_offset, sib1_M, numerology_mu, 7));
}

/// Helper class to initialize and store relevant objects for the test and provide helper methods.
struct test_bench {
  const bwp_id_t        bwp_id      = to_bwp_id(0);
  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("MAC");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  cell_configuration      cfg;
  cell_resource_allocator res_grid;
  dummy_pdcch_scheduler   pdcch_sch;
  slot_point              sl_tx{0, 0};

  test_bench(uint8_t pdcch_config_sib1, uint8_t ssb_bitmap) :
    cfg{make_cell_cfg_req_for_sib_sched(pdcch_config_sib1, ssb_bitmap)},
    res_grid{cfg}
  {
    res_grid.slot_indication(sl_tx);
  };

  cell_slot_resource_allocator& get_slot_res_grid() { return res_grid[0]; };

  sched_cell_configuration_request_message make_cell_cfg_req_for_sib_sched(uint8_t pdcch_config_sib1,
                                                                           uint8_t ssb_bitmap)
  {
    sched_cell_configuration_request_message msg =
        make_scheduler_cell_configuration_request(test_helpers::make_default_mac_cell_creation_request());
    msg.pdcch_config_sib1     = pdcch_config_sib1;
    msg.ssb_config.ssb_bitmap = static_cast<uint64_t>(ssb_bitmap) << static_cast<uint64_t>(56U);
    return msg;
  }

  void slot_indication()
  {
    sl_tx++;
    mac_logger.set_context(sl_tx.to_uint());
    test_logger.set_context(sl_tx.to_uint());
    test_logger.info("Starting new slot {}", sl_tx);
    res_grid.slot_indication(sl_tx);
  }
};

void assess_filled_grants(const cell_slot_resource_allocator& test_res_grid)
{
  // Test SIB_information message
  const sib_information& test_sib1 = test_res_grid.result.dl.bc.sibs.back();
  TESTASSERT_EQ(sib_information::si_indicator_type::sib1, test_sib1.si_indicator);

  // Test PDCCH_grant and DCI
  // TESTASSERT_NEQ(nullptr, test_sib1.pdcch_cfg);
  const pdcch_dl_information test_pdcch = *test_sib1.pdcch_cfg;

  const cell_configuration& cfg = test_res_grid.cfg;

  TESTASSERT_EQ(SI_RNTI, test_pdcch.ctx.rnti);
  TESTASSERT_EQ(dci_dl_format::f1_0, test_pdcch.dci.format_type);
  TESTASSERT_EQ(cfg.sib1_mcs, test_pdcch.dci.f1_0.mcs);
  TESTASSERT_EQ(cfg.sib1_rv, test_pdcch.dci.f1_0.rv);
}

void verify_prbs_allocation(const cell_slot_resource_allocator& test_res_grid, bool got_allocated = true)
{
  const cell_configuration& cfg = test_res_grid.cfg;
  if (got_allocated) {
    TESTASSERT(test_res_grid.dl_res_grid.sch_crbs(cfg.dl_cfg_common.init_dl_bwp.generic_params).any());
  } else {
    TESTASSERT(not test_res_grid.dl_res_grid.sch_crbs(cfg.dl_cfg_common.init_dl_bwp.generic_params).any());
  }
}

void test_sib1_scheduler()
{
  test_bench t_bench{9U, 0b10101010};
  size_t     test_length_slots = 10000;

  sib1_scheduler sib1_sched{
      t_bench.cfg, t_bench.pdcch_sch, to_numerology_value(t_bench.cfg.dl_cfg_common.init_dl_bwp.generic_params.scs)};

  // unsigned sib1_slots[] = {7, 7, 8, 8, 9, 9, 10, 10};

  /*
  auto nth_ssb_beam_active = [](uint64_t ssb_bitmap, unsigned ssb_index) {
    return (ssb_bitmap & (static_cast<uint64_t>(0b1U) << static_cast<uint64_t>(63U - ssb_index))) > 0;
  };
   */

  for (size_t sl_idx = 0; sl_idx < test_length_slots; sl_idx++) {
    // Run SIB1 scheduler.
    sib1_sched.schedule_sib1(t_bench.get_slot_res_grid(), t_bench.sl_tx);

    auto& res_slot_grid = t_bench.get_slot_res_grid();
    // Test scheduled results.
    if (sl_idx % SIB1_PERIODICITY == 5) {
      TESTASSERT(res_slot_grid.result.dl.bc.sibs.size() == 1);
      assess_filled_grants(res_slot_grid);
      verify_prbs_allocation(res_slot_grid);
    } else if (sl_idx % SIB1_PERIODICITY == 9) {
      TESTASSERT(res_slot_grid.result.dl.bc.sibs.size() == 1);
      assess_filled_grants(res_slot_grid);
      verify_prbs_allocation(res_slot_grid);
    } else if (sl_idx % SIB1_PERIODICITY == 13) {
      TESTASSERT(res_slot_grid.result.dl.bc.sibs.size() == 1);
      assess_filled_grants(res_slot_grid);
      verify_prbs_allocation(res_slot_grid);
    } else if (sl_idx % SIB1_PERIODICITY == 17) {
      TESTASSERT(res_slot_grid.result.dl.bc.sibs.size() == 1);
      assess_filled_grants(res_slot_grid);
      verify_prbs_allocation(res_slot_grid);
    } else {
      TESTASSERT(res_slot_grid.result.dl.bc.sibs.size() == 0);
      verify_prbs_allocation(res_slot_grid, false);
    }

    // Update SLOT.
    t_bench.slot_indication();
    if (sl_idx == 4) {
      printf("Stop here");
    }
  }
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  test_sib1_is_even_frame();
  test_get_sib1_n0();
  test_sib1_scheduler();

  return 0;
}
