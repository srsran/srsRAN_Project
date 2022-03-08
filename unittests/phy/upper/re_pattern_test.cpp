
#include "srsgnb/phy/upper/re_pattern.h"

using namespace srsgnb;

// Tests that two patterns with subcarrier mask in common are merged into one.
void test_merge_1()
{
  unsigned rb_begin  = 1;
  unsigned rb_end    = 50;
  unsigned rb_stride = 50;

  re_pattern_list list;

  // Reset list.
  list.clear();

  // Create first pattern containing:
  // - even subcarrier indexes, and
  // - even symbol indexes.
  re_pattern pattern_1 = {};
  pattern_1.rb_begin   = rb_begin;
  pattern_1.rb_end     = rb_end;
  pattern_1.rb_stride  = rb_stride;
  for (uint32_t k = 0; k != NRE; ++k) {
    pattern_1.re_mask[k] = (k % 2 == 0);
  }
  for (uint32_t l = 0; l < NSYMB_PER_SLOT_NORM; l++) {
    pattern_1.symbol_mask[l] = (l % 2 == 0);
  }
  list.merge(pattern_1);
  srsran_assert(list.get_nof_entries() == 1, "Failed");

  // Create second pattern:
  // - Even subcarrier indexes, and
  // - odd symbol indexes.
  re_pattern pattern_2 = pattern_1;
  for (uint32_t l = 0; l < NSYMB_PER_SLOT_NORM; l++) {
    pattern_2.symbol_mask[l] = (l % 2 == 1); // Only odd symbols
  }
  list.merge(pattern_2);
  srsran_assert(list.get_nof_entries() == 1, "Failed");

  // For each symbol, the pattern shall be repeated for each symbol
  for (uint32_t l = 0; l < NSYMB_PER_SLOT_NORM; l++) {
    // Create mask with all entries to false
    std::array<bool, MAX_RB* NRE> mask = {};

    // Set include mask.
    list.include_mask(mask, l);

    // For each subcarrier it checks the mask.
    for (uint32_t k = 0; k < MAX_RB * NRE; k++) {
      bool gold = false;

      // if it is in the RB range, then check the subcarrier index
      if (k >= rb_begin * NRE && k < rb_end * NRE && (k / NRE - rb_begin) % rb_stride == 0) {
        // Even subcarrier
        gold = (k % 2 == 0);
      }
      srsran_assert(mask[k] == gold, "Failed");
    }

    // Set exclude.
    list.exclude_mask(mask, l);

    // All the subcarriers shall be false.
    for (uint32_t k = 0; k < MAX_RB * NRE; k++) {
      srsran_assert(!mask[k], "Failed");
    }
  }
}

// Tests that two patterns with symbol mask in common are merged into one.
void test_merge_2()
{
  unsigned rb_begin  = 1;
  unsigned rb_end    = 50;
  unsigned rb_stride = 50;

  re_pattern_list list;

  // Reset list.
  list.clear();

  // Create first pattern containing:
  // - even subcarrier indexes, and
  // - even symbol indexes.
  re_pattern pattern_1 = {};
  pattern_1.rb_begin   = rb_begin;
  pattern_1.rb_end     = rb_end;
  pattern_1.rb_stride  = rb_stride;
  for (uint32_t k = 0; k != NRE; ++k) {
    pattern_1.re_mask[k] = (k % 2 == 0);
  }
  for (uint32_t l = 0; l < NSYMB_PER_SLOT_NORM; l++) {
    pattern_1.symbol_mask[l] = (l % 2 == 0);
  }
  list.merge(pattern_1);
  srsran_assert(list.get_nof_entries() == 1, "Failed");

  // Create second pattern:
  // - odd subcarrier indexes, and
  // - even symbol indexes.
  re_pattern pattern_2 = pattern_1;
  for (uint32_t k = 0; k != NRE; ++k) {
    pattern_2.re_mask[k] = (k % 2 == 1);
  }
  list.merge(pattern_2);
  srsran_assert(list.get_nof_entries() == 1, "Failed");

  // For each symbol, the pattern shall be repeated for each symbol
  for (uint32_t l = 0; l < NSYMB_PER_SLOT_NORM; l++) {
    // Create mask with all entries to false
    std::array<bool, MAX_RB* NRE> mask = {};

    // Set include mask.
    list.include_mask(mask, l);

    // For each subcarrier it checks the mask.
    for (uint32_t k = 0; k < MAX_RB * NRE; k++) {
      bool gold = false;

      // if it is in the RB range, then check the subcarrier index
      if (k >= rb_begin * NRE && k < rb_end * NRE && (k / NRE - rb_begin) % rb_stride == 0) {
        // Even subcarrier
        gold = (l % 2 == 0);
      }
      srsran_assert(mask[k] == gold, "Failed");
    }

    // Set exclude.
    list.exclude_mask(mask, l);

    // All the subcarriers shall be false.
    for (uint32_t k = 0; k < MAX_RB * NRE; k++) {
      srsran_assert(!mask[k], "Failed");
    }
  }
}

int main()
{
  test_merge_1();
  test_merge_2();
  return 0;
}