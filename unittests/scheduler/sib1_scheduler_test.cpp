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

  sched_cell_configuration_request_message cfg_msg;
  cell_configuration                       cfg;
  cell_resource_allocator                  res_grid;
  dummy_pdcch_scheduler                    pdcch_sch;
  slot_point                               sl_tx;

  test_bench(subcarrier_spacing init_bwp_scs, uint8_t pdcch_config_sib1, uint8_t ssb_bitmap) :
    cfg_msg{make_cell_cfg_req_for_sib_sched(init_bwp_scs, pdcch_config_sib1, ssb_bitmap)},
    cfg{cfg_msg},
    res_grid{cfg},
    sl_tx{to_numerology_value(cfg.dl_cfg_common.init_dl_bwp.generic_params.scs), 0}
  {
    res_grid.slot_indication(sl_tx);
  };

  cell_slot_resource_allocator& get_slot_res_grid() { return res_grid[0]; };

  // Create default configuration and change specific parameters based on input args.
  sched_cell_configuration_request_message
  make_cell_cfg_req_for_sib_sched(subcarrier_spacing init_bwp_scs, uint8_t pdcch_config_sib1, uint8_t ssb_bitmap)
  {
    sched_cell_configuration_request_message msg =
        make_scheduler_cell_configuration_request(test_helpers::make_default_mac_cell_creation_request());
    msg.dl_cfg_common.init_dl_bwp.generic_params.scs = init_bwp_scs;
    msg.ssb_config.scs                               = init_bwp_scs;
    // Change Carrier parameters when SCS is 30kHz.
    if (init_bwp_scs == subcarrier_spacing::kHz30) {
      msg.dl_cfg_common.freq_info_dl.scs_carrier_list.emplace_back(
          scs_specific_carrier{0, subcarrier_spacing::kHz30, 52});
      // Random ARFCN that must be in FR1 and > 3GHz.
      msg.dl_carrier.arfcn          = 700000;
      msg.dl_carrier.carrier_bw_mhz = 20;
      msg.dl_carrier.nof_ant        = 1;
    }
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

  /// Helper that tests if the PDCCH and DCI grants in the scheduled results have been filled properly.
  void assess_filled_grants()
  {
    // Test SIB_information message
    const sib_information& test_sib1 = res_grid[0].result.dl.bc.sibs.back();
    TESTASSERT_EQ(sib_information::si_indicator_type::sib1, test_sib1.si_indicator);

    // Test PDCCH_grant and DCI
    TESTASSERT(test_sib1.pdcch_cfg != nullptr);
    const pdcch_dl_information test_pdcch = *test_sib1.pdcch_cfg;

    TESTASSERT_EQ(SI_RNTI, test_pdcch.ctx.rnti);
    TESTASSERT_EQ(dci_dl_rnti_config_type::si_f1_0, test_pdcch.dci.type);
    TESTASSERT_EQ(cfg_msg.sib1_mcs, test_pdcch.dci.si_f1_0.modulation_coding_scheme);
    TESTASSERT_EQ(cfg_msg.sib1_rv, test_pdcch.dci.si_f1_0.redundancy_version);
  }

  /// Tests if PRBs have been set as used in the resource grid for the current slot.
  void verify_prbs_allocation(bool got_allocated = true)
  {
    // Tests if PRBs have been allocated.
    if (got_allocated) {
      TESTASSERT(res_grid[0].dl_res_grid.sch_crbs(cfg.dl_cfg_common.init_dl_bwp.generic_params).any());
    } else {
      // Tests if PRBs are still unused.
      TESTASSERT(not res_grid[0].dl_res_grid.sch_crbs(cfg.dl_cfg_common.init_dl_bwp.generic_params).any());
    }
  }
};

/// \brief Tests if the SIB1 scheduler schedules the SIB1s at the right slot n0.
/// \param[in] scs_common SCS corresponding to subCarrierSpacingCommon.
/// \param[in] sib1_n0_slots array of n0 slots; the n-th array's value is the n0 corresponding to the n-th SSB beam.
/// \param[in] pdcch_config_sib1 is the parameter (in the MIB) determining the n0 for each beam.
/// \param[in] ssb_beam_bitmap corresponds to the ssb-PositionsInBurs in the TS 38.311, with L_max = 8.
void test_sib1_scheduler(subcarrier_spacing                   scs_common,
                         std::array<unsigned, MAX_NUM_BEAMS>& sib1_n0_slots,
                         uint8_t                              pdcch_config_sib1,
                         uint8_t                              ssb_beam_bitmap)
{
  // Instantiate the test_bench and the SIB1 scheduler.
  test_bench     t_bench{scs_common, pdcch_config_sib1, ssb_beam_bitmap};
  sib1_scheduler sib1_sched{t_bench.cfg,
                            t_bench.pdcch_sch,
                            t_bench.cfg_msg.pdcch_config_sib1,
                            t_bench.cfg_msg.sib1_mcs,
                            t_bench.cfg_msg.sib1_rv,
                            t_bench.cfg_msg.sib1_dci_aggr_lev,
                            t_bench.cfg_msg.sib1_retx_periodicity,
                            t_bench.cfg.dl_cfg_common.init_dl_bwp.generic_params.scs};

  // SIB1 periodicity in slots.
  unsigned sib1_period_slots = SIB1_PERIODICITY * t_bench.sl_tx.nof_slots_per_subframe();

  // Define helper lambda to determine from ssb_beam_bitmap if the n-th SSB beam is used.
  uint64_t ssb_bitmap          = t_bench.cfg.ssb_cfg.ssb_bitmap;
  auto     nth_ssb_beam_active = [ssb_bitmap](unsigned ssb_index) {
    return (ssb_bitmap & (static_cast<uint64_t>(0b1U) << static_cast<uint64_t>(63U - ssb_index))) > 0;
  };

  // Run the test for 10000 slots.
  size_t test_length_slots = 10000;
  for (size_t sl_idx = 0; sl_idx < test_length_slots; sl_idx++) {
    // Run SIB1 scheduler.
    sib1_sched.schedule_sib1(t_bench.get_slot_res_grid(), t_bench.sl_tx);

    auto& res_slot_grid = t_bench.get_slot_res_grid();

    // Verify if for any active beam, the SIB1 got allocated within the proper n0 slots.
    for (size_t ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ssb_idx++) {
      // Only check for the active slots.
      if (nth_ssb_beam_active(ssb_idx) && (sl_idx % sib1_period_slots == sib1_n0_slots[ssb_idx])) {
        // Verify that the scheduler results list contain 1 element with the SIB1 information.
        TESTASSERT_EQ(1, res_slot_grid.result.dl.bc.sibs.size());
        // Verify the PDCCH grants and DCI have been filled correctly.
        t_bench.assess_filled_grants();
        // Verify the PRBs in the res_grid are set as used.
        t_bench.verify_prbs_allocation();
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

  // SCS Common: 15kHz
  // Test SIB1 scheduler for different values of searchSpaceZero (4 LSBs of pdcch_config_sib1) and for different
  // SSB_bitmaps.
  // The array sib1_slots contains the expected slots n0, at which the SIB1 is scheduled. The i-th element of the array
  // refers to the n0 for the i-th SSB's beam. The slots n0 have been pre-computed based on TS 38.213, Section 13.
  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots{5, 7, 9, 11, 13, 15, 17, 19};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 9U, 0b10101010);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 9U, 0b01010101);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 9U, 0b11111111);

  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots_1{2, 3, 4, 5, 6, 7, 8, 9};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots_1, 2U, 0b10101010);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots_1, 2U, 0b01010101);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots_1, 2U, 0b11111111);

  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots_2{7, 8, 9, 10, 11, 12, 13, 14};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots_2, 6U, 0b10101010);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots_2, 6U, 0b01010101);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots_2, 6U, 0b11111111);

  // SCS Common: 30kHz
  // Test SIB1 scheduler for different values of searchSpaceZero (4 LSBs of pdcch_config_sib1) and for different
  // SSB_bitmaps.
  // The array sib1_slots contains the expected slots n0, at which the SIB1 is scheduled. The i-th element of the array
  // refers to the n0 for the i-th SSB's beam. The slots n0 have been pre-computed based on TS 38.213, Section 13.
  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots_3{10, 12, 14, 16, 18, 20, 22, 24};
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots_3, 9U, 0b10101010);
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots_3, 9U, 0b01010101);
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots_3, 9U, 0b11111111);

  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots_4{10, 11, 12, 13, 14, 15, 16, 17};
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots_4, 4U, 0b10101010);
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots_4, 4U, 0b01010101);
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots_4, 4U, 0b11111111);

  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots_5{4, 5, 6, 7, 8, 9, 10, 11};
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots_5, 12U, 0b10101010);
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots_5, 12U, 0b01010101);
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots_5, 12U, 0b11111111);

  return 0;
}
