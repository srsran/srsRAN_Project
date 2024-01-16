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

#include "lib/scheduler/cell/resource_grid.h"
#include "lib/scheduler/common_scheduling/ssb_scheduler.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "srsran/ran/frame_types.h"
#include "srsran/ran/ssb_mapping.h"
#include "srsran/scheduler/sched_consts.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

/// This will be removed once we can get this value from the slot object.
#define TEST_HARQ_ASSERT_MSG(SLOT, PERIODICITY, CASE)                                                                  \
  "Failed at slot: '{}', periodicity '{}', case '{}'", SLOT, PERIODICITY, ssb_case_to_str(CASE)

using namespace srsran;

/// Helper function to print the SSB CASE.
const char* ssb_case_to_str(ssb_pattern_case ssb_case)
{
  switch (ssb_case) {
    case ssb_pattern_case::A:
      return "SSB case A";
    case srsran::ssb_pattern_case::B:
      return "SSB case B";
    case srsran::ssb_pattern_case::C:
      return "SSB case C - paired spectrum";
    default:
      return "SSB case invalid";
  }
}

/// Helper struct to test HARQs and update loggers slot context.
struct ssb_test_bench {
  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  ssb_test_bench(ssb_periodicity    ssb_period,
                 uint32_t           freq_arfcn,
                 uint16_t           offset_to_point_A,
                 uint64_t           in_burst_bitmap,
                 subcarrier_spacing ssb_scs,
                 uint8_t            k_ssb) :
    cfg(sched_cfg,
        make_cell_cfg_req_for_sib_sched(ssb_period, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb)),
    cell_res_grid(cfg)
  {
    // Set numerology for slot_point depending on the SSB case.
    uint8_t numerology = 0;
    switch (cfg.ssb_case) {
      case ssb_pattern_case::A:
        numerology = 0;
        break;
      case ssb_pattern_case::B:
      case ssb_pattern_case::C:
        numerology = 1;
        break;
      default:
        srsran_assert(cfg.ssb_case < ssb_pattern_case::invalid,
                      "Invalid SSB case. Only case A, B, and C are supported");
    }
    test_logger.set_context(0, 0);
    mac_logger.set_context(0, 0);
    t = slot_point(numerology, 0);
    cell_res_grid.slot_indication(t);
  }

  void new_slot()
  {
    t++;
    cell_res_grid.slot_indication(t);
    test_logger.set_context(t.sfn(), t.slot_index());
    mac_logger.set_context(t.sfn(), t.slot_index());
  }

  cell_slot_resource_allocator& get_slot_allocator() { return cell_res_grid; }
  const cell_configuration&     get_cell_sched_config() { return cfg; };

  slot_point slot_tx() { return t; }

private:
  slot_point                    t;
  const scheduler_expert_config sched_cfg = config_helpers::make_default_scheduler_expert_config();
  const cell_configuration      cfg;
  cell_slot_resource_allocator  cell_res_grid;

  // Create default configuration and change specific parameters based on input args.
  sched_cell_configuration_request_message make_cell_cfg_req_for_sib_sched(ssb_periodicity    ssb_period,
                                                                           uint32_t           freq_arfcn,
                                                                           uint16_t           offset_to_point_A,
                                                                           uint64_t           in_burst_bitmap,
                                                                           subcarrier_spacing init_bwp_scs,
                                                                           uint8_t            k_ssb)
  {
    sched_cell_configuration_request_message msg     = test_helpers::make_default_sched_cell_configuration_request();
    msg.dl_carrier.arfcn                             = freq_arfcn;
    msg.dl_carrier.band                              = band_helper::get_band_from_dl_arfcn(freq_arfcn);
    msg.dl_cfg_common.freq_info_dl.offset_to_point_a = offset_to_point_A;
    msg.dl_cfg_common.init_dl_bwp.generic_params.scs = init_bwp_scs;
    msg.ssb_config.scs                               = init_bwp_scs;
    msg.scs_common                                   = init_bwp_scs;
    msg.ssb_config.ssb_bitmap                        = in_burst_bitmap;
    msg.ssb_config.ssb_period                        = ssb_period;
    msg.ssb_config.offset_to_point_A                 = ssb_offset_to_pointA{offset_to_point_A};
    msg.ssb_config.k_ssb                             = k_ssb;
    // Change Carrier parameters when SCS is 15kHz.
    if (init_bwp_scs == subcarrier_spacing::kHz15) {
      msg.dl_cfg_common.freq_info_dl.scs_carrier_list.front().carrier_bandwidth = 106;
      msg.dl_cfg_common.init_dl_bwp.generic_params.crbs =
          crb_interval{0, msg.dl_cfg_common.freq_info_dl.scs_carrier_list.front().carrier_bandwidth};
    }
    // Change Carrier parameters when SCS is 30kHz.
    else if (init_bwp_scs == subcarrier_spacing::kHz30) {
      msg.dl_cfg_common.freq_info_dl.scs_carrier_list.emplace_back(
          scs_specific_carrier{0, subcarrier_spacing::kHz30, 52});
      msg.dl_cfg_common.init_dl_bwp.generic_params.crbs = {
          0, msg.dl_cfg_common.freq_info_dl.scs_carrier_list[1].carrier_bandwidth};
    }
    msg.dl_carrier.carrier_bw_mhz = 20;
    msg.dl_carrier.nof_ant        = 1;

    return msg;
  }
};

// Helper function to test the SSB symbols and CRBs are correctly registered in the resource grid.
void test_ssb_grid_allocation(const cell_slot_resource_grid& res_grid,
                              subcarrier_spacing             scs,
                              ssb_offset_to_pointA           offset_pA,
                              unsigned                       nof_CRBs,
                              ssb_subcarrier_offset          k_ssb,
                              const ofdm_symbol_range&       ssb_symbols,
                              const crb_interval&            ssb_crbs)
{
  unsigned ssb_crb_start = scs == subcarrier_spacing::kHz15 ? offset_pA.to_uint() : offset_pA.to_uint() / 2;
  unsigned ssb_crb_stop  = k_ssb.to_uint() > 0 ? ssb_crb_start + NOF_SSB_PRBS + 1 : ssb_crb_start + NOF_SSB_PRBS;

  // Verify resources on the left-side of SSB (lower CRBs) are unused.
  grant_info empty_space{scs, {0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP}, {0, ssb_crb_start}};
  TESTASSERT(not res_grid.collides(empty_space),
             "PRBs {} over symbols {} should be empty",
             empty_space.crbs,
             empty_space.symbols);

  // Verify resources on the left-side of SSB (lower CRBs) are unused.
  TESTASSERT(ssb_crb_stop <= nof_CRBs, "SSB falls outside the BWP");
  empty_space.crbs = {ssb_crb_stop, nof_CRBs};
  TESTASSERT(not res_grid.collides(empty_space),
             "PRBs {} over symbols {} should be empty",
             empty_space.crbs,
             empty_space.symbols);

  // Verify resources for SSB in the grid are set.
  grant_info ssb_resources{scs, ssb_symbols, ssb_crbs};
  TESTASSERT(res_grid.all_set(ssb_resources),
             "PRBs {} over symbols {} should be set",
             ssb_resources.crbs,
             ssb_resources.symbols);
}

// Helper function to test the symbols and CRBs in the SSB information struct.
void test_ssb_information(uint8_t                expected_sym,
                          uint32_t               expected_slot_shift,
                          crb_interval           expected_crbs,
                          const ssb_information& ssb_info)
{
  // Check OFDM symbols and frequency allocation in the ssb_information struct.
  // con
  unsigned slot_symbol_start =
      (expected_sym + expected_slot_shift * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP) % NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
  TESTASSERT_EQ(slot_symbol_start, ssb_info.symbols.start(), "Start symbol check failed in SSB information");
  unsigned slot_symbol_stop =
      (expected_sym + expected_slot_shift * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP + 4) % NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
  TESTASSERT_EQ(slot_symbol_stop, ssb_info.symbols.stop(), "Stop symbol check failed in SSB information");
  TESTASSERT_EQ(expected_crbs.start(), ssb_info.crbs.start(), "Start CRB check failed in SSB information");
  TESTASSERT_EQ(expected_crbs.stop(), ssb_info.crbs.stop(), "Stop CRB check failed in SSB information");
}

/// This function tests SSB case A and C (both paired and unpaired spectrum).
void test_ssb_case_A_C(const slot_point&             slot_tx,
                       uint32_t                      freq_cutoff,
                       const cell_configuration&     cell_cfg,
                       cell_slot_resource_allocator& slot_alloc)
{
  const ssb_configuration& ssb_cfg = cell_cfg.ssb_cfg;

  // Use ssb_bitmap with 8 bits, to simplify the code (L_max 64 is not used at the moment).
  uint8_t in_burst_bitmap = static_cast<uint8_t>(ssb_cfg.ssb_bitmap >> 56U);

  // For frequencies lower than the cutoff, there should only be at most 4 SSB opportunities (4 left-most bits in
  // in_burst_bitmap).
  if (cell_cfg.dl_carrier.arfcn <= freq_cutoff) {
    TESTASSERT((cell_cfg.ssb_cfg.ssb_bitmap & 0b00001111) == 0,
               TEST_HARQ_ASSERT_MSG(slot_tx.to_uint(), ssb_cfg.ssb_period, cell_cfg.ssb_case));
  }

  uint32_t sl_point_mod =
      slot_tx.to_uint() % (ssb_periodicity_to_value(ssb_cfg.ssb_period) * slot_tx.nof_slots_per_subframe());

  // Get the size of the SSB list from the in_burst_bitmap.
  size_t ssb_list_size = 0;
  if (sl_point_mod == 0) {
    ssb_list_size = ((in_burst_bitmap & 0b10000000) >> 7U) + ((in_burst_bitmap & 0b01000000) >> 6U);
  } else if (sl_point_mod == 1) {
    ssb_list_size = ((in_burst_bitmap & 0b00100000) >> 5U) + ((in_burst_bitmap & 0b00010000) >> 4U);
  } else if (sl_point_mod == 2) {
    ssb_list_size = ((in_burst_bitmap & 0b00001000) >> 3U) + ((in_burst_bitmap & 0b0000100) >> 2U);
  } else if (sl_point_mod == 3) {
    ssb_list_size = ((in_burst_bitmap & 0b00000010) >> 1U) + (in_burst_bitmap & 0b00000001);
  }

  const ssb_information_list& ssb_list = slot_alloc.result.dl.bc.ssb_info;
  // Check the SSB list size.
  TESTASSERT_EQ(
      ssb_list_size, ssb_list.size(), TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));

  unsigned expected_ssb_per_slot;
  if (sl_point_mod == 0 or sl_point_mod == 1 or sl_point_mod == 2 or sl_point_mod == 3) {
    expected_ssb_per_slot               = 0;
    std::array<uint8_t, 2> ofdm_symbols = {2, 8};
    uint8_t                ssb_idx_mask = 0b10000000 >> (sl_point_mod * 2);
    auto                   it           = ssb_list.begin();

    // Check if, for each OFDM symbols in the bitmap, there is a corresponding SSB from the list.
    for (uint8_t n = 0; n < ofdm_symbols.size(); n++) {
      if ((in_burst_bitmap & ssb_idx_mask) > 0) {
        auto ssb_item = *it;
        ++expected_ssb_per_slot;

        unsigned ssb_crb_start = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs == subcarrier_spacing::kHz15
                                     ? ssb_cfg.offset_to_point_A.to_uint()
                                     : ssb_cfg.offset_to_point_A.to_uint() / 2;
        unsigned ssb_crb_stop =
            ssb_cfg.k_ssb.to_uint() > 0 ? ssb_crb_start + NOF_SSB_PRBS + 1 : ssb_crb_start + NOF_SSB_PRBS;
        test_ssb_information(ofdm_symbols[n], sl_point_mod, crb_interval{ssb_crb_start, ssb_crb_stop}, ssb_item);

        // Verify there is no collision in the SSB
        test_ssb_grid_allocation(slot_alloc.dl_res_grid,
                                 ssb_cfg.scs,
                                 ssb_cfg.offset_to_point_A,
                                 cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list[0].carrier_bandwidth,
                                 ssb_cfg.k_ssb,
                                 ssb_item.symbols,
                                 ssb_item.crbs);

        it++;
      }

      ssb_idx_mask = ssb_idx_mask >> 1;
    }

    // Verify that the grid of symbols/PRBs is empty in the slots when SSBs are not expected.
    if (expected_ssb_per_slot == 0) {
      grant_info empty_space{slot_alloc.cfg.dl_cfg_common.init_dl_bwp.generic_params.scs,
                             {0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP},
                             {0, slot_alloc.cfg.dl_cfg_common.freq_info_dl.scs_carrier_list[0].carrier_bandwidth}};
      TESTASSERT(not slot_alloc.dl_res_grid.collides(empty_space), "PRBs {} should be empty", empty_space.crbs);
    }
  }
}

/// This function tests SSB case B.
void test_ssb_case_B(const slot_point&             slot_tx,
                     const cell_configuration&     cell_cfg,
                     cell_slot_resource_allocator& slot_alloc)
{
  const ssb_configuration& ssb_cfg = cell_cfg.ssb_cfg;

  // Use ssb_bitmap with 8 bits, to simplify the code (L_max 64 is not used at the moment).
  uint8_t in_burst_bitmap = static_cast<uint8_t>(ssb_cfg.ssb_bitmap >> 56U);

  // For frequencies lower than the cutoff, there should only be at most 4 SSB opportunities (4 left-most bits in
  // in_burst_bitmap).
  if (cell_cfg.dl_carrier.arfcn <= CUTOFF_FREQ_ARFCN_CASE_A_B_C) {
    TESTASSERT((in_burst_bitmap & 0b00001111) == 0,
               TEST_HARQ_ASSERT_MSG(slot_tx.to_uint(), ssb_cfg.ssb_period, cell_cfg.ssb_case));
  }

  uint32_t sl_point_mod =
      slot_tx.to_uint() % (ssb_periodicity_to_value(ssb_cfg.ssb_period) * slot_tx.nof_slots_per_subframe());

  // Get the size of the SSB list from the in_burst_bitmap.
  size_t ssb_list_size = 0;
  if (sl_point_mod == 0) {
    ssb_list_size = ((in_burst_bitmap & 0b10000000) >> 7U) + ((in_burst_bitmap & 0b01000000) >> 6U);
  } else if (sl_point_mod == 1) {
    ssb_list_size = ((in_burst_bitmap & 0b00100000) >> 5U) + ((in_burst_bitmap & 0b00010000) >> 4U);
  } else if (sl_point_mod == 2) {
    ssb_list_size = ((in_burst_bitmap & 0b00001000) >> 3U) + ((in_burst_bitmap & 0b0000100) >> 2U);
  } else if (sl_point_mod == 3) {
    ssb_list_size = ((in_burst_bitmap & 0b00000010) >> 1U) + (in_burst_bitmap & 0b00000001);
  }

  const ssb_information_list& ssb_list = slot_alloc.result.dl.bc.ssb_info;
  // Check the SSB list size
  TESTASSERT_EQ(
      ssb_list_size, ssb_list.size(), TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));

  unsigned expected_ssb_per_slot;
  // This block targets SSB occasions at ssb_burst_ofdm_symbols = {4, 8, 32, 36}.
  if (sl_point_mod == 0 or sl_point_mod == 2) {
    expected_ssb_per_slot = 0;
    // For frequency less than the CUTOFF freq, list cannot have more than 4 elements.
    std::array<uint8_t, 2> ssb_burst_ofdm_symb = {4, 8};
    uint8_t                ssb_idx_mask        = 0b10000000 >> (sl_point_mod * 2);
    auto                   it                  = ssb_list.begin();

    // Check if, for each OFDM symbols in the bitmap, there is a corresponding SSB from the list.
    for (uint8_t n = 0; n < ssb_burst_ofdm_symb.size(); n++) {
      if ((in_burst_bitmap & ssb_idx_mask) > 0) {
        auto ssb_item = *it;
        ++expected_ssb_per_slot;

        // Check OFDM symbols and frequency allocation in the ssb_information struct.
        unsigned ssb_crb_start = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs == subcarrier_spacing::kHz15
                                     ? ssb_cfg.offset_to_point_A.to_uint()
                                     : ssb_cfg.offset_to_point_A.to_uint() / 2;
        unsigned ssb_crb_stop =
            ssb_cfg.k_ssb.to_uint() > 0 ? ssb_crb_start + NOF_SSB_PRBS + 1 : ssb_crb_start + NOF_SSB_PRBS;
        test_ssb_information(ssb_burst_ofdm_symb[n], sl_point_mod, crb_interval{ssb_crb_start, ssb_crb_stop}, ssb_item);

        // Verify there is no collision in the SSB
        test_ssb_grid_allocation(slot_alloc.dl_res_grid,
                                 ssb_cfg.scs,
                                 ssb_cfg.offset_to_point_A,
                                 cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list[0].carrier_bandwidth,
                                 ssb_cfg.k_ssb,
                                 ssb_item.symbols,
                                 ssb_item.crbs);

        it++;
      }

      ssb_idx_mask = ssb_idx_mask >> 1;
    }

    // Verify that the grid of symbols/PRBs is empty in the slots when SSBs are not expected.
    if (expected_ssb_per_slot == 0) {
      grant_info empty_space{slot_alloc.cfg.dl_cfg_common.init_dl_bwp.generic_params.scs,
                             {0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP},
                             {0, slot_alloc.cfg.dl_cfg_common.freq_info_dl.scs_carrier_list[0].carrier_bandwidth}};
      TESTASSERT(not slot_alloc.dl_res_grid.collides(empty_space), "PRBs {} should be empty", empty_space.crbs);
    }
  }

  // This block targets SSB occasions at ssb_burst_ofdm_symbols = {16, 20, 44, 48}.
  if (sl_point_mod == 1 or sl_point_mod == 3) {
    expected_ssb_per_slot = 0;
    // For frequency less than the CUTOFF freq, list cannot have more than 4 elements
    std::array<uint8_t, 2> ssb_burst_ofdm_symb = {16, 20};
    uint8_t                ssb_idx_mask        = 0b00100000 >> ((sl_point_mod - 1) * 2);
    auto                   it                  = ssb_list.begin();

    // Check if, for each OFDM symbols in the bitmap, there is a corresponding SSB from the list.
    for (uint8_t n = 0; n < ssb_burst_ofdm_symb.size(); n++) {
      if ((in_burst_bitmap & ssb_idx_mask) > 0) {
        auto ssb_item = *it;
        ++expected_ssb_per_slot;

        // Check OFDM symbols and frequency allocation in the ssb_information struct.
        unsigned ssb_crb_start = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs == subcarrier_spacing::kHz15
                                     ? ssb_cfg.offset_to_point_A.to_uint()
                                     : ssb_cfg.offset_to_point_A.to_uint() / 2;
        unsigned ssb_crb_stop =
            ssb_cfg.k_ssb.to_uint() > 0 ? ssb_crb_start + NOF_SSB_PRBS + 1 : ssb_crb_start + NOF_SSB_PRBS;
        test_ssb_information(
            ssb_burst_ofdm_symb[n], sl_point_mod - 1, crb_interval{ssb_crb_start, ssb_crb_stop}, ssb_item);

        // Verify there is no collision in the SSB
        test_ssb_grid_allocation(slot_alloc.dl_res_grid,
                                 ssb_cfg.scs,
                                 ssb_cfg.offset_to_point_A,
                                 cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list[0].carrier_bandwidth,
                                 ssb_cfg.k_ssb,
                                 ssb_item.symbols,
                                 ssb_item.crbs);

        it++;
      }

      ssb_idx_mask = ssb_idx_mask >> 1;
    }

    // Verify that the grid of symbols/PRBs is empty in the slots when SSBs are not expected.
    if (expected_ssb_per_slot == 0) {
      grant_info empty_space{slot_alloc.cfg.dl_cfg_common.init_dl_bwp.generic_params.scs,
                             {0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP},
                             {0, slot_alloc.cfg.dl_cfg_common.freq_info_dl.scs_carrier_list[0].carrier_bandwidth}};
      TESTASSERT(not slot_alloc.dl_res_grid.collides(empty_space), "PRBs {} should be empty", empty_space.crbs);
    }
  }
}

void test_ssb_allocation(ssb_periodicity    ssb_period,
                         uint32_t           freq_arfcn,
                         uint16_t           offset_to_point_A,
                         uint64_t           in_burst_bitmap,
                         subcarrier_spacing ssb_scs,
                         uint8_t            k_ssb)
{
  const size_t NUM_OF_TEST_SLOTS = 1000;

  ssb_test_bench bench{ssb_period, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb};
  ssb_scheduler  ssb_sch{bench.get_cell_sched_config()};

  bench.new_slot();

  // in_burst_bitmap is not used by the function for the time being.

  // Run test for a given number of slots.
  for (size_t slot_count = 0; slot_count < NUM_OF_TEST_SLOTS; slot_count++, bench.new_slot()) {
    // Clear the SSB list of it is not empty.
    auto& ssb_list = bench.get_slot_allocator().result.dl.bc.ssb_info;
    if (ssb_list.size() > 0) {
      ssb_list.clear();
    }

    // Schedule the SSB.
    ssb_sch.schedule_ssb(bench.get_slot_allocator());

    // Select SSB case with reference to TS 38.213, Section 4.1.
    ssb_pattern_case ssb_case = bench.get_cell_sched_config().ssb_case;
    switch (ssb_case) {
      case ssb_pattern_case::A:
        test_ssb_case_A_C(
            bench.slot_tx(), CUTOFF_FREQ_ARFCN_CASE_A_B_C, bench.get_cell_sched_config(), bench.get_slot_allocator());
        break;
      case ssb_pattern_case::B:
        test_ssb_case_B(bench.slot_tx(), bench.get_cell_sched_config(), bench.get_slot_allocator());
        break;
      case ssb_pattern_case::C: {
        uint32_t f_cutoff_arfnc = bench.get_cell_sched_config().paired_spectrum ? CUTOFF_FREQ_ARFCN_CASE_A_B_C
                                                                                : CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED;
        test_ssb_case_A_C(bench.slot_tx(), f_cutoff_arfnc, bench.get_cell_sched_config(), bench.get_slot_allocator());
      } break;
      default:
        srsran_assert(ssb_case < ssb_pattern_case::invalid, "Only SSB case A, B and C are currently supported");
    }
  }
}

// This tests the SSB scheduling for different SSB periods and bursts.
TEST(ssb_scheduler_test, test_time_domain_ssb_scheduling)
{
  // ##########################################################
  //                   TEST CASE A
  // ##########################################################
  // TEST Case A, frequency < 3GHz.
  uint32_t freq_arfcn = 536020;
  //
  uint64_t           in_burst_bitmap = static_cast<uint64_t>(0b01100000U) << static_cast<uint64_t>(56U);
  subcarrier_spacing ssb_scs         = subcarrier_spacing::kHz15;

  // TEST Different periodicity with Case A.
  ssb_periodicity periodicity       = ssb_periodicity::ms5;
  const uint8_t   k_ssb             = 0;
  const uint16_t  offset_to_point_A = 14;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity = ssb_periodicity::ms10;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity = ssb_periodicity::ms20;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity = ssb_periodicity::ms40;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity = ssb_periodicity::ms80;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  // TEST Case A, frequency < 3GHz, different inBurst bitmaps.
  periodicity     = ssb_periodicity::ms5;
  in_burst_bitmap = static_cast<uint64_t>(0b10000000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity     = ssb_periodicity::ms5;
  in_burst_bitmap = static_cast<uint64_t>(0b11010000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity     = ssb_periodicity::ms5;
  in_burst_bitmap = static_cast<uint64_t>(0b11110000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity     = ssb_periodicity::ms5;
  in_burst_bitmap = static_cast<uint64_t>(0b01110000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity     = ssb_periodicity::ms5;
  in_burst_bitmap = static_cast<uint64_t>(0b00010000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  // NOTE: We do not test CASE A for frequency > 3GHz. Although TS 38.213, Section 4.1 defines SSB case A for
  // frequency > 3GHz, there are no current NR bands available that fall into this category.

  // ##########################################################
  //                   TEST CASE B
  // ##########################################################
  // TEST Case B, frequency < 3GHz.
  ssb_scs         = subcarrier_spacing::kHz30;
  freq_arfcn      = 176000;
  in_burst_bitmap = static_cast<uint64_t>(0b01100000U) << static_cast<uint64_t>(56U);

  // TEST Different periodicity with Case B
  periodicity = ssb_periodicity::ms5;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity = ssb_periodicity::ms10;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity = ssb_periodicity::ms20;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity = ssb_periodicity::ms40;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity = ssb_periodicity::ms80;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  // TEST Case B, frequency < 3GHz, different inBurst bitmaps.
  periodicity     = ssb_periodicity::ms5;
  in_burst_bitmap = static_cast<uint64_t>(0b10000000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity     = ssb_periodicity::ms10;
  in_burst_bitmap = static_cast<uint64_t>(0b11010000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity     = ssb_periodicity::ms5;
  in_burst_bitmap = static_cast<uint64_t>(0b11110000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity     = ssb_periodicity::ms5;
  in_burst_bitmap = static_cast<uint64_t>(0b01110000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity     = ssb_periodicity::ms5;
  in_burst_bitmap = static_cast<uint64_t>(0b00010000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  // NOTE: We do not test CASE B for frequency > 3GHz. Although TS 38.213, Section 4.1 defines SSB case B for
  // frequency > 3GHz, there are no current NR bands available that fall into this category.

  // ##########################################################
  //                   TEST CASE C
  // ##########################################################
  // TEST CASE C - unpaired spectrum, freq. < 1.88GHz.
  ssb_scs         = subcarrier_spacing::kHz30;
  in_burst_bitmap = static_cast<uint64_t>(0b01100000U) << static_cast<uint64_t>(56U);
  // This corresponds to band 50, which is the only case C band below 1.88GHz.
  freq_arfcn  = 292000;
  periodicity = ssb_periodicity::ms5;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity = ssb_periodicity::ms10;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity = ssb_periodicity::ms20;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity = ssb_periodicity::ms40;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  periodicity = ssb_periodicity::ms80;
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  // TEST CASE C - unpaired spectrum, freq. < 1.88GHz, different inBurst bitmaps.
  periodicity     = ssb_periodicity::ms5;
  in_burst_bitmap = static_cast<uint64_t>(0b10000000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  in_burst_bitmap = static_cast<uint64_t>(0b11010000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  in_burst_bitmap = static_cast<uint64_t>(0b11110000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  in_burst_bitmap = static_cast<uint64_t>(0b01110000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  in_burst_bitmap = static_cast<uint64_t>(0b00010000U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  // TEST CASE C - unpaired spectrum, freq. > 1.88GHz, different inBurst bitmaps.
  // This corresponds to band 50, which is the only case C band below 1.88GHz.
  freq_arfcn      = 518000;
  in_burst_bitmap = static_cast<uint64_t>(0b01101001U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  in_burst_bitmap = static_cast<uint64_t>(0b11111111U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  in_burst_bitmap = static_cast<uint64_t>(0b01111101U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);

  in_burst_bitmap = static_cast<uint64_t>(0b10011110U) << static_cast<uint64_t>(56U);
  test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb);
}

// This tests the SSB scheduling for different offsetToPointA and k_SSB.
TEST(ssb_scheduler_test, test_freq_domain_ssb_scheduling)
{
  // ##########################################################
  //                   TEST CASE A
  // ##########################################################
  // TEST Case A, frequency < 3GHz.
  uint32_t           freq_arfcn      = 536020;
  uint64_t           in_burst_bitmap = static_cast<uint64_t>(0b11110000U) << static_cast<uint64_t>(56U);
  subcarrier_spacing ssb_scs         = subcarrier_spacing::kHz15;
  ssb_periodicity    periodicity     = ssb_periodicity::ms10;

  // Test different offset_to_point_A and different k_SSB.
  periodicity = ssb_periodicity::ms10;
  // For case A (20MHz, SCS 15kHz), there are 106 within the Initial DL BWP.
  unsigned max_offset_to_point_A = 106 - NOF_SSB_PRBS;
  for (uint16_t offset_to_point_A = 0; offset_to_point_A < max_offset_to_point_A; offset_to_point_A++) {
    for (uint8_t k_ssb_val = 0; k_ssb_val < 12; k_ssb_val++) {
      test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb_val);
    }
  }

  // ##########################################################
  //                   TEST CASE B
  // ##########################################################
  // TEST Case B, frequency < 3GHz.
  freq_arfcn      = 176000;
  ssb_scs         = subcarrier_spacing::kHz30;
  in_burst_bitmap = static_cast<uint64_t>(0b11110000U) << static_cast<uint64_t>(56U);

  // Test different offset_to_point_A and different k_SSB.
  periodicity = ssb_periodicity::ms10;
  // For case B (20MHz, SCS 30kHz), there are 52 within the Initial DL BWP.
  max_offset_to_point_A = 52 - NOF_SSB_PRBS;
  for (uint16_t offset_to_point_A = 0; offset_to_point_A < 28; offset_to_point_A += 2) {
    for (uint8_t k_ssb_val = 0; k_ssb_val < 24; k_ssb_val++) {
      test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb_val);
    }
  }

  // ##########################################################
  //                   TEST CASE C
  // ##########################################################
  // TEST Case C, Unpaired spectrum, frequency < 1.88GHz.
  freq_arfcn      = 292000;
  in_burst_bitmap = static_cast<uint64_t>(0b11110000U) << static_cast<uint64_t>(56U);

  // Test different offset_to_point_A and different k_SSB.
  periodicity = ssb_periodicity::ms10;
  // For case C (20MHz, SCS 30kHz), there are 52 within the Initial DL BWP.
  max_offset_to_point_A = 52 - NOF_SSB_PRBS;
  for (uint16_t offset_to_point_A = 0; offset_to_point_A < max_offset_to_point_A; offset_to_point_A += 2) {
    for (uint8_t k_ssb_val = 0; k_ssb_val < 24; k_ssb_val++) {
      test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb_val);
    }
  }

  // ##########################################################
  //                   TEST CASE C
  // ##########################################################
  // TEST Case C, Unpaired spectrum, frequency > 1.88GHz.
  freq_arfcn      = 518000;
  in_burst_bitmap = static_cast<uint64_t>(0b11111111U) << static_cast<uint64_t>(56U);

  // Test different offset_to_point_A and different k_SSB.
  periodicity = ssb_periodicity::ms10;
  for (uint16_t offset_to_point_A = 0; offset_to_point_A < max_offset_to_point_A; offset_to_point_A += 2) {
    for (uint8_t k_ssb_val = 0; k_ssb_val < 24; k_ssb_val++) {
      test_ssb_allocation(periodicity, freq_arfcn, offset_to_point_A, in_burst_bitmap, ssb_scs, k_ssb_val);
    }
  }
}
