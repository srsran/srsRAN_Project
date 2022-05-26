/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RB_ALLOCATION_H
#define SRSGNB_RB_ALLOCATION_H

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

/// Defines the Virtual Resource Block (VRB) to Physical Resource Block (PRB) mapping type.
enum class vrb_to_prb_mapping_type {
  /// Non interleaved VRB-to-PRB mapping, the VRB n is mapped to PRB n.
  NON_INTERLEAVED = 0,
  /// Interleaved VRB-to-PRB mapping with L=2.
  INTERLEAVED_L_2,
  /// Interleaved VRB-to-PRB mapping with L=4.
  INTERLEAVED_L_4
};

// TBD.
class rb_allocation
{
private:
  // TBD.
  vrb_to_prb_mapping_type mapping_type;
  // TBD.
  bounded_bitset<MAX_RB> vrb_mask;
  // TBD.
  static void generate_vrb_to_prb_indexes(span<unsigned>          prb_indices,
                                          vrb_to_prb_mapping_type vrb_to_prb_mapping,
                                          unsigned                bwp_start_rb);

public:
  // TBD.
  rb_allocation() = default;

  // TBD.
  rb_allocation(std::initializer_list<const unsigned> vrb_indexes, vrb_to_prb_mapping_type mapping_type_)
  {
    unsigned max_vrb_index = *std::max_element(vrb_indexes.begin(), vrb_indexes.end());
    vrb_mask.resize(max_vrb_index + 1);

    // For each VRB index, setup mask.
    for (unsigned vrb_index : vrb_indexes) {
      vrb_mask.set(vrb_index, true);
    }
  }

  // TBD.
  rb_allocation(unsigned vrb_start, unsigned vrb_size, vrb_to_prb_mapping_type mapping_type_) :
    mapping_type(mapping_type_)
  {
    vrb_mask.resize(vrb_start + vrb_size);
    vrb_mask.fill(vrb_start, vrb_start + vrb_size);
  }

  // TBD.
  rb_allocation(span<const uint8_t> packed_mask, vrb_to_prb_mapping_type mapping_type_) : mapping_type(mapping_type_)
  {
    // Unpack the mask and count the number of RB
    for (unsigned rb_idx = 0; rb_idx < MAX_RB; ++rb_idx) {
      // Extract RB mask.
      uint8_t mask = (packed_mask[rb_idx / 8] >> (rb_idx % 8)) & 1U;

      // Setup mask.
      vrb_mask.set(rb_idx, (mask != 0));
    }
  }

  // TBD.
  bool operator==(const rb_allocation& other) const { return vrb_mask == other.vrb_mask; }

  // TBD.
  unsigned get_nof_rb() const { return vrb_mask.count(); }

  // TBD.
  unsigned get_prb_begin(unsigned bwp_start_rb) const
  {
    int vrb_first = vrb_mask.find_lowest();
    if (vrb_first == -1) {
      return 0;
    }

    return static_cast<unsigned>(vrb_first) + bwp_start_rb;
  }

  // TBD.
  unsigned get_prb_end(unsigned bwp_start_rb) const
  {
    int vrb_last = vrb_mask.find_highest();
    if (vrb_last == -1) {
      return 0;
    }

    return static_cast<unsigned>(vrb_last + 1) + bwp_start_rb;
  }

  // TBD.
  bool is_contiguous() const { return vrb_mask.is_contiguous(); }

  // TBD.
  bounded_bitset<MAX_RB> get_prb_mask(unsigned bwp_start_rb, unsigned bwp_size_rb) const;

  // TBD.
  static_vector<uint16_t, MAX_RB> get_prb_indices(unsigned bwp_start_rb, unsigned bwp_size_rb) const;
};

} // namespace srsgnb
#endif // SRSGNB_RB_ALLOCATION_H
