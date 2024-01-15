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

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/upper/vrb_to_prb_mapper.h"

namespace srsran {

/// \brief Describes the frequency allocation for PDSCH and PUSCH  transmissions.
///
/// The frequency allocation is defined at VRB level and it can be of type 0, type 1 or custom. Frequency allocation
/// Types 0 and 1 are described in TS38.214 Section 5.1.2.2 for PDSCH and Section 6.1.2.2 for PUSCH.
///
/// The VRB allocation is made using one of the maker methods:
/// - make_type0() for making a type0 allocation,
/// - make_type1() for making a type1 allocation, or
/// - make_custom() for making a custom allocation.
///
/// Each of the makers optionally take a VRB-to-PRB mapper for PDSCH transmissions. If the
/// VRB-to-PRB mapper is not provided, the VRB-to-PRB is defaulted. The default VRB-to-PRB mapper corresponds to
/// non-interleaved and to a PDSCH transmission is not scheduled in a common SS.
///
/// \see vrb_to_prb_mapper for more information regarding the VRB-to-PRB mapping for PDSCH.
///
/// \remarks
/// - The default constructor rb_allocation() does not allocate any VRB.
/// - Use is_contiguous() to determine whether the resultant allocation on the resource grid is contiguous.
/// - Use get_prb_indices() to generate the list of PRB indices.
/// - If the resultant physical allocation is contiguous, the methods get_prb_begin() and get_prb_end() can be used to
/// determine the first and last PRB indices without requiring to generate a list of PRB indices.
/// - Independently of the allocation type and VRB-to-PRB mapping, the method get_prb_mask() generates a mask indicating
/// the PRB used for the transmission.
class rb_allocation
{
private:
  /// Indicates, with a mask, the VRBs selected for the transmission.
  bounded_bitset<MAX_RB> vrb_mask;
  /// VRB-to-PRB mapper.
  vrb_to_prb_mapper vrb_to_prb_map;

  /// \brief Computes the PRB allocation mask if the resultant allocation is contiguous and non-interleaved.
  ///
  /// \param[in] bwp_start_rb Indicates the BWP start PRB index relative to CRB0 (PointA).
  /// \param[in] bwp_size_rb Indicates the BWP size in PRB units.
  bounded_bitset<MAX_RB> get_contiguous_prb_mask(unsigned bwp_start_rb, unsigned bwp_size_rb) const;

  /// \brief Computes the PRB allocation mask for the cases not covered by get_contiguous_prb_mask().
  ///
  /// \param[in] bwp_start_rb Indicates the BWP start PRB index relative to CRB0 (PointA).
  /// \param[in] bwp_size_rb Indicates the BWP size in PRB units.
  bounded_bitset<MAX_RB> get_other_prb_mask(unsigned bwp_start_rb, unsigned bwp_size_rb) const;

public:
  /// \brief Default constructor.
  ///
  /// It constructs an empty allocation with default VRB-to-PRB mapping.
  rb_allocation() = default;

  /// \brief Creates a Type 0 RB allocation object.
  ///
  /// Type 0 allocation is described in TS38.214 Section 5.1.2.2.1 for PDSCH and Section 6.1.2.2.1 for PUSCH.
  ///
  /// \param[in] vrb_bitmap VRB allocation bitmap in which each entry represents an active RB.
  /// \param[in] vrb_to_prb_map_ Optional VRB-to-PRB mapping (for PDSCH only).
  /// \return An RB allocation instance.
  static rb_allocation make_type0(const bounded_bitset<MAX_RB>&      vrb_bitmap,
                                  const optional<vrb_to_prb_mapper>& vrb_to_prb_map_ = {});

  /// \brief Creates a Type 1 RB allocation object.
  ///
  /// Type 1 allocation is described in TS38.214 Section 5.1.2.2.2 for PDSCH and Section 6.1.2.2.2 for PUSCH.
  ///
  /// \param[in] rb_start Indicates the lowest VRB used for this transmission as \f$RB_{start}\f$.
  /// \param[in] rb_count Indicates the number of consecutive active VRB for this transmission as \f$L_{RB_s}\f$.
  /// \param[in] vrb_to_prb_map_ Optional VRB-to-PRB mapping (for PDSCH only).
  /// \return An RB allocation instance.
  static rb_allocation
  make_type1(unsigned rb_start, unsigned rb_count, const optional<vrb_to_prb_mapper>& vrb_to_prb_map_ = {});

  /// \brief Creates a custom allocation object using a list of VRB indexes.
  ///
  /// The custom allocation is indexed for testing and it is not described in the TS. The VRB mask is constructed from
  /// an integer initializer list that indicates the VRBs used for the PDSCH or PUSCH transmission.
  ///
  /// The index list order is irrelevant and the index zero represents VRB0.
  ///
  /// \param[in] vrb_indexes Provides the VRB indexes.
  /// \param[in] vrb_to_prb_map_ Optional VRB-to-PRB mapping (for PDSCH only).
  /// \return An RB allocation instance.
  static rb_allocation make_custom(std::initializer_list<const unsigned> vrb_indexes,
                                   const optional<vrb_to_prb_mapper>&    vrb_to_prb_map_ = {});

  /// \brief Compares two frequency domain allocations.
  ///
  /// Two allocations are considered equal if:
  /// - their VRB masks are equal, and
  /// - the VRB-to-PRB mapping is equal.
  bool operator==(const rb_allocation& other) const
  {
    return (vrb_mask == other.vrb_mask) && (vrb_to_prb_map == other.vrb_to_prb_map);
  }

  /// \brief Determines if resultant VRB-to-PRB allocation is contiguous in frequency domain.
  ///
  /// The PRB allocation is contiguous if:
  /// - the VRB allocated for the transmission are contiguous, and
  /// - the VRB-to-PRB mapping is not interleaved.
  ///
  /// \return True if the PRB allocation is contiguous. Otherwise, false.
  bool is_contiguous() const { return vrb_mask.is_contiguous() && !vrb_to_prb_map.is_interleaved(); }

  /// \brief Gets the lowest allocated PRB index.
  /// \param[in] bwp_start_rb Indicates the BWP lowest PRB index relative to CRB0.
  /// \return A PRB index relative to CRB0 (PointA) if there are active VRB. Otherwise, zero.
  unsigned get_prb_begin(unsigned bwp_start_rb) const
  {
    int first_vrb = vrb_mask.find_lowest(0, vrb_mask.size());
    if (first_vrb == -1) {
      return 0;
    }

    return static_cast<unsigned>(first_vrb) + bwp_start_rb + vrb_to_prb_map.get_coreset_start();
  }

  /// \brief Gets the highest allocated PRB index plus one.
  /// \param[in] bwp_start_rb Indicates the BWP lowest PRB index relative to CRB0.
  /// \return A non-zero PRB index relative to CRB0 (PointA) if there are active VRB. Otherwise, zero.
  unsigned get_prb_end(unsigned bwp_start_rb) const
  {
    int last_vrb = vrb_mask.find_highest(0, vrb_mask.size());
    if (last_vrb == -1) {
      return 0;
    }

    return static_cast<unsigned>(last_vrb + 1) + bwp_start_rb + vrb_to_prb_map.get_coreset_start();
  }

  /// Gets the number of allocated VRB.
  unsigned get_nof_rb() const { return vrb_mask.count(); }

  /// \brief Determines whether the resource block allocation is valid for a given bandwidth part.
  ///
  /// The resource block allocation combined with the BWP parameters are valid if:
  /// - the sum of the BWP start and size does not exceed \ref MAX_RB; and
  /// - the size of the virtual resource block allocation does not exceed the size of the available physical resources.
  ///
  /// For PDSCH transmissions scheduled by DCI in a common SS, the PRBs used for VRB to PRB mapping start from the
  /// lowest numbered resource block in the CORESET. For all other allocations, the available physical resources for VRB
  /// to PRB mapping comprise the entire BWP size.
  ///
  /// \param[in] bwp_start_rb Lowest PRB index of the BWP relative to CRB0 (PointA), i.e. \f$N_{BWP,i}^{start}\f$.
  /// \param[in] bwp_size_rb  BWP size in PRB, i.e. \f$N_{BWP,i}^{size}\f$.
  /// \return True if the combination of BWP and resource block is valid, false otherwise.
  bool is_bwp_valid(unsigned bwp_start_rb, unsigned bwp_size_rb) const
  {
    return ((bwp_start_rb + bwp_size_rb <= MAX_RB) &&
            (vrb_mask.size() + vrb_to_prb_map.get_coreset_start() <= bwp_size_rb));
  }

  /// \brief Generates the PRB allocation mask for the frequency domain allocation.
  ///
  /// Calculates the PRB allocation mask considering the BWP frequency allocation, the VRB mask and the VRB-to-PRB
  /// mapping.
  ///
  /// The resultant mask is represented in a \c bounded_bitset<MAX_RB> of size \f$N_{BWP,i}^{start}+N_{BWP,i}^{size}\f$
  /// in which every set bit represents an active PRB. The first bit of the mask belongs to CRB0 (PointA).
  ///
  /// \param[in] bwp_start_rb Indicates the BWP lowest PRB index relative to CRB0 (PointA) as \f$N_{BWP,i}^{start}\f$.
  /// \param[in] bwp_size_rb Indicates the BWP size in PRB as \f$N_{BWP,i}^{size}\f$.
  /// \return The PRB allocation mask represented as a bounded bitset.
  bounded_bitset<MAX_RB> get_prb_mask(unsigned bwp_start_rb, unsigned bwp_size_rb) const;

  /// \brief Generates the PRB allocation indexes for the frequency domain allocation.
  ///
  /// Calculates the PRB allocation indexes considering the BWP frequency allocation, the VRB mask and the VRB-to-PRB
  /// mapping.
  ///
  /// The resultant indices are represented in a \c static_vector<uint16_t,MAX_RB> of size
  /// \f$N_{BWP,i}^{start}+N_{BWP,i}^{size}\f$. The indices are relative to CRB0 (PointA) and they are indexed in VRB
  /// increasing order. In other words, the first index indicates the PRB index of the lowest active VRB.
  /// Correspondingly, the last index corresponds to the highest active VRB.
  ///
  /// This method is intended for non-contiguous and/or interleaved allocation. For contiguous non-interleaved
  /// transmission use \c get_prb_begin() and \c get_prb_end().
  ///
  /// \param[in] bwp_start_rb Indicates the BWP lowest PRB index relative to CRB0 (PointA) as \f$N_{BWP,i}^{start}\f$.
  /// \param[in] bwp_size_rb Indicates the BWP size in PRB as \f$N_{BWP,i}^{size}\f$.
  /// \return The resultant PRB allocation indices represented as a static vector.
  static_vector<uint16_t, MAX_RB> get_prb_indices(unsigned bwp_start_rb, unsigned bwp_size_rb) const;
};

} // namespace srsran

namespace fmt {

/// \brief Custom formatter for pusch_processor::rb_allocation.
template <>
struct formatter<srsran::rb_allocation> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rb_allocation& rb_alloc, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (rb_alloc.is_contiguous()) {
      return format_to(
          ctx.out(), "[{}, {})", rb_alloc.get_prb_begin(0), rb_alloc.get_prb_begin(0) + rb_alloc.get_nof_rb());
    }

    return format_to(ctx.out(), "non-contiguous");
  }
};
} // namespace fmt
