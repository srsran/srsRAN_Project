/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "lib/scheduler/common_scheduling/sib_scheduler.h"
#include "lib/scheduler/common_scheduling/ssb_scheduler.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/support/ssb_helpers.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/support/srsran_test.h"
#include <gtest/gtest.h>

using namespace srsran;

// Dummy PDCCH scheduler required to instantiate the SIB1 scheduler.
class dummy_pdcch_resource_allocator : public pdcch_resource_allocator
{
public:
  pdcch_dl_information* alloc_dl_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                              rnti_t                        rnti,
                                              search_space_id               ss_id,
                                              aggregation_level             aggr_lvl) override
  {
    TESTASSERT_EQ(ss_id, slot_alloc.cfg.dl_cfg_common.init_dl_bwp.pdcch_common.sib1_search_space_id);
    slot_alloc.result.dl.dl_pdcchs.emplace_back();
    slot_alloc.result.dl.dl_pdcchs.back().ctx.rnti    = rnti;
    slot_alloc.result.dl.dl_pdcchs.back().ctx.bwp_cfg = &slot_alloc.cfg.dl_cfg_common.init_dl_bwp.generic_params;
    slot_alloc.result.dl.dl_pdcchs.back().ctx.coreset_cfg =
        &*slot_alloc.cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0;
    slot_alloc.result.dl.dl_pdcchs.back().ctx.cces = {0, srsran::aggregation_level::n4};
    return &slot_alloc.result.dl.dl_pdcchs[0];
  }

  pdcch_dl_information* alloc_dl_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl) override
  {
    srsran_terminate("UE-dedicated PDCCHs should not be called while allocating RARs");
    return nullptr;
  }

  pdcch_ul_information* alloc_ul_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl) override
  {
    srsran_terminate("UE-dedicated PDCCHs should not be called while allocating RARs");
    return nullptr;
  }

  pdcch_ul_information* alloc_ul_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                              rnti_t                        rnti,
                                              search_space_id               ss_id,
                                              aggregation_level             aggr_lvl) override
  {
    srsran_terminate("Common PDCCHs should not be called while allocating RARs");
    return nullptr;
  }

  bool cancel_last_pdcch(cell_slot_resource_allocator& slot_alloc) override
  {
    srsran_terminate("Not supported");
    return true;
  }
};

/// Helper class to initialize and store relevant objects for the test and provide helper methods.
struct sib_test_bench {
  const bwp_id_t          bwp_id           = to_bwp_id(0);
  srslog::basic_logger&   sched_logger     = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger&   test_logger      = srslog::fetch_basic_logger("TEST");
  scheduler_result_logger sched_res_logger = scheduler_result_logger{true};

  const scheduler_expert_config            sched_cfg;
  const scheduler_si_expert_config&        si_cfg{sched_cfg.si};
  sched_cell_configuration_request_message cfg_msg;
  cell_configuration                       cfg{sched_cfg, cfg_msg};
  cell_resource_allocator                  res_grid{cfg};
  dummy_pdcch_resource_allocator           pdcch_sch;
  slot_point                               sl_tx;

  // Test bench ctor for SIB1 scheduler test use. It allows us to set single parameters.
  sib_test_bench(subcarrier_spacing   init_bwp_scs,
                 uint8_t              pdcch_config_sib1,
                 uint8_t              ssb_bitmap,
                 uint16_t             carrier_bw_mhz,
                 duplex_mode          duplx_mode,
                 sib1_rtx_periodicity sib1_rtx_period = sib1_rtx_periodicity::ms160,
                 ssb_periodicity      ssb_period      = ssb_periodicity::ms5) :
    sched_cfg(make_scheduler_expert_cfg({10, aggregation_level::n4, 10, aggregation_level::n4, sib1_rtx_period})),
    cfg_msg{make_cell_cfg_req_for_sib_sched(init_bwp_scs,
                                            pdcch_config_sib1,
                                            ssb_bitmap,
                                            ssb_period,
                                            carrier_bw_mhz,
                                            duplx_mode)},
    sl_tx{to_numerology_value(cfg.dl_cfg_common.init_dl_bwp.generic_params.scs), 0}
  {
    test_logger.set_context(0, 0);
    sched_logger.set_context(0, 0);
    res_grid.slot_indication(sl_tx);
  };

  // Test bench ctor for SIB1 scheduler test use in case of partial slot TDD configuration.
  sib_test_bench(sched_cell_configuration_request_message msg,
                 sib1_rtx_periodicity                     sib1_rtx_period = sib1_rtx_periodicity::ms160) :
    sched_cfg(make_scheduler_expert_cfg({10, aggregation_level::n4, 10, aggregation_level::n4, sib1_rtx_period})),
    cfg_msg{msg},
    sl_tx{to_numerology_value(cfg.dl_cfg_common.init_dl_bwp.generic_params.scs), 0}
  {
    test_logger.set_context(0, 0);
    sched_logger.set_context(0, 0);
    res_grid.slot_indication(sl_tx);
  };

  // Test bench ctor for SSB/SIB1 scheduler collision test.
  sib_test_bench(uint32_t           freq_arfcn,
                 uint16_t           offset_to_point_A,
                 uint8_t            k_ssb,
                 uint8_t            ssb_bitmap,
                 subcarrier_spacing init_bwp_scs,
                 uint8_t            pdcch_config_sib1,
                 uint16_t           carrier_bw_mhz) :
    sched_cfg(
        make_scheduler_expert_cfg({10, aggregation_level::n4, 10, aggregation_level::n4, sib1_rtx_periodicity::ms10})),
    cfg_msg{make_cell_cfg_req_for_sib_sched(freq_arfcn,
                                            offset_to_point_A,
                                            k_ssb,
                                            ssb_bitmap,
                                            init_bwp_scs,
                                            pdcch_config_sib1,
                                            carrier_bw_mhz)},
    sl_tx{to_numerology_value(init_bwp_scs), 0}
  {
    test_logger.set_context(0, 0);
    sched_logger.set_context(0, 0);
    res_grid.slot_indication(sl_tx);
  }

  // Delete the default constructor to force usage of defined constructors.
  sib_test_bench() = delete;

  cell_slot_resource_allocator& get_slot_res_grid() { return res_grid[0]; };

  static scheduler_expert_config make_scheduler_expert_cfg(const scheduler_si_expert_config& si_cfg)
  {
    scheduler_expert_config expert_cfg = config_helpers::make_default_scheduler_expert_config();
    expert_cfg.si                      = si_cfg;
    return expert_cfg;
  }

  // Create default configuration and change specific parameters based on input args.
  static sched_cell_configuration_request_message make_cell_cfg_req_for_sib_sched(subcarrier_spacing init_bwp_scs,
                                                                                  uint8_t            pdcch_config_sib1,
                                                                                  uint8_t            ssb_bitmap,
                                                                                  ssb_periodicity    ssb_period,
                                                                                  uint16_t           carrier_bw_mhz,
                                                                                  duplex_mode        duplx_mode)
  {
    cell_config_builder_params cell_cfg{};
    if (duplx_mode == srsran::duplex_mode::FDD) {
      cell_cfg.dl_arfcn = init_bwp_scs == subcarrier_spacing::kHz15 ? 536020 : 176300;
      cell_cfg.band     = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_arfcn);
    } else {
      // Random ARFCN that must be in FR1 and > 3GHz.
      cell_cfg.dl_arfcn = init_bwp_scs == subcarrier_spacing::kHz15 ? 286400 : 465000;
      cell_cfg.band     = init_bwp_scs == subcarrier_spacing::kHz15 ? nr_band::n50 : nr_band::n40;
    }
    cell_cfg.scs_common          = init_bwp_scs;
    cell_cfg.channel_bw_mhz      = static_cast<bs_channel_bandwidth_fr1>(carrier_bw_mhz);
    cell_cfg.coreset0_index      = (pdcch_config_sib1 >> 4U) & 0b00001111U;
    cell_cfg.search_space0_index = pdcch_config_sib1 & 0b00001111U;

    sched_cell_configuration_request_message msg =
        test_helpers::make_default_sched_cell_configuration_request(cell_cfg);

    msg.ssb_config.ssb_bitmap = static_cast<uint64_t>(ssb_bitmap) << static_cast<uint64_t>(56U);
    msg.ssb_config.ssb_period = ssb_period;

    if (duplx_mode == srsran::duplex_mode::TDD) {
      // Change TDD pattern so that PDCCH slots falls in DL slot when using 5Mhz carrier BW.
      msg.tdd_ul_dl_cfg_common.value().pattern1.dl_ul_tx_period_nof_slots = 20;
      msg.tdd_ul_dl_cfg_common.value().pattern1.nof_dl_slots              = 12;
      msg.tdd_ul_dl_cfg_common.value().pattern1.nof_ul_slots              = 7;
    }

    return msg;
  }

  // Create default configuration and change specific parameters based on input args.
  static sched_cell_configuration_request_message make_cell_cfg_req_for_sib_sched(uint32_t           freq_arfcn,
                                                                                  uint16_t           offset_to_point_A,
                                                                                  uint8_t            k_ssb,
                                                                                  uint8_t            ssb_bitmap,
                                                                                  subcarrier_spacing init_bwp_scs,
                                                                                  uint8_t            pdcch_config_sib1,
                                                                                  uint16_t           carrier_bw_mhz)
  {
    cell_config_builder_params cell_cfg{};
    cell_cfg.dl_arfcn       = freq_arfcn;
    cell_cfg.scs_common     = init_bwp_scs;
    cell_cfg.band           = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_arfcn);
    cell_cfg.channel_bw_mhz = static_cast<bs_channel_bandwidth_fr1>(carrier_bw_mhz);

    const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
        cell_cfg.channel_bw_mhz,
        cell_cfg.scs_common,
        cell_cfg.band.has_value() ? band_helper::get_freq_range(cell_cfg.band.value()) : frequency_range::FR1);

    optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
        band_helper::get_ssb_coreset0_freq_location(cell_cfg.dl_arfcn,
                                                    *cell_cfg.band,
                                                    nof_crbs,
                                                    cell_cfg.scs_common,
                                                    cell_cfg.scs_common,
                                                    cell_cfg.search_space0_index,
                                                    cell_cfg.max_coreset0_duration);
    srsran_assert(ssb_freq_loc.has_value(), "Invalid cell config parameters");
    cell_cfg.offset_to_point_a = ssb_freq_loc->offset_to_point_A;
    cell_cfg.k_ssb             = ssb_freq_loc->k_ssb;
    cell_cfg.coreset0_index    = ssb_freq_loc->coreset0_idx;

    sched_cell_configuration_request_message msg =
        test_helpers::make_default_sched_cell_configuration_request(cell_cfg);
    msg.dl_cfg_common.freq_info_dl.offset_to_point_a = offset_to_point_A;
    msg.ssb_config.ssb_bitmap                        = static_cast<uint64_t>(ssb_bitmap) << static_cast<uint64_t>(56U);
    msg.ssb_config.ssb_period                        = ssb_periodicity::ms10;
    msg.ssb_config.offset_to_point_A                 = ssb_offset_to_pointA{offset_to_point_A};
    msg.ssb_config.k_ssb                             = k_ssb;
    msg.dl_carrier.carrier_bw_mhz                    = carrier_bw_mhz;
    msg.coreset0                                     = (pdcch_config_sib1 >> 4U) & 0b00001111;
    msg.searchspace0                                 = pdcch_config_sib1 & 0b00001111;

    if (band_helper::get_duplex_mode(band_helper::get_band_from_dl_arfcn(freq_arfcn)) == srsran::duplex_mode::TDD) {
      // Change TDD pattern so that PDCCH slots falls in DL slot when using 5Mhz carrier BW.
      msg.tdd_ul_dl_cfg_common.value().pattern1.dl_ul_tx_period_nof_slots = 20;
      msg.tdd_ul_dl_cfg_common.value().pattern1.nof_dl_slots              = 12;
      msg.tdd_ul_dl_cfg_common.value().pattern1.nof_ul_slots              = 7;
    }

    return msg;
  }

  void slot_indication()
  {
    sl_tx++;
    sched_logger.set_context(sl_tx.sfn(), sl_tx.slot_index());
    test_logger.set_context(sl_tx.sfn(), sl_tx.slot_index());
    res_grid.slot_indication(sl_tx);
  }

  /// Helper that tests if the PDCCH and DCI grants in the scheduled results have been filled properly.
  void assess_filled_grants()
  {
    // Test SIB_information message
    const sib_information& test_sib1 = res_grid[0].result.dl.bc.sibs.back();
    TESTASSERT_EQ(sib_information::si_indicator_type::sib1, test_sib1.si_indicator);
    TESTASSERT_EQ(rnti_t::SI_RNTI, test_sib1.pdsch_cfg.rnti);

    // Test PDCCH_grant and DCI
    const pdcch_dl_information* pdcch =
        std::find_if(res_grid[0].result.dl.dl_pdcchs.begin(),
                     res_grid[0].result.dl.dl_pdcchs.end(),
                     [](const auto& pdcch_) { return pdcch_.ctx.rnti == rnti_t::SI_RNTI; });
    TESTASSERT(pdcch != nullptr);
    TESTASSERT_EQ(dci_dl_rnti_config_type::si_f1_0, pdcch->dci.type);
    TESTASSERT_EQ(si_cfg.sib1_mcs_index, pdcch->dci.si_f1_0.modulation_coding_scheme);
    TESTASSERT_EQ(0, pdcch->dci.si_f1_0.redundancy_version);
  }

  /// Tests if PRBs have been set as used in the resource grid for the current slot.
  void verify_prbs_allocation(bool got_allocated = true)
  {
    // Tests if PRBs have been allocated.
    if (got_allocated) {
      TESTASSERT(res_grid[0].dl_res_grid.used_crbs(cfg.dl_cfg_common.init_dl_bwp.generic_params, {0, 14}).any());
    } else {
      // Tests if PRBs are still unused.
      TESTASSERT(res_grid[0].dl_res_grid.used_crbs(cfg.dl_cfg_common.init_dl_bwp.generic_params, {0, 14}).none());
    }
  }
};

/// \brief Tests if the SIB1 scheduler schedules the SIB1s at the right slot n0.
/// \param[in] scs_common SCS corresponding to subCarrierSpacingCommon.
/// \param[in] sib1_pdcch_slots array of PDCCH slots; the n-th array's value is the PDCCH slot corresponding to the
/// n-th SSB beam.
/// \param[in] pdcch_config_sib1 is the parameter (in the MIB) determining the n0 for each beam.
/// \param[in] ssb_beam_bitmap corresponds to the ssb-PositionsInBurst in the TS 38.331, with L_max = 8.
/// \param[in] carrier_bw_mhz corresponds to the width of this carrier in MHz. Values: 5, 10, 15, 20, 25, 30, 40,
/// 50, 60, 70, 80, 90, 100, 200, 400.
/// \param[in] duplx_mode corresponds to duplex mode FDD or TDD.
void test_sib1_scheduler(subcarrier_spacing                         scs_common,
                         const std::array<unsigned, MAX_NUM_BEAMS>& sib1_pdcch_slots,
                         uint8_t                                    pdcch_config_sib1,
                         uint8_t                                    ssb_beam_bitmap,
                         uint16_t                                   carrier_bw_mhz,
                         duplex_mode                                duplx_mode)
{
  // Instantiate the sib_test_bench and the SIB1 scheduler.
  sib_test_bench t_bench{scs_common, pdcch_config_sib1, ssb_beam_bitmap, carrier_bw_mhz, duplx_mode};
  sib1_scheduler sib1_sched{t_bench.si_cfg, t_bench.cfg, t_bench.pdcch_sch, t_bench.cfg_msg};

  // SIB1 periodicity in slots.
  const unsigned sib1_period_slots = SIB1_PERIODICITY * t_bench.sl_tx.nof_slots_per_subframe();

  // Define helper lambda to determine from ssb_beam_bitmap if the n-th SSB beam is used.
  const uint64_t ssb_bitmap = t_bench.cfg.ssb_cfg.ssb_bitmap;

  // Run the test for 10000 slots.
  const size_t test_length_slots = 10000;
  for (size_t sl_idx = 0; sl_idx < test_length_slots; sl_idx++) {
    t_bench.sched_res_logger.on_slot_start();

    // Run SIB1 scheduler.
    sib1_sched.schedule_sib1(t_bench.get_slot_res_grid(), t_bench.sl_tx);

    auto& res_slot_grid = t_bench.get_slot_res_grid();

    test_scheduler_result_consistency(t_bench.cfg, res_slot_grid.slot, res_slot_grid.result);

    // Verify if for any active beam, the SIB1 got allocated within the proper n0 slots.
    for (size_t ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ssb_idx++) {
      // Only check for the active slots.
      if (is_nth_ssb_beam_active(ssb_bitmap, ssb_idx) && (sl_idx % sib1_period_slots == sib1_pdcch_slots[ssb_idx])) {
        // Verify that the scheduler results list contain 1 element with the SIB1 information.
        TESTASSERT_EQ(1, res_slot_grid.result.dl.bc.sibs.size());
        // Verify the PDCCH grants and DCI have been filled correctly.
        t_bench.assess_filled_grants();
        // Verify the PRBs in the res_grid are set as used.
        t_bench.verify_prbs_allocation();
      }
    }

    // Log results.
    t_bench.sched_res_logger.on_scheduler_result(t_bench.get_slot_res_grid().result);

    // Update SLOT.
    t_bench.slot_indication();
  }
}

/// \brief Tests if the SIB1 scheduler schedules SIB1s according to the correct retransmission periodicity.
///
/// This test evaluates the correct SIB1 retransmission period, which we assume it should be the maximum between the SSB
/// periodicity and the SIB1 retx periodicity set as a parameter. This is due to the fact that the SIB1 requires the SSB
/// to be decoded, meaning there is no point in scheduling SIBs more frequently than SSBs.
/// This test only evaluates the periodicity of SIB1, therefore it uses a standard set of values for the remaining
/// parameters (e.g., SCS, pdcch_config_sib1, SSB bitmap).
///
/// \param[in] sib1_rtx_period period set for the SIB1 retransmissions.
/// \param[in] ssb_period period set for the SSB.
void test_sib1_periodicity(sib1_rtx_periodicity sib1_rtx_period, ssb_periodicity ssb_period)
{
  // Instantiate the sib_test_bench and the SIB1 scheduler.
  sib_test_bench t_bench{
      subcarrier_spacing::kHz15, 9U, 0b10000000, 20, srsran::duplex_mode::FDD, sib1_rtx_period, ssb_period};
  sib1_scheduler sib1_sched{t_bench.si_cfg, t_bench.cfg, t_bench.pdcch_sch, t_bench.cfg_msg};

  // Determine the expected SIB1 retx periodicity.
  const unsigned expected_sib1_period_ms =
      sib1_rtx_periodicity_to_value(sib1_rtx_period) > ssb_periodicity_to_value(ssb_period)
          ? sib1_rtx_periodicity_to_value(sib1_rtx_period)
          : ssb_periodicity_to_value(ssb_period);

  // SIB1 periodicity in slots.
  const unsigned expected_sib1_period_slots = expected_sib1_period_ms * t_bench.sl_tx.nof_slots_per_subframe();

  // Slot (or offset) at which SIB1 PDCCH is allocated, measured as a delay compared to the slot with SSB. Specifically,
  // n0 = 5 is the offset of the SIB1 for the first beam, for searcSpaceZero = 9U, multiplexing pattern 1 (15kHz SCS,
  // FR1); as per Section 13, TS 38.213. As the scheduler allocate the SIB1 PDCCH in the n0 + 1 slot,
  // sib1_allocation_slot is 6.
  const unsigned sib1_allocation_slot{6};

  // Run the test for 10000 slots.
  const size_t test_length_slots = 10000;
  for (size_t sl_idx = 0; sl_idx < test_length_slots; sl_idx++) {
    t_bench.sched_res_logger.on_slot_start();

    // Run SIB1 scheduler.
    sib1_sched.schedule_sib1(t_bench.get_slot_res_grid(), t_bench.sl_tx);

    auto& res_slot_grid = t_bench.get_slot_res_grid();

    test_scheduler_result_consistency(t_bench.cfg, t_bench.sl_tx, res_slot_grid.result);

    // With the SSB bitmap set 0b10000000, only the SSB and SIB1 for the 1 beams are used; we perform the check for
    // this beam.
    if ((sl_idx % expected_sib1_period_slots) == sib1_allocation_slot) {
      // Verify that the scheduler results list contain 1 element with the SIB1 information.
      TESTASSERT_EQ(1, res_slot_grid.result.dl.bc.sibs.size());
    } else {
      TESTASSERT(res_slot_grid.result.dl.bc.sibs.empty());
    }

    // Log results.
    t_bench.sched_res_logger.on_scheduler_result(t_bench.get_slot_res_grid().result);

    // Update SLOT.
    t_bench.slot_indication();
  }
}

/// \brief Tests if the any potential collision occurs between SIB1 PDCCH/PDSCH and SSB.
///
/// \param[in] freq_arfcn ARFCN of point A for DL carrier.
/// \param[in] offset_to_point_A as per TS38.211, Section 4.4.4.2.
/// \param[in] k_ssb or ssb-SubcarrierOffset, as per TS38.211, Section 7.4.3.1.
/// \param[in] ssb_bitmap is \c ssb-PositionsInBurst.inOneGroup, as per TS38.331, \c ServingCellConfigCommonSIB.
/// \param[in] scs subcarrier spacing of SSB and SCScommon.
/// \param[in] pdcch_config_sib1 is \c pdcch-ConfigSIB1, as per TS38.213, Section 13.
/// \param[in] carrier_bw_mhz corresponds to the width of this carrier in MHz. Values: 5, 10, 15, 20, 25, 30, 40,
/// 50, 60, 70, 80, 90, 100, 200, 400.
void test_ssb_sib1_collision(uint32_t           freq_arfcn,
                             uint16_t           offset_to_point_A,
                             uint8_t            k_ssb,
                             uint8_t            ssb_bitmap,
                             subcarrier_spacing scs,
                             uint8_t            pdcch_config_sib1,
                             uint16_t           carrier_bw_mhz)
{
  // Instantiate the sib_test_bench and the SIB1 scheduler.
  sib_test_bench t_bench{freq_arfcn, offset_to_point_A, k_ssb, ssb_bitmap, scs, pdcch_config_sib1, carrier_bw_mhz};
  sib1_scheduler sib1_sched{t_bench.si_cfg, t_bench.cfg, t_bench.pdcch_sch, t_bench.cfg_msg};
  ssb_scheduler  ssb_sched{t_bench.cfg};

  // Run the test for 10000 slots.
  const size_t test_length_slots = 100;
  for (size_t sl_idx = 0; sl_idx < test_length_slots; sl_idx++) {
    t_bench.sched_res_logger.on_slot_start();

    // Clear the SSB list of it is not empty.
    auto& ssb_list = t_bench.get_slot_res_grid().result.dl.bc.ssb_info;
    if (not ssb_list.empty()) {
      ssb_list.clear();
    }

    // Run SSB scheduler.
    ssb_sched.schedule_ssb(t_bench.get_slot_res_grid());

    // Run SIB1 scheduler.
    sib1_sched.schedule_sib1(t_bench.get_slot_res_grid(), t_bench.sl_tx);

    auto& res_slot_grid = t_bench.get_slot_res_grid();

    test_scheduler_result_consistency(t_bench.cfg, t_bench.sl_tx, res_slot_grid.result);

    test_dl_resource_grid_collisions(t_bench.cfg, res_slot_grid.result.dl);

    // Log results.
    t_bench.sched_res_logger.on_scheduler_result(t_bench.get_slot_res_grid().result);

    // Update SLOT.
    t_bench.slot_indication();
  }
}

// Test for potential collisions between SIB1 PDCCH/PDSCH and SSB.
void test_sib_1_pdsch_collisions(unsigned freq_arfcn, subcarrier_spacing scs, uint16_t carrier_bw_mhz)
{
  const auto min_ch_bw = band_helper::get_min_channel_bw(band_helper::get_band_from_dl_arfcn(freq_arfcn), scs);
  srsran_assert(carrier_bw_mhz >= min_channel_bandwidth_to_MHz(min_ch_bw), "Invalid carrier BW");

  const auto nof_rbs_bpw =
      band_helper::get_n_rbs_from_bw(static_cast<bs_channel_bandwidth_fr1>(carrier_bw_mhz),
                                     scs,
                                     band_helper::get_freq_range(band_helper::get_band_from_dl_arfcn(freq_arfcn)));

  // NOTE: We only test 1 beam, as we don't have resource grids for multiple beams implemented yet.
  const uint8_t ssb_bitmap = 0b10000000;
  // Allocate SIB1 in the same slot as SSB - searchspace0 = 0U.
  const uint8_t searchspace0 = 0U;
  const uint8_t coreset0_max = scs == subcarrier_spacing::kHz15 ? 15 : 16;

  // Test different combinations of offsetToPointA and k_SSB.
  const unsigned max_offset_to_point_A = nof_rbs_bpw - NOF_SSB_PRBS;
  // Consider a +2 increment for both offsetToPointA and k_SSB, to be compliant with 30kHz SCS.
  for (unsigned offset_to_point_A = 0; offset_to_point_A < max_offset_to_point_A; offset_to_point_A += 2) {
    for (uint8_t k_ssb_val = 0; k_ssb_val < 12; k_ssb_val += 2) {
      // Test all possible combinations of coreset0 position.
      for (uint8_t coreset0 = 0; coreset0 < coreset0_max; ++coreset0) {
        const pdcch_type0_css_coreset_description coreset0_param =
            pdcch_type0_css_coreset_get(nr_band::n7, scs, scs, coreset0, k_ssb_val);

        // If the Coreset 0 exceeds the BPW limit, skip this configuration.
        TESTASSERT(coreset0_param.offset >= 0, "FR2 not supported in this test");

        // CRB (with reference to SCScommon carrier) pointed to by offset_to_point_A.
        const unsigned crb_ssb = scs == subcarrier_spacing::kHz15 ? offset_to_point_A : offset_to_point_A / 2;

        // If the Coreset 0 exceeds the Initial DL BPW limits, skip this configuration.
        if (crb_ssb - static_cast<unsigned>(coreset0_param.offset) +
                    static_cast<unsigned>(coreset0_param.nof_rb_coreset) >=
                nof_rbs_bpw or
            static_cast<unsigned>(coreset0_param.offset) > crb_ssb) {
          continue;
        }

        const uint8_t pdcch_config_sib1 = static_cast<uint8_t>((coreset0 << 4U) + searchspace0);
        test_ssb_sib1_collision(
            freq_arfcn, offset_to_point_A, k_ssb_val, ssb_bitmap, scs, pdcch_config_sib1, carrier_bw_mhz);
      }
    }
  }
}

TEST(sib1_scheduler_test, test_sib1_scheduler_allocation_fdd)
{
  // Test SIB1 scheduler for different values of searchSpaceZero (4 LSBs of pdcch_config_sib1) and for different
  // SSB_bitmaps.
  // Depending on the SIB1 scheduler implementation, the array sib1_slots contains the expected slots n0 or n0 + 1, at
  // which the SIB1 is scheduled.
  // NOTE: As the current SIB1 scheduler implementation allocates the SIB1 PDCCH on the slot n0 + 1, i-th element of the
  // sib1_slots array refers to the n0 + 1 for the i-th SSB's beam. The slots n0 have been pre-computed based on
  // TS 38.213, Section 13.

  // SCS Common: 15kHz
  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots = {6, 8, 10, 12, 14, 16, 18, 20};
  // pdcch_config_sib1 = 9U => { coreset0 = 0U, searchspace0 = 9U).
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 9U, 0b10101010, 20, srsran::duplex_mode::FDD);
  // pdcch_config_sib1 = 57U => { coreset0 = 3U, searchspace0 = 9U).
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 57U, 0b01010101, 20, srsran::duplex_mode::FDD);
  // pdcch_config_sib1 = 105U => { coreset0 = 0U, searchspace0 = 9U).
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 105U, 0b11111111, 20, srsran::duplex_mode::FDD);

  // 5Mhz Carrier BW.
  sib1_slots = {6, 8, 10, 12, 14, 16, 18, 20};
  // pdcch_config_sib1 = 9U => { coreset0 = 0U, searchspace0 = 9U).
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 9U, 0b10101010, 5, srsran::duplex_mode::FDD);
  // pdcch_config_sib1 = 57U => { coreset0 = 3U, searchspace0 = 9U).
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 57U, 0b01010101, 5, srsran::duplex_mode::FDD);

  sib1_slots = {3, 4, 5, 6, 7, 8, 9, 10};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b10101010, 20, srsran::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b01010101, 20, srsran::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b11111111, 20, srsran::duplex_mode::FDD);

  // 5Mhz Carrier BW.
  sib1_slots = {3, 4, 5, 6, 7, 8, 9, 10};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b10101010, 5, srsran::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b01010101, 5, srsran::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b11111111, 5, srsran::duplex_mode::FDD);

  sib1_slots = {8, 9, 10, 11, 12, 13, 14, 15};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b10101010, 20, srsran::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b01010101, 20, srsran::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b11111111, 20, srsran::duplex_mode::FDD);

  // 5Mhz Carrier BW.
  sib1_slots = {8, 9, 10, 11, 12, 13, 14, 15};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b10101010, 5, srsran::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b01010101, 5, srsran::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b11111111, 5, srsran::duplex_mode::FDD);

  // SCS Common: 30kHz
  sib1_slots = {11, 13, 15, 17, 19, 21, 23, 25};
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 9U, 0b10101010, 10, srsran::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 9U, 0b01010101, 10, srsran::duplex_mode::FDD);
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 9U, 0b11111111, 10, srsran::duplex_mode::FDD);

  sib1_slots = {11, 12, 13, 14, 15, 16, 17, 18};
  // pdcch_config_sib1 = 4U => { coreset0 = 0U, searchspace0 = 4U).
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 4U, 0b10101010, 10, srsran::duplex_mode::FDD);
  // pdcch_config_sib1 = 84U => { coreset0 = 5U, searchspace0 = 4U).
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 84U, 0b01010101, 10, srsran::duplex_mode::FDD);
}

TEST(sib1_scheduler_test, test_sib1_scheduler_allocation_tdd)
{
  // Test SIB1 scheduler for different values of searchSpaceZero (4 LSBs of pdcch_config_sib1), for
  // SSB_bitmap = 0b10000000.
  // Depending on the SIB1 scheduler implementation, the array sib1_slots contains the expected slots n0 or n0 + 1, at
  // which the SIB1 is scheduled.
  // NOTE: (i) As in this test we assume only the first SSB beam gets transmitted, sib1_slots only contains one element,
  // which is slot n0 + 1, as the current SIB1 scheduler implementation allocates the SIB1 PDCCH on the slot n0 + 1.
  // (ii) The slots n0 have been pre-computed based on TS 38.213, Section 13.

  // SCS Common: 15kHz
  std::array<unsigned, MAX_NUM_BEAMS> sib1_slots = {6};
  // pdcch_config_sib1 = 9U => { coreset0 = 0U, searchspace0 = 9U).
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 9U, 0b10000000, 20, srsran::duplex_mode::TDD);

  // 10Mhz Carrier BW.
  sib1_slots = {6};
  // pdcch_config_sib1 = 9U => { coreset0 = 0U, searchspace0 = 9U).
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 9U, 0b10000000, 10, srsran::duplex_mode::TDD);

  sib1_slots = {3};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b10000000, 20, srsran::duplex_mode::TDD);

  // 10Mhz Carrier BW.
  sib1_slots = {3};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 2U, 0b10000000, 10, srsran::duplex_mode::TDD);

  sib1_slots = {8};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b10000000, 20, srsran::duplex_mode::TDD);

  // 10Mhz Carrier BW.
  sib1_slots = {8};
  test_sib1_scheduler(subcarrier_spacing::kHz15, sib1_slots, 6U, 0b10000000, 10, srsran::duplex_mode::TDD);

  // SCS Common: 30kHz
  sib1_slots = {11};
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 9U, 0b10000000, 10, srsran::duplex_mode::TDD);

  sib1_slots = {11};
  // pdcch_config_sib1 = 4U => { coreset0 = 0U, searchspace0 = 4U).
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 4U, 0b10000000, 10, srsran::duplex_mode::TDD);

  sib1_slots = {5};
  test_sib1_scheduler(subcarrier_spacing::kHz30, sib1_slots, 2U, 0b10000000, 10, srsran::duplex_mode::TDD);
}

TEST(sib1_scheduler_test, test_sib1_periodicity)
{
  // Test the SIB1 scheduler periodicity for different combinations of SIB1 retx perdiod and SSB period values.
  // This test uses a standard set of values for SCS, searchSpaceSetZero and SSB bitmap.
  test_sib1_periodicity(sib1_rtx_periodicity::ms5, ssb_periodicity::ms40);
  test_sib1_periodicity(sib1_rtx_periodicity::ms80, ssb_periodicity::ms20);
  test_sib1_periodicity(sib1_rtx_periodicity::ms10, ssb_periodicity::ms10);
  test_sib1_periodicity(sib1_rtx_periodicity::ms20, ssb_periodicity::ms80);
  test_sib1_periodicity(sib1_rtx_periodicity::ms40, ssb_periodicity::ms10);
  test_sib1_periodicity(sib1_rtx_periodicity::ms40, ssb_periodicity::ms10);
  test_sib1_periodicity(sib1_rtx_periodicity::ms160, ssb_periodicity::ms80);
  test_sib1_periodicity(sib1_rtx_periodicity::ms80, ssb_periodicity::ms160);
}

TEST(sib1_scheduler_test, test_sib1_ssb_collision_for_15khz_scs)
{
  // TEST SIB1/SSB collision on the resource grid.
  // SCS 15kHz.
  const subcarrier_spacing scs = subcarrier_spacing::kHz15;
  // This can be any frequency such that the DL band has SSB SCS 15kHz (case A, in this case).
  const uint32_t freq_arfcn = 536020;

  test_sib_1_pdsch_collisions(freq_arfcn, scs, 20);
}

TEST(sib1_scheduler_test, test_sib1_ssb_collision_for_30khz_scs)
{
  // TEST SIB1/SSB collision on the resource grid.
  // SCS 30kHz.
  const subcarrier_spacing scs = subcarrier_spacing::kHz30;
  // This can be any frequency such that the DL band has SSB SCS 30kHz (case B, in this case).
  const uint32_t freq_arfcn = 176000;

  test_sib_1_pdsch_collisions(freq_arfcn, scs, 20);
}

/// Parameters used by partial slot TDD tests.
struct sib1_tdd_partial_slot_test_params {
  subcarrier_spacing      scs;
  tdd_ul_dl_config_common tdd_config;
  uint8_t                 pdcch_config_sib1;
  uint8_t                 ssb_beam_bitmap;
  uint16_t                carrier_bw_mhz;
  sib1_rtx_periodicity    sib1_rtx_period;
  ssb_periodicity         ssb_period;
};

class sib1_tdd_partial_slot_test : public ::testing::TestWithParam<sib1_tdd_partial_slot_test_params>
{
protected:
  sib1_tdd_partial_slot_test() : params(GetParam()) {}

  /// \brief Tests if the SIB1 scheduler schedules SIB1s in the partial slot in the TDD configuration.
  void test_sib1_scheduled_in_partial_slot(const tdd_ul_dl_config_common& tdd_cfg,
                                           subcarrier_spacing             init_bwp_scs,
                                           uint8_t                        pdcch_config_sib1,
                                           uint8_t                        ssb_bitmap,
                                           uint16_t                       carrier_bw_mhz,
                                           sib1_rtx_periodicity           sib1_rtx_period,
                                           ssb_periodicity                ssb_period)
  {
    sched_cell_configuration_request_message msg{sib_test_bench::make_cell_cfg_req_for_sib_sched(
        init_bwp_scs, pdcch_config_sib1, ssb_bitmap, ssb_period, carrier_bw_mhz, srsran::duplex_mode::TDD)};
    msg.tdd_ul_dl_cfg_common = tdd_cfg;
    // Generate PDSCH Time domain allocation based on the partial slot TDD configuration.
    msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list = config_helpers::make_pdsch_time_domain_resource(
        msg.searchspace0, msg.dl_cfg_common.init_dl_bwp.pdcch_common, nullopt, tdd_cfg);

    sib_test_bench t_bench{msg, sib1_rtx_period};

    sib1_scheduler sib1_sched{t_bench.si_cfg, t_bench.cfg, t_bench.pdcch_sch, t_bench.cfg_msg};

    // Determine the expected SIB1 retx periodicity.
    const unsigned expected_sib1_period_ms =
        sib1_rtx_periodicity_to_value(sib1_rtx_period) > ssb_periodicity_to_value(ssb_period)
            ? sib1_rtx_periodicity_to_value(sib1_rtx_period)
            : ssb_periodicity_to_value(ssb_period);

    // SIB1 periodicity in slots.
    const unsigned expected_sib1_period_slots = expected_sib1_period_ms * t_bench.sl_tx.nof_slots_per_subframe();

    // NOTE: The function assumes that arguments provided results in SIB1 scheduler allocating SIB1 in partial slot.
    const unsigned     sib1_allocation_slot_pattern1{tdd_cfg.pattern1.nof_dl_slots};
    optional<unsigned> sib1_allocation_slot_pattern2;
    if (tdd_cfg.pattern2.has_value()) {
      sib1_allocation_slot_pattern2 = tdd_cfg.pattern2->nof_dl_slots;
    }

    // Run the test for 10000 slots.
    const size_t test_length_slots = 10000;
    for (size_t sl_idx = 0; sl_idx < test_length_slots; sl_idx++) {
      t_bench.sched_res_logger.on_slot_start();

      // Run SIB1 scheduler.
      sib1_sched.schedule_sib1(t_bench.get_slot_res_grid(), t_bench.sl_tx);

      auto& res_slot_grid = t_bench.get_slot_res_grid();

      test_scheduler_result_consistency(t_bench.cfg, t_bench.sl_tx, res_slot_grid.result);

      // With the SSB bitmap set 0b10000000, only the SSB and SIB1 for the 1 beams are used; we perform the check for
      // this beam.
      if (((sl_idx % expected_sib1_period_slots) == sib1_allocation_slot_pattern1) or
          (sib1_allocation_slot_pattern2.has_value() and
           (sl_idx % expected_sib1_period_slots) == sib1_allocation_slot_pattern2.value())) {
        // Verify that the scheduler results list contain 1 element with the SIB1 information.
        TESTASSERT_EQ(1, res_slot_grid.result.dl.bc.sibs.size());
      } else {
        TESTASSERT(res_slot_grid.result.dl.bc.sibs.empty());
      }

      // Log results.
      t_bench.sched_res_logger.on_scheduler_result(t_bench.res_grid[0].result);

      // Update SLOT.
      t_bench.slot_indication();
    }
  }

  // Test parameters.
  sib1_tdd_partial_slot_test_params params;
};

TEST_P(sib1_tdd_partial_slot_test, successful_sib1_allocation_in_partial_slot)
{
  test_sib1_scheduled_in_partial_slot(params.tdd_config,
                                      params.scs,
                                      params.pdcch_config_sib1,
                                      params.ssb_beam_bitmap,
                                      params.carrier_bw_mhz,
                                      params.sib1_rtx_period,
                                      params.ssb_period);
}

INSTANTIATE_TEST_SUITE_P(sib1_scheduler_test,
                         sib1_tdd_partial_slot_test,
                         ::testing::Values(sib1_tdd_partial_slot_test_params{
                             .scs               = subcarrier_spacing::kHz30,
                             .tdd_config        = tdd_ul_dl_config_common{.ref_scs  = subcarrier_spacing::kHz30,
                                                                          .pattern1 = {.dl_ul_tx_period_nof_slots = 20,
                                                                                       .nof_dl_slots              = 11,
                                                                                       .nof_dl_symbols            = 8,
                                                                                       .nof_ul_slots              = 8,
                                                                                       .nof_ul_symbols            = 0}},
                             .pdcch_config_sib1 = 4U,
                             .ssb_beam_bitmap   = 0b10000000,
                             .carrier_bw_mhz    = 20,
                             .sib1_rtx_period   = sib1_rtx_periodicity::ms10,
                             .ssb_period        = ssb_periodicity::ms10}));

int main(int argc, char** argv)
{
  srslog::init();
  srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
