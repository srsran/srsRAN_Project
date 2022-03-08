#include "srsgnb/phy/upper/re_pattern.h"

using namespace srsgnb;

/// Helper macro to assert the validity of the pattern keeping the line usage
#define re_pattern_assert()                                                                                            \
  do {                                                                                                                 \
    srsran_assert(rb_begin < MAX_RB, "RB begin (%d) is out-of-range", rb_begin);                                       \
    srsran_assert(rb_end > 1 && rb_end <= MAX_RB, "RB end (%d) is out-of-range", rb_end);                              \
    srsran_assert(rb_stride > 0, "RB stride (%d) is out-of-range", rb_stride);                                         \
  } while (false)

void re_pattern::include_mask(span<bool> mask, unsigned int symbol) const
{
  re_pattern_assert();
  srsran_assert(
      mask.size() >= rb_end, "Provided mask size (%d) is too small. The minimum is %d.", (unsigned)mask.size(), rb_end);

  // Skip if the symbol is not used
  if (symbol_mask[symbol]) {
    return;
  }

  // For each resource block within the pattern.
  for (unsigned rb = rb_begin; rb < rb_end; rb += rb_stride) {
    // For each subcarrier in the resource block, apply a logical OR
    for (unsigned k = 0; k != NRE; ++k) {
      mask[k + rb * NRE] |= re_mask[k];
    }
  }
}

void re_pattern::exclude_mask(span<bool> mask, unsigned int symbol) const
{
  re_pattern_assert();
  srsran_assert(
      mask.size() >= rb_end, "Provided mask size (%d) is too small. The minimum is %d.", (unsigned)mask.size(), rb_end);

  // Skip if the symbol is not used
  if (symbol_mask[symbol]) {
    return;
  }

  // For each resource block within the pattern.
  for (unsigned rb = rb_begin; rb < rb_end; rb += rb_stride) {
    // For each subcarrier in the resource block, apply a logical OR
    for (unsigned k = 0; k != NRE; ++k) {
      mask[k + rb * NRE] &= (!re_mask[k]);
    }
  }
}