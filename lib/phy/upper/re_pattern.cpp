#include "srsgnb/phy/upper/re_pattern.h"
#include "srsgnb/phy/cyclic_prefix.h"

using namespace srsgnb;

// Helper macro to assert the RB allocation of the pattern keeping the file and line for code tracing purposes.
#define re_pattern_assert()                                                                                            \
  do {                                                                                                                 \
    srsran_assert(rb_begin < MAX_RB, "RB begin (%d) is out-of-range", rb_begin);                                       \
    srsran_assert(rb_end > 1 && rb_end <= MAX_RB, "RB end (%d) is out-of-range", rb_end);                              \
    srsran_assert(rb_stride > 0, "RB stride (%d) is out-of-range", rb_stride);                                         \
  } while (false)

void re_pattern::get_inclusion_mask(span<bool> mask, unsigned symbol) const
{
  // Verify attributes and inputs.
  re_pattern_assert();
  srsran_assert(
      mask.size() >= rb_end, "Provided mask size (%d) is too small. The minimum is %d.", (unsigned)mask.size(), rb_end);

  // Skip if the symbol is not set to true.
  if (!symbols[symbol]) {
    return;
  }

  // For each resource block within the pattern.
  for (unsigned rb = rb_begin; rb < rb_end; rb += rb_stride) {
    // For each subcarrier in the resource block, apply the inclusion operation.
    for (unsigned k = 0; k != NRE; ++k) {
      // The following logical operation shall result in:
      // - true if mask is true or re_mask is true,
      // - otherwise false.
      mask[k + rb * NRE] |= re_mask[k];
    }
  }
}

void re_pattern::get_exclusion_mask(span<bool> mask, unsigned symbol) const
{
  // Verify attributes and inputs.
  re_pattern_assert();
  srsran_assert(
      mask.size() >= rb_end, "Provided mask size (%d) is too small. The minimum is %d.", (unsigned)mask.size(), rb_end);

  // Skip if the symbol is not used.
  if (!symbols[symbol]) {
    return;
  }

  // For each resource block within the pattern.
  for (unsigned rb = rb_begin; rb < rb_end; rb += rb_stride) {
    // For each subcarrier in the resource block, apply the exclusion operation
    for (unsigned k = 0; k != NRE; ++k) {
      // The following logical operation shall result in:
      // - true if mask is true and re_mask is false,
      // - otherwise false.
      mask[k + rb * NRE] &= (!re_mask[k]);
    }
  }
}

void re_pattern_list::merge(const re_pattern& pattern)
{
  // Iterate all given patterns.
  for (re_pattern& p : list) {
    // Skip if RB allocation parameters do NOT match.
    if (p.rb_begin != pattern.rb_begin || p.rb_end != pattern.rb_end || p.rb_stride != pattern.rb_stride) {
      continue;
    }

    // Check if symbol mask matches.
    bool lmatch = std::equal(pattern.symbols.begin(), pattern.symbols.end(), p.symbols.begin(), p.symbols.end());

    // Check if RE mask matches.
    bool kmatch = std::equal(pattern.re_mask.begin(), pattern.re_mask.end(), p.re_mask.begin(), p.re_mask.end());

    // If OFDM symbols and subcarriers mask match, it means that the patterns are completely overlapped and no merging
    // is required.
    if (kmatch && lmatch) {
      return;
    }

    // If OFDM symbols mask matches, combine subcarrier mask.
    if (lmatch) {
      for (unsigned k = 0; k != NRE; ++k) {
        p.re_mask[k] |= pattern.re_mask[k];
      }
      return;
    }

    // If subcarriers mask matches, combine OFDM symbols mask.
    if (kmatch) {
      for (unsigned l = 0; l != MAX_NSYMB_PER_SLOT; ++l) {
        p.symbols[l] |= pattern.symbols[l];
      }
      return;
    }
  }

  // If reached here, no pattern was matched. Check if there is free space.
  srsran_assert(!list.full(), "RE pattern list is full. It seems %d maximum entries are not enough.", MAX_RE_PATTERN);

  // Append pattern.
  list.emplace_back(pattern);
}

void re_pattern_list::get_inclusion_mask(span<bool> mask, unsigned symbol) const
{
  // Iterate all given patterns.
  for (const re_pattern& p : list) {
    p.get_inclusion_mask(mask, symbol);
  }
}

void re_pattern_list::get_exclusion_mask(span<bool> mask, unsigned symbol) const
{
  // Iterate all given patterns.
  for (const re_pattern& p : list) {
    p.get_exclusion_mask(mask, symbol);
  }
}