/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/cyclic_prefix.h"
#include "srsgnb/phy/upper/re_pattern.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

// Tests that two patterns with subcarrier mask in common are merged into one.
void test_merge_even()
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
  for (unsigned k = 0; k != NRE; ++k) {
    pattern_1.re_mask[k] = (k % 2 == 0);
  }
  for (unsigned l = 0; l != MAX_NSYMB_PER_SLOT; ++l) {
    pattern_1.symbols[l] = (l % 2 == 0);
  }
  list.merge(pattern_1);
  TESTASSERT_EQ(list.get_nof_entries(), 1);

  // Create second pattern:
  // - Even subcarrier indexes, and
  // - odd symbol indexes.
  re_pattern pattern_2 = pattern_1;
  for (unsigned l = 0; l != MAX_NSYMB_PER_SLOT; ++l) {
    pattern_2.symbols[l] = (l % 2 == 1); // Only odd symbols
  }
  list.merge(pattern_2);
  TESTASSERT_EQ(list.get_nof_entries(), 1);

  // The pattern should be repeated for each symbol.
  for (unsigned l = 0; l != MAX_NSYMB_PER_SLOT; ++l) {
    // Create mask with all entries to false
    std::array<bool, MAX_RB* NRE> mask = {};

    // Set include mask.
    list.get_inclusion_mask(mask, l);

    // For each subcarrier it checks the mask.
    for (unsigned k = 0; k != MAX_RB * NRE; ++k) {
      bool gold = false;

      // if it is in the RB range, then check the subcarrier index.
      if ((k >= rb_begin * NRE) && (k < rb_end * NRE) && ((k / NRE - rb_begin) % rb_stride == 0)) {
        // Even subcarrier
        gold = (k % 2 == 0);
      }
      TESTASSERT_EQ(mask[k], gold);
    }

    // Set exclude.
    list.get_exclusion_mask(mask, l);

    // All the subcarriers shall be false.
    for (unsigned k = 0; k != MAX_RB * NRE; ++k) {
      TESTASSERT(!mask[k]);
    }
  }
}

// Tests that two patterns with symbol mask in common are merged into one.
void test_merge_odd()
{
  unsigned rb_begin  = 1;
  unsigned rb_end    = 50;
  unsigned rb_stride = 2;

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
  for (unsigned k = 0; k != NRE; ++k) {
    pattern_1.re_mask[k] = (k % 2 == 0);
  }
  for (unsigned l = 0; l != MAX_NSYMB_PER_SLOT; ++l) {
    pattern_1.symbols[l] = (l % 2 == 0);
  }
  list.merge(pattern_1);
  TESTASSERT(list.get_nof_entries() == 1);

  // Create second pattern:
  // - odd subcarrier indexes, and
  // - even symbol indexes.
  re_pattern pattern_2 = pattern_1;
  for (unsigned k = 0; k != NRE; ++k) {
    pattern_2.re_mask[k] = (k % 2 == 1);
  }
  list.merge(pattern_2);
  TESTASSERT(list.get_nof_entries() == 1);

  // The pattern should be repeated for each symbol.
  for (unsigned l = 0; l != MAX_NSYMB_PER_SLOT; ++l) {
    // Create mask with all entries to false
    std::array<bool, MAX_RB* NRE> mask = {};

    // Set include mask.
    list.get_inclusion_mask(mask, l);

    // For each subcarrier it checks the mask.
    for (unsigned k = 0; k != MAX_RB * NRE; ++k) {
      bool gold = false;

      // if it is in the RB range, then check the subcarrier index.
      if (k >= rb_begin * NRE && k < rb_end * NRE && (k / NRE - rb_begin) % rb_stride == 0) {
        // Even symbol
        gold = (l % 2 == 0);
      }
      TESTASSERT_EQ(mask[k], gold);
    }

    // Set exclude.
    list.get_exclusion_mask(mask, l);

    // All the subcarriers shall be false.
    for (unsigned k = 0; k != MAX_RB * NRE; ++k) {
      TESTASSERT(!mask[k]);
    }
  }
}

// Tests a merge between two patterns are the same.
void test_merge_same()
{
  // Create a pattern.
  re_pattern pattern;
  pattern.rb_begin   = 0;
  pattern.rb_end     = 1;
  pattern.rb_stride  = 1;
  pattern.re_mask    = {};
  pattern.re_mask[0] = true;
  pattern.symbols    = {};
  pattern.symbols[0] = true;

  // Add the pattern to a list twice.
  re_pattern_list list;
  list.merge(pattern);
  list.merge(pattern);

  // The number of entries shall be 1.
  TESTASSERT_EQ(list.get_nof_entries(), 1);
}

// Tests a merge between two patterns with different RB mapping.
void test_merge_diff_rb()
{
  // Create pattern 1.
  re_pattern pattern1;
  pattern1.rb_begin   = 0;
  pattern1.rb_end     = 1;
  pattern1.rb_stride  = 1;
  pattern1.re_mask    = {};
  pattern1.re_mask[0] = true;
  pattern1.symbols    = {};
  pattern1.symbols[0] = true;

  // Create a pattern 2.
  re_pattern pattern2;
  pattern1.rb_begin   = 1;
  pattern1.rb_end     = 2;
  pattern1.rb_stride  = 1;
  pattern1.re_mask    = {};
  pattern1.re_mask[0] = true;
  pattern1.symbols    = {};
  pattern1.symbols[0] = true;

  // Add the pattern to the list.
  re_pattern_list list;
  list.merge(pattern1);
  list.merge(pattern2);

  // The number of entries shall be 2.
  TESTASSERT_EQ(list.get_nof_entries(), 2);
}

void test_inclusion_count()
{
  // Create a pattern.
  re_pattern pattern;
  pattern.rb_begin   = 0;
  pattern.rb_end     = 1;
  pattern.rb_stride  = 1;
  pattern.re_mask    = {};
  pattern.re_mask[0] = true;
  pattern.symbols    = {};
  pattern.symbols[0] = true;

  // Add the pattern to a list.
  re_pattern_list list;
  list.merge(pattern);

  // Validate the inclusion count with an RB mask that matches.
  bounded_bitset<MAX_RB> rb_mask_match(2);
  rb_mask_match.set(pattern.rb_begin, true);
  TESTASSERT_EQ(list.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, rb_mask_match), 1);

  // Validate the inclusion count with an RB mask that does not match.
  bounded_bitset<MAX_RB> rb_mask_unmatch(2);
  rb_mask_unmatch.set(pattern.rb_end, true);
  TESTASSERT_EQ(list.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, rb_mask_unmatch), 0);
}

// Test that a pattern is equal to itself and different to others.
void test_equal()
{
  // Create a pattern.
  re_pattern pattern;
  pattern.rb_begin   = 0;
  pattern.rb_end     = 1;
  pattern.rb_stride  = 1;
  pattern.re_mask    = {};
  pattern.re_mask[0] = true;
  pattern.symbols    = {};
  pattern.symbols[0] = true;

  // Add the pattern to a first list.
  re_pattern_list list1;
  list1.merge(pattern);

  // Modify pattern.
  pattern.symbols[1] = true;

  // Add the pattern to a list.
  re_pattern_list list2;
  list2.merge(pattern);

  // Validate the equal operator.
  TESTASSERT(list1 == list1);
  TESTASSERT(list2 == list2);

  // Validate the not equal operator.
  TESTASSERT(list1 != list2);
}

// Test a pattern creation from an initializer list.
void test_bracket_initializer()
{
  // Create pattern list from full initializer list.
  re_pattern_list list1 = {
      {0, 52, 1, {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}},
      {0, 52, 1, {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}}};

  // Create same pattern using parameters.
  re_pattern pattern;
  pattern.rb_begin    = 0;
  pattern.rb_end      = 52;
  pattern.rb_stride   = 1;
  pattern.re_mask     = {};
  pattern.re_mask[0]  = true;
  pattern.re_mask[4]  = true;
  pattern.re_mask[8]  = true;
  pattern.symbols     = {};
  pattern.symbols[6]  = true;
  pattern.symbols[10] = true;

  // Create list using initializer list of pattern.
  re_pattern_list list2 = {pattern};

  // Create list using default constructor and merge pattern.
  re_pattern_list list3;
  list3.merge(pattern);

  // Validate that all lists are equal.
  TESTASSERT(list1 == list2);
  TESTASSERT(list1 == list3);
}

int main()
{
  test_merge_even();
  test_merge_odd();
  test_merge_same();
  test_merge_diff_rb();
  test_inclusion_count();
  test_equal();
  test_bracket_initializer();
  return 0;
}