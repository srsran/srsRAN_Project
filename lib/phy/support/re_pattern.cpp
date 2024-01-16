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

#include "srsran/phy/support/re_pattern.h"

using namespace srsran;

void re_pattern::get_inclusion_mask(bounded_bitset<MAX_RB * NRE>& mask, unsigned symbol) const
{
  // Skip if the symbol is not set to true.
  if (!symbols.test(symbol)) {
    return;
  }

  // Generate a RE mask for the entire bandwidth.
  bounded_bitset<MAX_RB* NRE> pattern_re_mask = prb_mask.kronecker_product<NRE>(re_mask);

  // Append zeros or discard bits to match the input mask size.
  pattern_re_mask.resize(mask.size());

  // The following logical operation shall result in:
  // - true if mask is true or re_mask is true,
  // - otherwise false.
  mask |= pattern_re_mask;
}

void re_pattern::get_exclusion_mask(bounded_bitset<MAX_RB * NRE>& mask, unsigned symbol) const
{
  // Skip if the symbol is not used.
  if (!symbols.test(symbol)) {
    return;
  }

  // Generate a RE mask for the entire bandwidth.
  bounded_bitset<MAX_RB* NRE> pattern_re_mask = prb_mask.kronecker_product<NRE>(re_mask);

  // Append zeros or discard bits to match the input mask size.
  pattern_re_mask.resize(mask.size());

  // The following logical operation shall result in:
  // - true if mask is true and re_mask is false,
  // - otherwise false.
  mask &= (~pattern_re_mask);
}

void re_pattern_list::merge(const re_pattern& pattern)
{
  // Iterate all given patterns.
  for (re_pattern& p : list) {
    // Skip if RB allocation parameters do NOT match.
    bool prb_match = (p.prb_mask == pattern.prb_mask);

    // Check if symbol mask matches.
    bool lmatch = (pattern.symbols == p.symbols);

    // Check if RE mask matches.
    bool kmatch = (pattern.re_mask == p.re_mask);

    // If PRB, OFDM symbols and subcarriers mask match, it means that the patterns are completely overlapped and no
    // merging is required.
    if (prb_match && kmatch && lmatch) {
      return;
    }

    // OFDM symbols and subcarriers mask match, combine PRB.
    if (lmatch && kmatch) {
      bounded_bitset<MAX_RB> temp_prb_mask = pattern.prb_mask;
      if (p.prb_mask.size() < temp_prb_mask.size()) {
        p.prb_mask.resize(temp_prb_mask.size());
      } else if (p.prb_mask.size() > temp_prb_mask.size()) {
        temp_prb_mask.resize(p.prb_mask.size());
      }
      p.prb_mask |= temp_prb_mask;
      return;
    }

    // If OFDM symbols mask matches, combine subcarrier mask.
    if (prb_match && lmatch) {
      p.re_mask |= pattern.re_mask;
      return;
    }

    // If subcarriers mask matches, combine OFDM symbols mask.
    if (prb_match && kmatch) {
      p.symbols |= pattern.symbols;
      return;
    }
  }

  // If reached here, no pattern was matched. Check if there is free space.
  srsran_assert(
      !list.full(), "RE pattern list is full. It seems {} maximum entries are not enough.", (unsigned)MAX_RE_PATTERN);

  // Append pattern.
  list.emplace_back(pattern);
}

bool re_pattern_list::operator==(const re_pattern_list& other) const
{
  // Generates the inclusion mask for each symbol and compare if they are equal.
  for (unsigned symbol = 0; symbol != MAX_NSYMB_PER_SLOT; ++symbol) {
    bounded_bitset<MAX_RB * NRE> inclusion_mask(MAX_RB * NRE);
    get_inclusion_mask(inclusion_mask, symbol);

    bounded_bitset<MAX_RB * NRE> inclusion_mask_other(MAX_RB * NRE);
    other.get_inclusion_mask(inclusion_mask_other, symbol);

    // Early return false if they are not equal for this symbol.
    if (inclusion_mask != inclusion_mask_other) {
      return false;
    }
  }

  return true;
}

void re_pattern_list::get_inclusion_mask(bounded_bitset<MAX_RB * NRE>& mask, unsigned symbol) const
{
  // Iterate all given patterns.
  for (const re_pattern& p : list) {
    p.get_inclusion_mask(mask, symbol);
  }
}

unsigned re_pattern_list::get_inclusion_count(unsigned                      start_symbol,
                                              unsigned                      nof_symbols,
                                              const bounded_bitset<MAX_RB>& rb_mask) const
{
  // Early return if the list is empty.
  if (list.empty()) {
    return 0;
  }

  // Direct calculation if the list contains only one entry.
  if (list.size() == 1) {
    const re_pattern& pattern = list.front();

    // Get PRB mask from the pattern.
    bounded_bitset<MAX_RB> prb_mask = pattern.prb_mask;

    // Adapt pattern to the mask size.
    prb_mask.resize(rb_mask.size());

    // Filter the PRB of interest.
    prb_mask &= rb_mask;

    return prb_mask.count() * pattern.symbols.count() * pattern.re_mask.count();
  }

  unsigned count = 0;

  re_prb_mask                 base_re_mask   = ~re_prb_mask();
  bounded_bitset<MAX_RB* NRE> active_re_mask = rb_mask.kronecker_product<NRE>(base_re_mask);

  for (unsigned symbol_idx = start_symbol; symbol_idx != start_symbol + nof_symbols; ++symbol_idx) {
    bounded_bitset<MAX_RB * NRE> inclusion_mask(active_re_mask.size());

    // Iterate all patterns to get a wideband inclusion mask.
    for (const re_pattern& p : list) {
      p.get_inclusion_mask(inclusion_mask, symbol_idx);
    }

    // Apply the mask to the active RE.
    inclusion_mask &= active_re_mask;

    // Count all the included elements.
    count += inclusion_mask.count();
  }

  return count;
}

void re_pattern_list::get_exclusion_mask(bounded_bitset<MAX_RB * NRE>& mask, unsigned symbol) const
{
  // Iterate all given patterns.
  for (const re_pattern& p : list) {
    p.get_exclusion_mask(mask, symbol);
  }
}
