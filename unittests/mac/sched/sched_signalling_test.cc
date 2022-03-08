#include "../../../lib/mac/sched/sched_ssb.h"
#include "srsgnb/support/test_utils.h"

/// This will be removed once we can get this value from the slot object
#define NOF_SLOTS_PER_SUBFRAME 1
#define TEST_HARQ_ASSERT_MSG(SLOT, PERIODICITY)                                                                        \
  fmt::format("Failed at slot: '{}' and periodicity '{}'", SLOT, PERIODICITY).c_str()

using namespace srsgnb;

/// Helper struct to test HARQs and update loggers slot context
struct test_bench {
  static srslog::basic_logger& test_logger;
  static srslog::basic_logger& mac_logger;

  test_bench()
  {
    test_logger.set_context(0);
    mac_logger.set_context(0);
  }

  void new_slot()
  {
    ++t;
    test_logger.set_context(t);
    mac_logger.set_context(t);
  }

  slot_point slot_tx() { return t; }

private:
  // Let it do the job of finding the MAX UINT 32
  slot_point t = -1;
};
srslog::basic_logger& test_bench::test_logger = srslog::fetch_basic_logger("TEST");
srslog::basic_logger& test_bench::mac_logger  = srslog::fetch_basic_logger("MAC-NR");

void test_ssb_time_allocation(uint16_t periodicity)
{
  const size_t NUM_OF_TEST_SLOTS = 1000;

  test_bench bench{};

  bench.new_slot();

  // Define parameters and variable for the test
  ssb_list_t ssb_list;
  // in_burst_bitmap is not used by the function for the time being
  uint64_t in_burst_bitmap   = 0;
  uint32_t offset_to_point_A = 14;
  uint16_t k_ssb             = 0;

  // Run test for a given number of slots
  for (; bench.slot_tx() < NUM_OF_TEST_SLOTS; bench.new_slot()) {
    // Clear the SSB list of it is not empty
    if (ssb_list.size() > 0) {
      ssb_list.clear();
    }

    // Schedule the SSB
    sched_ssb(bench.slot_tx(), periodicity, k_ssb, offset_to_point_A, in_burst_bitmap, ssb_list);

    // Test that SSB is scheduled only according to periodicity
    // For now, all the parameters are hard-coded.
    // TODO: When the function gets extended, improve the test by moving the parameters as function inputs
    if (bench.slot_tx() % (uint32_t)(periodicity * NOF_SLOTS_PER_SUBFRAME) == 0) {
      TESTASSERT_EQ_MSG(ssb_list.size(), 1, TEST_HARQ_ASSERT_MSG(bench.slot_tx(), periodicity));
      auto& ssb_item = ssb_list.back();
      TESTASSERT_EQ_MSG(ssb_item.tx_mode, srsgnb::ssb_transmission, TEST_HARQ_ASSERT_MSG(bench.slot_tx(), periodicity));
      TESTASSERT_EQ_MSG(
          ssb_item.ssb_info.ofdm_symbols.start(), 4, TEST_HARQ_ASSERT_MSG(bench.slot_tx(), periodicity));
      TESTASSERT_EQ_MSG(
          ssb_item.ssb_info.ofdm_symbols.stop(), 4 + 4, TEST_HARQ_ASSERT_MSG(bench.slot_tx(), periodicity));
      TESTASSERT_EQ_MSG(ssb_item.ssb_info.prb_alloc.start(),
                        offset_to_point_A,
                        TEST_HARQ_ASSERT_MSG(bench.slot_tx(), periodicity));
      TESTASSERT_EQ_MSG(ssb_item.ssb_info.prb_alloc.stop(),
                        offset_to_point_A + 20,
                        TEST_HARQ_ASSERT_MSG(bench.slot_tx(), periodicity));
    } else {
      TESTASSERT_EQ_MSG(ssb_list.size(), 0, TEST_HARQ_ASSERT_MSG(bench.slot_tx(), periodicity));
    }
  }
}

/// This tests the SSB scheduling for different periodicities
void test_sched_ssb()
{
  uint16_t periodicity = 5;
  test_ssb_time_allocation(periodicity);

  periodicity = 10;
  test_ssb_time_allocation(periodicity);

  periodicity = 20;
  test_ssb_time_allocation(periodicity);

  periodicity = 40;
  test_ssb_time_allocation(periodicity);

  periodicity = 80;
  test_ssb_time_allocation(periodicity);
}

int main()
{
  // Initialize logger
  srslog::init();

  // Test SSB scheduling
  test_sched_ssb();

  return 0;
}