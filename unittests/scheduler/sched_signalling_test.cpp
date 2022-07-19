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
#include "lib/scheduler/cell/resource_grid.h"
#include "lib/scheduler/common_scheduling/ssb_scheduler.h"
#include "srsgnb/ran/frame_types.h"
#include "srsgnb/ran/ssb_mapping.h"
#include "srsgnb/scheduler/sched_consts.h"
#include "srsgnb/support/test_utils.h"

/// This will be removed once we can get this value from the slot object.
#define TEST_HARQ_ASSERT_MSG(SLOT, PERIODICITY, CASE)                                                                  \
  "Failed at slot: '{}', periodicity '{}', case '{}'", SLOT, PERIODICITY, ssb_case_to_str(CASE)

using namespace srsgnb;

/// Helper function to print the SSB CASE.
const char* ssb_case_to_str(ssb_pattern_case ssb_case)
{
  switch (ssb_case) {
    case ssb_pattern_case::A:
      return "SSB case A";
    case srsgnb::ssb_pattern_case::B:
      return "SSB case B";
    case srsgnb::ssb_pattern_case::C:
      return "SSB case C - paired spectrum";
    default:
      return "SSB case invalid";
  }
}

/// Helper struct to test HARQs and update loggers slot context.
struct test_bench {
  static srslog::basic_logger& test_logger;
  static srslog::basic_logger& mac_logger;

  const slot_difference tx_delay = 4;

  test_bench(sched_cell_configuration_request_message cell_cfg_msg) : cfg(cell_cfg_msg), cell_res_grid(cfg)
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
    test_logger.set_context(0);
    mac_logger.set_context(0);
    t = slot_point(numerology, 0);
  }

  void new_slot()
  {
    t++;
    cell_res_grid.slot_indication(t + tx_delay);
    test_logger.set_context(t.to_uint());
    mac_logger.set_context(t.to_uint());
  }

  cell_slot_resource_allocator& get_slot_allocator() { return cell_res_grid; }
  const cell_configuration&     get_cell_sched_config() { return cfg; };

  slot_point slot_tx() { return t; }

private:
  slot_point                   t;
  const cell_configuration     cfg;
  cell_slot_resource_allocator cell_res_grid;
};

srslog::basic_logger& test_bench::test_logger = srslog::fetch_basic_logger("TEST");
srslog::basic_logger& test_bench::mac_logger  = srslog::fetch_basic_logger("MAC-NR");

/// This function tests SSB case A and C (both paired and unpaired spectrum).
void test_ssb_case_A_C(const slot_point&             slot_tx,
                       uint32_t                      freq_cutoff,
                       const cell_configuration&     cell_cfg,
                       cell_slot_resource_allocator& slot_alloc)
{
  const ssb_configuration& ssb_cfg = cell_cfg.ssb_cfg;

  // For frequencies lower than the cutoff, there should only be at most 4 SSB opportunities (4 left-most bits in
  // in_burst_bitmap).
  if (cell_cfg.dl_carrier.arfcn <= freq_cutoff) {
    TESTASSERT((cell_cfg.ssb_cfg.ssb_bitmap & 0b00001111) == 0,
               TEST_HARQ_ASSERT_MSG(slot_tx.to_uint(), ssb_cfg.ssb_period, cell_cfg.ssb_case));
  }

  uint32_t sl_point_mod =
      slot_tx.to_uint() % (ssb_periodicity_to_value(ssb_cfg.ssb_period) * slot_tx.nof_slots_per_subframe());

  // Use ssb_bitmap with 8 bits, to simplify the code (L_max 64 is not used at the moment).
  uint8_t in_burst_bitmap = static_cast<uint8_t>(ssb_cfg.ssb_bitmap >> 56U);

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

  if (sl_point_mod == 0 or sl_point_mod == 1 or sl_point_mod == 2 or sl_point_mod == 3) {
    std::array<uint8_t, 2> ofdm_symbols = {2, 8};
    uint8_t                ssb_idx_mask = 0b10000000 >> (sl_point_mod * 2);
    auto                   it           = ssb_list.begin();

    // Check if, for each OFDM symbols in the bitmap, there is a corresponding SSB from the list.
    for (uint8_t n = 0; n < ofdm_symbols.size(); n++) {
      if ((in_burst_bitmap & ssb_idx_mask) > 0) {
        auto ssb_item = *it;

        // Check OFDM symbols and frequency allocation.
        TESTASSERT_EQ(ofdm_symbols[n] + sl_point_mod * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                      ssb_item.symbols.start(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));
        TESTASSERT_EQ(ofdm_symbols[n] + sl_point_mod * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP + 4,
                      ssb_item.symbols.stop(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));
        TESTASSERT_EQ(ssb_cfg.ssb_offset_to_point_A,
                      ssb_item.crbs.start(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));
        unsigned ssb_crb_stop = ssb_cfg.ssb_subcarrier_offset > 0
                                    ? static_cast<unsigned>(ssb_cfg.ssb_offset_to_point_A + NOF_SSB_PRBS + 1)
                                    : static_cast<unsigned>(ssb_cfg.ssb_offset_to_point_A + NOF_SSB_PRBS);
        TESTASSERT_EQ(ssb_crb_stop,
                      ssb_item.crbs.stop(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));
        it++;
      }

      ssb_idx_mask = ssb_idx_mask >> 1;
    }
  }

  // Check if DL CRB allocation is correct.
  if (ssb_list.size() > 0) {
    grant_info empty_space{grant_info::channel::ssb,
                           slot_alloc.cfg.dl_cfg_common.init_dl_bwp.generic_params.scs,
                           {0, 14},
                           {0, ssb_cfg.ssb_offset_to_point_A}};
    TESTASSERT(not slot_alloc.dl_res_grid.collides(empty_space), "CRBs {} should be empty", empty_space.crbs);
    unsigned ssb_crb_stop = ssb_cfg.ssb_subcarrier_offset > 0
                                ? static_cast<unsigned>(ssb_cfg.ssb_offset_to_point_A + NOF_SSB_PRBS + 1)
                                : static_cast<unsigned>(ssb_cfg.ssb_offset_to_point_A + NOF_SSB_PRBS);
    empty_space.crbs = {ssb_crb_stop, slot_alloc.cfg.dl_cfg_common.freq_info_dl.scs_carrier_list[0].carrier_bandwidth};
    TESTASSERT(not slot_alloc.dl_res_grid.collides(empty_space), "PRBs {} should be empty", empty_space.crbs);
    // FIXME: Check the non-empty PRBs.
  }
}

/// This function tests SSB case B.
void test_ssb_case_B(const slot_point&             slot_tx,
                     const cell_configuration&     cell_cfg,
                     cell_slot_resource_allocator& slot_alloc)
{
  const ssb_configuration& ssb_cfg = cell_cfg.ssb_cfg;

  // For frequencies lower than the cutoff, there should only be at most 4 SSB opportunities (4 left-most bits in
  // in_burst_bitmap).
  if (cell_cfg.dl_carrier.arfcn <= CUTOFF_FREQ_ARFCN_CASE_A_B_C) {
    TESTASSERT((cell_cfg.ssb_cfg.ssb_bitmap & 0b00001111) == 0,
               TEST_HARQ_ASSERT_MSG(slot_tx.to_uint(), ssb_cfg.ssb_period, cell_cfg.ssb_case));
  }

  uint32_t sl_point_mod =
      slot_tx.to_uint() % (ssb_periodicity_to_value(ssb_cfg.ssb_period) * slot_tx.nof_slots_per_subframe());

  // Use ssb_bitmap with 8 bits, to simplify the code (L_max 64 is not used at the moment).
  uint8_t in_burst_bitmap = static_cast<uint8_t>(ssb_cfg.ssb_bitmap >> 56U);

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

  // This block targets SSB occation at ofdm_symbols = {4, 8, 32, 36}.
  if (sl_point_mod == 0 or sl_point_mod == 2) {
    // For frequency less than the CUTOFF freq, list cannot have more than 4 elements.
    std::array<uint8_t, 2> ofdm_symbols = {4, 8};
    uint8_t                ssb_idx_mask = 0b10000000 >> (sl_point_mod * 2);
    auto                   it           = ssb_list.begin();

    // Check if, for each OFDM symbols in the bitmap, there is a corresponding SSB from the list.
    for (uint8_t n = 0; n < ofdm_symbols.size(); n++) {
      if ((in_burst_bitmap & ssb_idx_mask) > 0) {
        auto ssb_item = *it;

        // Check OFDM symbols and frequency allocation.
        TESTASSERT_EQ(ofdm_symbols[n] + sl_point_mod * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                      ssb_item.symbols.start(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));
        TESTASSERT_EQ(ofdm_symbols[n] + sl_point_mod * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP + 4,
                      ssb_item.symbols.stop(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));
        TESTASSERT_EQ(ssb_cfg.ssb_offset_to_point_A,
                      ssb_item.crbs.start(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));
        unsigned ssb_crb_stop = ssb_cfg.ssb_subcarrier_offset > 0
                                    ? static_cast<unsigned>(ssb_cfg.ssb_offset_to_point_A + NOF_SSB_PRBS + 1)
                                    : static_cast<unsigned>(ssb_cfg.ssb_offset_to_point_A + NOF_SSB_PRBS);
        TESTASSERT_EQ(ssb_crb_stop,
                      ssb_item.crbs.stop(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));
        it++;
      }

      ssb_idx_mask = ssb_idx_mask >> 1;
    }
  }

  // This block targets SSB occation at ofdm_symbols = {16, 20, 44, 48}.
  if (sl_point_mod == 1 or sl_point_mod == 3) {
    // For frequency less than the CUTOFF freq, list cannot have more than 4 elements
    std::array<uint8_t, 2> ofdm_symbols = {16, 20};
    uint8_t                ssb_idx_mask = 0b00100000 >> ((sl_point_mod - 1) * 2);
    auto                   it           = ssb_list.begin();

    // Check if, for each OFDM symbols in the bitmap, there is a corresponding SSB from the list.
    for (uint8_t n = 0; n < ofdm_symbols.size(); n++) {
      if ((in_burst_bitmap & ssb_idx_mask) > 0) {
        auto ssb_item = *it;

        // Check OFDM symbols and frequency allocation
        TESTASSERT_EQ(ofdm_symbols[n] + (sl_point_mod - 1) * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                      ssb_item.symbols.start(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));
        TESTASSERT_EQ(ofdm_symbols[n] + (sl_point_mod - 1) * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP + 4,
                      ssb_item.symbols.stop(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));
        TESTASSERT_EQ(ssb_cfg.ssb_offset_to_point_A,
                      ssb_item.crbs.start(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));
        unsigned ssb_crb_stop = ssb_cfg.ssb_subcarrier_offset > 0
                                    ? static_cast<unsigned>(ssb_cfg.ssb_offset_to_point_A + NOF_SSB_PRBS + 1)
                                    : static_cast<unsigned>(ssb_cfg.ssb_offset_to_point_A + NOF_SSB_PRBS);
        TESTASSERT_EQ(ssb_crb_stop,
                      ssb_item.crbs.stop(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_cfg.ssb_period, cell_cfg.ssb_case));
        it++;
      }

      ssb_idx_mask = ssb_idx_mask >> 1;
    }
  }

  // Check if DL PRB allocation is correct.
  if (ssb_list.size() > 0) {
    grant_info empty_space{grant_info::channel::ssb,
                           slot_alloc.cfg.dl_cfg_common.init_dl_bwp.generic_params.scs,
                           {0, 14},
                           {0, ssb_cfg.ssb_offset_to_point_A}};
    TESTASSERT(not slot_alloc.dl_res_grid.collides(empty_space), "PRBs {} should be empty", empty_space.crbs);
    unsigned ssb_crb_stop = ssb_cfg.ssb_subcarrier_offset > 0
                                ? static_cast<unsigned>(ssb_cfg.ssb_offset_to_point_A + NOF_SSB_PRBS + 1)
                                : static_cast<unsigned>(ssb_cfg.ssb_offset_to_point_A + NOF_SSB_PRBS);
    empty_space.crbs = {ssb_crb_stop, slot_alloc.cfg.dl_cfg_common.freq_info_dl.scs_carrier_list[0].carrier_bandwidth};
    TESTASSERT(not slot_alloc.dl_res_grid.collides(empty_space), "PRBs {} should be empty", empty_space.crbs);
    // FIXME: Check the non-empty PRBs.
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

  sched_cell_configuration_request_message cell_cfg_msg = make_default_sched_cell_configuration_request();
  cell_cfg_msg.dl_carrier.arfcn                         = freq_arfcn;
  cell_cfg_msg.ssb_config.ssb_period                    = ssb_period;
  cell_cfg_msg.ssb_config.ssb_bitmap                    = in_burst_bitmap;
  cell_cfg_msg.ssb_config.ssb_offset_to_point_A         = offset_to_point_A;
  cell_cfg_msg.ssb_config.scs                           = ssb_scs;
  // Mixed numerology is not supported yet.
  cell_cfg_msg.scs_common                       = ssb_scs;
  cell_cfg_msg.ssb_config.ssb_subcarrier_offset = k_ssb;
  test_bench bench{cell_cfg_msg};

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
    schedule_ssb(bench.get_slot_allocator(), bench.slot_tx(), bench.get_cell_sched_config());

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

/// This tests the SSB scheduling for different periodicities.
void test_time_domain_sched_ssb()
{
  // ##########################################################
  //                   TEST CASE A
  // ##########################################################
  // TEST Case A, frequency < 3GHz.
  uint32_t freq_arfcn = 500000;
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
  // TEST Case B, frequency < 3GHz
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

  // NOTE: We do not test CASE A for frequency > 3GHz. Although TS 38.213, Section 4.1 defines SSB case B for
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

int main()
{
  // Initialize logger.
  srslog::init();

  // Test SSB scheduling.
  test_time_domain_sched_ssb();

  return 0;
}
