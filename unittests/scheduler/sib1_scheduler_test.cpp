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

// Dummy PDCCH scheduler required to instantiate the SIB1 scheduler.
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

/// Helper that tests if the PDCCH and DCI grants in the scheduled results have been filled properly.
void assess_filled_grants(const cell_slot_resource_allocator& test_res_grid)
{
  // Test SIB_information message
  const sib_information& test_sib1 = test_res_grid.result.dl.bc.sibs.back();
  TESTASSERT_EQ(sib_information::si_indicator_type::sib1, test_sib1.si_indicator);

  // Test PDCCH_grant and DCI
  TESTASSERT(test_sib1.pdcch_cfg != nullptr);
  const pdcch_dl_information test_pdcch = *test_sib1.pdcch_cfg;

  const cell_configuration& cfg = test_res_grid.cfg;

  TESTASSERT_EQ(SI_RNTI, test_pdcch.ctx.rnti);
  TESTASSERT_EQ(dci_dl_format::f1_0, test_pdcch.dci.format_type);
  TESTASSERT_EQ(cfg.sib1_mcs, test_pdcch.dci.f1_0.mcs);
  TESTASSERT_EQ(cfg.sib1_rv, test_pdcch.dci.f1_0.rv);
}

/// Tests if PRBs have been set as used in the resource grid for the current slot.
void verify_prbs_allocation(const cell_slot_resource_allocator& test_res_grid, bool got_allocated = true)
{
  const cell_configuration& cfg = test_res_grid.cfg;
  // Tests if PRBs have been allocated.
  if (got_allocated) {
    TESTASSERT(test_res_grid.dl_res_grid.sch_crbs(cfg.dl_cfg_common.init_dl_bwp.generic_params).any());
  } else {
    // Tests if PRBs are still unused.
    TESTASSERT(not test_res_grid.dl_res_grid.sch_crbs(cfg.dl_cfg_common.init_dl_bwp.generic_params).any());
  }
}

/// \brief Tests if the SIB1 scheduler schedules the SIB1s at the right slot n0.
/// \param[in] sib1_n0_slots array of n0 slots; the n-th array's value is the n0 corresponding to the n-th SSB beam.
/// \param[in] pdcch_config_sib1 is the parameter (in the MIB) determining the n0 for each beam.
/// \param[in] ssb_beam_bitmap corresponds to the ssb-PositionsInBurs in the TS 38.311, with L_max = 8.
void test_sib1_scheduler(std::array<unsigned, MAX_NUM_BEAMS>& sib1_n0_slots,
                         uint8_t                              pdcch_config_sib1,
                         uint8_t                              ssb_beam_bitmap)
{
  // Instantiate the test_bench and the SIB1 scheduler.
  test_bench     t_bench{pdcch_config_sib1, ssb_beam_bitmap};
  sib1_scheduler sib1_sched{
      t_bench.cfg, t_bench.pdcch_sch, to_numerology_value(t_bench.cfg.dl_cfg_common.init_dl_bwp.generic_params.scs)};

  // Define helper lambda to determine from ssb_beam_bitmap if the n-th SSB beam is used.
  uint64_t ssb_bitmap          = t_bench.cfg.ssb_cfg.ssb_bitmap;
  auto     nth_ssb_beam_active = [ssb_bitmap](unsigned ssb_index) {
    return (ssb_bitmap & (static_cast<uint64_t>(0b1U) << static_cast<uint64_t>(63U - ssb_index))) > 0;
  };

  // Run the test for 10000 slots and
  size_t test_length_slots = 10000;
  for (size_t sl_idx = 0; sl_idx < test_length_slots; sl_idx++) {
    // Run SIB1 scheduler.
    sib1_sched.schedule_sib1(t_bench.get_slot_res_grid(), t_bench.sl_tx);

    auto& res_slot_grid = t_bench.get_slot_res_grid();

    // Verify if for any active beam, the SIB1 got allocated within the proper n0 slots.
    for (size_t ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ssb_idx++) {
      // Only check for the active slots.
      if (nth_ssb_beam_active(ssb_idx) && (sl_idx % SIB1_PERIODICITY == sib1_n0_slots[ssb_idx])) {
        // Verify that the scheduler results contain the SIB1 information.
        TESTASSERT_EQ(1, res_slot_grid.result.dl.bc.sibs.size());
        // Verify the PDCCH grants and DCI have been filled correctly.
        assess_filled_grants(res_slot_grid);
        // Verify the PRBs in the res_grid are set as used.
        verify_prbs_allocation(res_slot_grid);
      }
    }

    // Update SLOT.
    t_bench.slot_indication();
  }
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots{5, 7, 9, 11, 13, 15, 17, 19};
  test_sib1_scheduler(sib1_slots, 9U, 0b10101010);
  test_sib1_scheduler(sib1_slots, 9U, 0b01010101);
  test_sib1_scheduler(sib1_slots, 9U, 0b11111111);

  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots_1{2, 3, 4, 5, 6, 7, 8, 9};
  test_sib1_scheduler(sib1_slots_1, 2U, 0b10101010);
  test_sib1_scheduler(sib1_slots_1, 2U, 0b01010101);
  test_sib1_scheduler(sib1_slots_1, 2U, 0b11111111);

  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots_2{7, 8, 9, 10, 11, 12, 13, 14};
  test_sib1_scheduler(sib1_slots_2, 6U, 0b10101010);
  test_sib1_scheduler(sib1_slots_2, 6U, 0b01010101);
  test_sib1_scheduler(sib1_slots_2, 6U, 0b11111111);

  return 0;
}
