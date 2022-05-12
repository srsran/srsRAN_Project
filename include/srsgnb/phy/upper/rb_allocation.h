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

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/support/srsran_assert.h"
#include <array>

namespace srsgnb {

/// Defines the resource block allocation types.
enum class rb_allocation_type {
  /// Allocation Type0, valid for PDSCH and PUSCH.
  TYPE0 = 0,
  /// Allocation Type1, valid for PDSCH and PUSCH.
  TYPE1
};

/// Defines the Virtual Resource Block (VRB) to Physical Resource Block (PRB) mapping type.
enum class vrb_to_prb_mapping_type {
  /// Non interleaved VRB-to-PRB mapping, the VRB n is mapped to PRB n.
  NON_INTERLEAVED = 0,
  /// Interleaved VRB-to-PRB mapping with L=2.
  INTERLEAVED_L_2,
  /// Interleaved VRB-to-PRB mapping with L=4.
  INTERLEAVED_L_4
};

/// Describes a Resource Block allocation for PDSCH and PUSCH.
class rb_allocation
{
private:
  rb_allocation_type      allocation_type;
  vrb_to_prb_mapping_type mapping_type;

  /// Provides a mask that indicates what VRBs are used.
  std::array<bool, MAX_RB> vrb_mask;

  /// Indicates the starting resource block corresponding to VRB0 for allocation Type1.
  unsigned type1_vrb_start;

  /// Indicates the number of RB used for the transmission.
  unsigned rb_count;

  /// \brief Generates VRB-to-PRB interleaved indexes.
  ///
  /// This method generates the indexes to map Virtual Resource Blocks (VRBs) to Physical Resource Blocks (PRBs)
  /// according to the interleaved strategy, as defined in TS 38.211 section 7.3.1.5 Mapping to virtual resource blocks.
  ///
  /// \param[out] prb_indices Provides the destination resource block indices. The size provides the BWP size in
  /// resource blocks.
  /// \param[in] vrb_to_prb_mapping Indicates the VRB to PRB bundle size
  /// \param[in] bwp_start_rb Indicates the BWP start physical resource block index.
  static void generate_vrb_to_prb_indexes(span<unsigned>          prb_indices,
                                          vrb_to_prb_mapping_type vrb_to_prb_mapping,
                                          unsigned                bwp_start_rb);

public:
  /// Default constructor.
  rb_allocation() = default;

  /// \brief Build resource allocation from a resource block index list. This constructor is a Type0 mapping.
  ///
  /// \param[in] vrb_indexes Provides a list of Virtual Resource Blocks indexes.
  /// \param[in] mapping_type_ Indicates the mapping type.
  rb_allocation(std::initializer_list<const unsigned> vrb_indexes, vrb_to_prb_mapping_type mapping_type_) :
    allocation_type(rb_allocation_type::TYPE0),
    mapping_type(mapping_type_),
    type1_vrb_start(0),
    rb_count(vrb_indexes.size())
  {
    // Reset VRB mask.
    std::fill(vrb_mask.begin(), vrb_mask.end(), false);

    // For each VRB index, setup mask.
    for (const unsigned& rb_idx : vrb_indexes) {
      vrb_mask[rb_idx] = true;
    }
  }

  /// \brief Build resource allocation from a start and size parameter. This constructor is a Type1 mapping.
  ///
  /// \param[in] vrb_start Indicates the starting resource block corresponds to VRB0.
  /// \param[in] vrb_size Indicates the number of RB.
  /// \param[in] mapping_type_ Indicates the mapping type.
  rb_allocation(unsigned vrb_start, unsigned vrb_size, vrb_to_prb_mapping_type mapping_type_) :
    allocation_type(rb_allocation_type::TYPE1),
    mapping_type(mapping_type_),
    type1_vrb_start(vrb_start),
    rb_count(vrb_size)
  {
    // Reset VRB mask before below VRB start.
    std::fill(vrb_mask.begin(), vrb_mask.begin() + vrb_start, false);

    // Reset VRB mask from VRB start to the end of the allocaton.
    std::fill(vrb_mask.begin() + vrb_start, vrb_mask.begin() + vrb_start + rb_count, true);

    // Reset VRB mask after the VRB allocation.
    std::fill(vrb_mask.begin() + vrb_start + rb_count, vrb_mask.end(), false);
  }

  /// \brief Build resource allocation from a packed mask. This constructor is a Type0 mapping.
  ///
  /// \param[in] packed_mask Indicates the starting resource block corresponding to VRB0.
  /// \param[in] mapping_type_ Indicates the mapping type.
  rb_allocation(span<const uint8_t> packed_mask, vrb_to_prb_mapping_type mapping_type_) :
    allocation_type(rb_allocation_type::TYPE0), mapping_type(mapping_type_), type1_vrb_start(0), rb_count(0)
  {
    // Unpack the mask and count the number of RB
    for (unsigned rb_idx = 0; rb_idx < MAX_RB; ++rb_idx) {
      // Extract RB mask.
      uint8_t mask = (packed_mask[rb_idx / 8] >> (rb_idx % 8)) & 1U;

      // Setup mask.
      vrb_mask[rb_idx] = (mask != 0);

      // Count active RB.
      rb_count += mask;
    }
  }

  /// Get the number of allocated VRB.
  unsigned get_nof_rb() const { return rb_count; }

  /// \brief Get if the allocation is Type1 and non-interleaved.
  ///
  /// \return True if the frequency allocation is Type1 and VRB-to-PRB mapping is non-interleaved, otherwise false.
  bool is_type1_non_interleaved() const
  {
    return allocation_type == rb_allocation_type::TYPE1 && mapping_type == vrb_to_prb_mapping_type::NON_INTERLEAVED;
  }

  ///  \brief Get the Type1 mapping for non-interleaved VRB-PRB mapping as start and size.
  ///
  /// \param[in] bwp_start_rb Indicates the BWP start physical resource block index.
  /// \param[in] bwp_size_rb Indicates the BWP size in resource blocks.
  /// \return A pair of integer values. The first one is the starting PRB and the second one is the number of PRBs.
  std::pair<unsigned, unsigned> get_allocation_type_1_non_interleaved(unsigned bwp_start_rb, unsigned bwp_size_rb) const
  {
    // Determines the RBs that are outside the BWP.
    unsigned exceed_rb = 0;
    if (bwp_start_rb + bwp_size_rb < type1_vrb_start + rb_count) {
      exceed_rb = (type1_vrb_start + rb_count) - bwp_start_rb + bwp_size_rb;
    }

    // Calculate start and size.
    return {bwp_start_rb + type1_vrb_start, rb_count - exceed_rb};
  }

  /// \brief Calculates a PDSCH or PUSCH frequency allocation mask from the configured VRB allocation.
  ///
  /// \param[out] mask Provides the destination of the allocation mask. The size of the mask determines the BWP size.
  /// \param[in] bwp_start_rb Indicates the BWP start physical resource block index.
  /// \param[in] bwp_size_rb Indicates the BWP size in resource blocks.
  /// \note The mask size shall be greater than the start plus the size of the BWP.
  void get_allocation_mask(span<bool> mask, unsigned bwp_start_rb, unsigned bwp_size_rb) const;

  /// \brief Calculates a PDSCH or PUSCH allocation PRB indices from the configured VRB allocation and the BWP start.
  ///
  /// \param[out] prb_indices Provides the destination of the PRB indices.
  /// \param[in] bwp_start_rb Indicates the BWP start physical resource block index.
  /// \param[in] bwp_size_rb Indicates the BWP size in resource blocks.
  /// \note The number of PRB indices shall be equal to the number of allocated VRB.
  void get_allocation_indices(span<unsigned> prb_indices, unsigned bwp_start_rb, unsigned bwp_size_rb) const;
};

} // namespace srsgnb
#endif // SRSGNB_RB_ALLOCATION_H
