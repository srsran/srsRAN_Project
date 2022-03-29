#include "../../../lib/mac/sched/sched_ssb.h"
#include "srsgnb/ran/frame_types.h"
#include "srsgnb/support/test_utils.h"

/// This will be removed once we can get this value from the slot object
#define TEST_HARQ_ASSERT_MSG(SLOT, PERIODICITY, CASE)                                                                  \
  "Failed at slot: '{}', periodicity '{}', case '{}'", SLOT, PERIODICITY, ssb_case_to_str(CASE)

using namespace srsgnb;

/// Helper function to print the SSB CASE
const char* ssb_case_to_str(ssb_alloc_case ssb_case)
{
  switch (ssb_case) {
    case srsgnb::ssb_alloc_case::A:
      return "SSB case A";
    case srsgnb::ssb_alloc_case::B:
      return "SSB case B";
    case srsgnb::ssb_alloc_case::C_paired:
      return "SSB case C - paired spectrum";
    case srsgnb::ssb_alloc_case::C_unpaired:
      return "SSB case C - unpaired spectrum";
    default:
      return "SSB case invalid";
  }
}

/// Helper struct to test HARQs and update loggers slot context
struct test_bench {
  static srslog::basic_logger& test_logger;
  static srslog::basic_logger& mac_logger;

  test_bench(ssb_alloc_case ssb_case)
  {
    uint8_t numerology = 0;
    switch (ssb_case) {
      case ssb_alloc_case::A:
        numerology = 0;
        break;
      case ssb_alloc_case::B:
      case ssb_alloc_case::C_paired:
      case ssb_alloc_case::C_unpaired:
        numerology = 1;
        break;
      default:
        srsran_assert(ssb_case < ssb_alloc_case::invalid, "Invalid SSB case. Only case A, B, and C are supported");
    }
    test_logger.set_context(0);
    mac_logger.set_context(0);
    t = slot_point(numerology, 0);
  }

  void new_slot()
  {
    t++;
    test_logger.set_context(t.to_uint());
    mac_logger.set_context(t.to_uint());
  }

  slot_point slot_tx() { return t; }

private:
  slot_point t;
};

srslog::basic_logger& test_bench::test_logger = srslog::fetch_basic_logger("TEST");
srslog::basic_logger& test_bench::mac_logger  = srslog::fetch_basic_logger("MAC-NR");

/// This function tests SSB case A and C (both paired and unpaired spectrum)
void test_ssb_case_A_C(const slot_point& slot_tx,
                       uint16_t          ssb_periodicity,
                       uint32_t          offset_to_point_A,
                       uint32_t          freq_arfcn,
                       uint32_t          freq_cutoff,
                       uint8_t           in_burst_bitmap,
                       ssb_alloc_case    ssb_case,
                       const ssb_list_t& ssb_list)
{
  /// For frequencies lower than the cutoff, there should only be at most 4 SSB opportunities (4 left-most bits in
  /// in_burst_bitmap)
  if (freq_arfcn <= freq_cutoff) {
    TESTASSERT((in_burst_bitmap & 0b00001111) == 0, TEST_HARQ_ASSERT_MSG(slot_tx.to_uint(), ssb_periodicity, ssb_case));
  }

  uint32_t sl_point_mod = slot_tx.to_uint() % (ssb_periodicity * slot_tx.nof_slots_per_subframe());

  /// Get the size of the SSB list from the in_burst_bitmap
  size_t ssb_list_size = 0;
  if (sl_point_mod == 0) {
    ssb_list_size = ((in_burst_bitmap & 0b10000000) >> 7) + ((in_burst_bitmap & 0b01000000) >> 6);
  } else if (sl_point_mod == 1) {
    ssb_list_size = ((in_burst_bitmap & 0b00100000) >> 5) + ((in_burst_bitmap & 0b00010000) >> 4);
  } else if (sl_point_mod == 2) {
    ssb_list_size = ((in_burst_bitmap & 0b00001000) >> 3) + ((in_burst_bitmap & 0b0000100) >> 2);
  } else if (sl_point_mod == 3) {
    ssb_list_size = ((in_burst_bitmap & 0b00000010) >> 1) + (in_burst_bitmap & 0b00000001);
  }
  /// Check the SSB list size
  TESTASSERT_EQ(ssb_list_size, ssb_list.size(), TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));

  if (sl_point_mod == 0 or sl_point_mod == 1 or sl_point_mod == 2 or sl_point_mod == 3) {
    std::array<uint8_t, 2> ofdm_symbols = {2, 8};
    uint8_t                ssb_idx_mask = 0b10000000 >> (sl_point_mod * 2);
    auto                   it           = ssb_list.begin();

    /// Check if, for each OFDM symbols in the bitmap, there is a corresponding SSB from the list
    for (uint8_t n = 0; n < ofdm_symbols.size(); n++) {
      if ((in_burst_bitmap & ssb_idx_mask) > 0) {
        auto ssb_item = *it;

        /// Check whether it's the first transmission of the burst
        uint8_t         ssb_idx               = n + sl_point_mod * 2;
        bool            is_first_transmission = (in_burst_bitmap & (0b11111111 >> ssb_idx)) == in_burst_bitmap;
        ssb_tx_mode_opt tx_mode = is_first_transmission ? srsgnb::ssb_transmission : srsgnb::ssb_repetition;
        TESTASSERT_EQ(tx_mode, ssb_item.tx_mode, TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));

        /// Check OFDM symbols and frequency allocation
        TESTASSERT_EQ(ofdm_symbols[n] + sl_point_mod * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                      ssb_item.ssb_info.ofdm_symbols.start(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));
        TESTASSERT_EQ(ofdm_symbols[n] + sl_point_mod * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP + 4,
                      ssb_item.ssb_info.ofdm_symbols.stop(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));
        TESTASSERT_EQ(offset_to_point_A,
                      ssb_item.ssb_info.prb_alloc.start(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));
        TESTASSERT_EQ(offset_to_point_A + 20,
                      ssb_item.ssb_info.prb_alloc.stop(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));
        it++;
      }

      ssb_idx_mask = ssb_idx_mask >> 1;
    }
  }
}

/// This function tests SSB case B
void test_ssb_case_B(const slot_point& slot_tx,
                     uint16_t          ssb_periodicity,
                     uint32_t          offset_to_point_A,
                     uint32_t          freq_arfcn,
                     uint8_t           in_burst_bitmap,
                     ssb_alloc_case    ssb_case,
                     const ssb_list_t& ssb_list)
{
  /// For frequencies lower than the cutoff, there should only be at most 4 SSB opportunities (4 left-most bits in
  /// in_burst_bitmap)
  if (freq_arfcn <= 600000) {
    TESTASSERT((in_burst_bitmap & 0b00001111) == 0, TEST_HARQ_ASSERT_MSG(slot_tx.to_uint(), ssb_periodicity, ssb_case));
  }

  uint32_t sl_point_mod = slot_tx.to_uint() % (ssb_periodicity * slot_tx.nof_slots_per_subframe());

  /// Get the size of the SSB list from the in_burst_bitmap
  size_t ssb_list_size = 0;
  if (sl_point_mod == 0) {
    ssb_list_size = ((in_burst_bitmap & 0b10000000) >> 7) + ((in_burst_bitmap & 0b01000000) >> 6);
  } else if (sl_point_mod == 1) {
    ssb_list_size = ((in_burst_bitmap & 0b00100000) >> 5) + ((in_burst_bitmap & 0b00010000) >> 4);
  } else if (sl_point_mod == 2) {
    ssb_list_size = ((in_burst_bitmap & 0b00001000) >> 3) + ((in_burst_bitmap & 0b0000100) >> 2);
  } else if (sl_point_mod == 3) {
    ssb_list_size = ((in_burst_bitmap & 0b00000010) >> 1) + (in_burst_bitmap & 0b00000001);
  }
  /// Check the SSB list size
  TESTASSERT_EQ(ssb_list_size, ssb_list.size(), TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));

  if (sl_point_mod == 0 or sl_point_mod == 2) {
    /// For frequency less than the CUTOFF freq, list cannot have more than 4 elements
    std::array<uint8_t, 2> ofdm_symbols = {4, 8};
    uint8_t                ssb_idx_mask = 0b10000000 >> (sl_point_mod * 2);
    auto                   it           = ssb_list.begin();

    /// Check if, for each OFDM symbols in the bitmap, there is a corresponding SSB from the list
    for (uint8_t n = 0; n < ofdm_symbols.size(); n++) {
      if ((in_burst_bitmap & ssb_idx_mask) > 0) {
        auto ssb_item = *it;

        /// Check whether it's the first transmission of the burst
        uint8_t         ssb_idx               = n + sl_point_mod * 2;
        bool            is_first_transmission = (in_burst_bitmap & (0b11111111 >> ssb_idx)) == in_burst_bitmap;
        ssb_tx_mode_opt tx_mode = is_first_transmission ? srsgnb::ssb_transmission : srsgnb::ssb_repetition;
        TESTASSERT_EQ(tx_mode, ssb_item.tx_mode, TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));

        /// Check OFDM symbols and frequency allocation
        TESTASSERT_EQ(ofdm_symbols[n] + sl_point_mod * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                      ssb_item.ssb_info.ofdm_symbols.start(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));
        TESTASSERT_EQ(ofdm_symbols[n] + sl_point_mod * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP + 4,
                      ssb_item.ssb_info.ofdm_symbols.stop(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));
        TESTASSERT_EQ(offset_to_point_A,
                      ssb_item.ssb_info.prb_alloc.start(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));
        TESTASSERT_EQ(offset_to_point_A + 20,
                      ssb_item.ssb_info.prb_alloc.stop(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));
        it++;
      }

      ssb_idx_mask = ssb_idx_mask >> 1;
    }
  }

  if (sl_point_mod == 1 or sl_point_mod == 3) {
    /// For frequency less than the CUTOFF freq, list cannot have more than 4 elements
    std::array<uint8_t, 2> ofdm_symbols = {16, 20};
    uint8_t                ssb_idx_mask = 0b00100000 >> ((sl_point_mod - 1) * 2);
    auto                   it           = ssb_list.begin();

    /// Check if, for each OFDM symbols in the bitmap, there is a corresponding SSB from the list
    for (uint8_t n = 0; n < ofdm_symbols.size(); n++) {
      if ((in_burst_bitmap & ssb_idx_mask) > 0) {
        auto ssb_item = *it;

        /// Check whether it's the first transmission of the burst
        uint8_t         ssb_idx               = n + sl_point_mod * 2;
        bool            is_first_transmission = (in_burst_bitmap & (0b11111111 >> ssb_idx)) == in_burst_bitmap;
        ssb_tx_mode_opt tx_mode = is_first_transmission ? srsgnb::ssb_transmission : srsgnb::ssb_repetition;
        TESTASSERT_EQ(tx_mode, ssb_item.tx_mode, TEST_HARQ_ASSERT_MSG(slot_tx.to_uint(), ssb_periodicity, ssb_case));

        /// Check OFDM symbols and frequency allocation
        TESTASSERT_EQ(ofdm_symbols[n] + (sl_point_mod - 1) * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                      ssb_item.ssb_info.ofdm_symbols.start(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));
        TESTASSERT_EQ(ofdm_symbols[n] + (sl_point_mod - 1) * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP + 4,
                      ssb_item.ssb_info.ofdm_symbols.stop(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));
        TESTASSERT_EQ(offset_to_point_A,
                      ssb_item.ssb_info.prb_alloc.start(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));
        TESTASSERT_EQ(offset_to_point_A + 20,
                      ssb_item.ssb_info.prb_alloc.stop(),
                      TEST_HARQ_ASSERT_MSG(sl_point_mod, ssb_periodicity, ssb_case));
        it++;
      }

      ssb_idx_mask = ssb_idx_mask >> 1;
    }
  }
}

void test_ssb_time_allocation(uint16_t       ssb_periodicity,
                              uint32_t       freq_arfcn,
                              uint8_t        in_burst_bitmap,
                              ssb_alloc_case ssb_case)
{
  const size_t   NUM_OF_TEST_SLOTS = 1000;
  const uint32_t offset_to_point_A = 14;

  test_bench bench{ssb_case};

  bench.new_slot();

  /// Define parameters and variable for the test
  ssb_list_t ssb_list;
  /// in_burst_bitmap is not used by the function for the time being

  /// Run test for a given number of slots
  for (size_t slot_count = 0; slot_count < NUM_OF_TEST_SLOTS; slot_count++, bench.new_slot()) {
    /// Clear the SSB list of it is not empty
    if (ssb_list.size() > 0) {
      ssb_list.clear();
    }

    /// Schedule the SSB
    sched_ssb(bench.slot_tx(), ssb_periodicity, offset_to_point_A, freq_arfcn, in_burst_bitmap, ssb_case, ssb_list);

    /// Select SSB case with reference to TS 38.213, Section 4.1
    switch (ssb_case) {
      case ssb_alloc_case::A:
      case ssb_alloc_case::C_paired: {
        test_ssb_case_A_C(bench.slot_tx(),
                          ssb_periodicity,
                          offset_to_point_A,
                          freq_arfcn,
                          600000,
                          in_burst_bitmap,
                          ssb_case,
                          ssb_list);
      } break;
      case ssb_alloc_case::B: {
        test_ssb_case_B(
            bench.slot_tx(), ssb_periodicity, offset_to_point_A, freq_arfcn, in_burst_bitmap, ssb_case, ssb_list);
      } break;
      case ssb_alloc_case::C_unpaired: {
        test_ssb_case_A_C(bench.slot_tx(),
                          ssb_periodicity,
                          offset_to_point_A,
                          freq_arfcn,
                          376000,
                          in_burst_bitmap,
                          ssb_case,
                          ssb_list);
      } break;
      default:
        srsran_assert(ssb_case < ssb_alloc_case::invalid, "Only SSB case A, B and C are currently supported");
    }
  }
}

/// This tests the SSB scheduling for different periodicities
void test_sched_ssb()
{
  /// ##########################################################
  ///                   TEST CASE A
  /// ##########################################################
  /// TEST Case A, frequency < 3GHz
  uint32_t       freq_arfcn      = 500000;
  uint8_t        in_burst_bitmap = 0b01100000;
  ssb_alloc_case ssb_case        = srsgnb::ssb_alloc_case::A;

  /// TEST Different perdiodicity with Case A
  uint16_t periodicity = 5;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity = 10;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity = 20;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity = 40;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity = 80;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  /// TEST Case A, frequency < 3GHz, different inBurst bitmaps
  periodicity     = 5;
  in_burst_bitmap = 0b10000000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity     = 5;
  in_burst_bitmap = 0b11010000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity     = 5;
  in_burst_bitmap = 0b11110000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity     = 5;
  in_burst_bitmap = 0b01110000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity     = 5;
  in_burst_bitmap = 0b00010000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  /// TEST CASE A, frequency > 3GHz, different inBurst bitmaps
  freq_arfcn      = 680000;
  in_burst_bitmap = 0b01101001;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  in_burst_bitmap = 0b11111111;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  in_burst_bitmap = 0b01111101;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  in_burst_bitmap = 0b10011110;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  in_burst_bitmap = 0b00000001;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  /// ##########################################################
  ///                   TEST CASE B
  /// ##########################################################
  /// TEST Case B, frequency < 3GHz
  ssb_case        = srsgnb::ssb_alloc_case::B;
  freq_arfcn      = 500000;
  in_burst_bitmap = 0b01100000;

  /// TEST Different perdiodicity with Case B
  periodicity = 5;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity = 10;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity = 20;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity = 40;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity = 80;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  /// TEST Case B, frequency < 3GHz, different inBurst bitmaps
  periodicity     = 5;
  in_burst_bitmap = 0b10000000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity     = 5;
  in_burst_bitmap = 0b11010000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity     = 5;
  in_burst_bitmap = 0b11110000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity     = 5;
  in_burst_bitmap = 0b01110000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity     = 5;
  in_burst_bitmap = 0b00010000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  /// TEST CASE B, frequency > 3GHz, different inBurst bitmaps
  freq_arfcn      = 680000;
  in_burst_bitmap = 0b01101001;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  in_burst_bitmap = 0b11111111;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  in_burst_bitmap = 0b01111101;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  in_burst_bitmap = 0b10011110;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  in_burst_bitmap = 0b00000001;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  /// ##########################################################
  ///                   TEST CASE C
  /// ##########################################################
  /// TEST CASE C - unpaired spectrum, freq. < 1.88GHz
  ssb_case        = srsgnb::ssb_alloc_case::C_unpaired;
  in_burst_bitmap = 0b01100000;
  freq_arfcn      = 300000;
  periodicity     = 5;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity = 10;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity = 20;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity = 40;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity = 80;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  /// TEST CASE C - unpaired spectrum, freq. < 1.88GHz, different inBurst bitmaps
  periodicity     = 5;
  in_burst_bitmap = 0b10000000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity     = 5;
  in_burst_bitmap = 0b11010000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity     = 5;
  in_burst_bitmap = 0b11110000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity     = 5;
  in_burst_bitmap = 0b01110000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  periodicity     = 5;
  in_burst_bitmap = 0b00010000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  /// TEST CASE C - unpaired spectrum, freq. > 1.88GHz, different inBurst bitmaps
  in_burst_bitmap = 0b01101001;
  freq_arfcn      = 400000;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  in_burst_bitmap = 0b11111111;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  in_burst_bitmap = 0b01111101;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);

  in_burst_bitmap = 0b10011110;
  test_ssb_time_allocation(periodicity, freq_arfcn, in_burst_bitmap, ssb_case);
}

int main()
{
  /// Initialize logger
  srslog::init();

  /// Test SSB scheduling
  test_sched_ssb();

  return 0;
}
