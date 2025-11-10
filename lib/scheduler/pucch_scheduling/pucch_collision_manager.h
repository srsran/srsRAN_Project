/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../cell/resource_grid.h"
#include "../config/cell_configuration.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/bounded_integer.h"
#include "srsran/adt/circular_array.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/slot_point.h"
#include <optional>

namespace srsran {

namespace detail {

/// Represents the time-frequency grants of a PUCCH resource.
struct pucch_grants {
  /// Time-frequency grants of the first hop.
  grant_info first_hop;
  /// Time-frequency grant of the second hop (if intra-slot frequency hopping is enabled).
  std::optional<grant_info> second_hop;

  bool operator==(const pucch_grants& other) const
  {
    return first_hop == other.first_hop and second_hop == other.second_hop;
  }
  bool operator!=(const pucch_grants& other) const { return not(*this == other); }

  /// Checks if this pucch_grants overlaps with another pucch_grants.
  bool overlaps(const pucch_grants& other) const
  {
    // Check if the first grant overlaps with any of the other's grants.
    if (first_hop.overlaps(other.first_hop) or
        (other.second_hop.has_value() and first_hop.overlaps(*other.second_hop))) {
      return true;
    }
    // Check if the second grant (if any) overlaps with any of the other's grants.
    if (second_hop.has_value() and (second_hop->overlaps(other.first_hop) or
                                    (other.second_hop.has_value() and second_hop->overlaps(*other.second_hop)))) {
      return true;
    }
    return false;
  }
};

/// Represents the relevant information of a PUCCH resource for collision checking.
struct resource_info {
  /// PUCCH format of the resource.
  pucch_format format;
  /// Multiplexing index of the resource. Resources with different multiplexing indices are orthogonal and do not
  /// collide. It is computed from different parameters depending on the format:
  ///  - Format 0: initial cyclic shift.
  ///  - Format 1: initial cyclic shift, time domain OCC index.
  ///  - Format 2/3: not multiplexed (always 0).
  ///  - Format 4: OCC index.
  unsigned multiplexing_index;
  /// Time-frequency grants of the resource.
  pucch_grants grants;
};

} // namespace detail

/// \brief This class manages PUCCH resource collisions within a cell.
/// It keeps track of the usage of both common and dedicated resources for each slot.
class pucch_collision_manager
{
public:
  /// Bounded integer representing a common PUCCH resource index (r_pucch).
  using r_pucch_t = bounded_integer<unsigned, 0, 15>;

  pucch_collision_manager(const cell_configuration& cell_cfg);

  void slot_indication(slot_point sl_tx);
  void stop();

  /// Check if two dedicated PUCCH resources collide.
  bool check_ded_to_ded_collision(unsigned cell_res_id1, unsigned cell_res_id2) const;

  /// Check if a common PUCCH resource collides with a dedicated PUCCH resource.
  bool check_common_to_ded_collision(r_pucch_t r_pucch, unsigned cell_res_id) const;

  /// Return the number of multiplexing regions defined in the cell configuration.
  unsigned nof_mux_regions() const { return mux_matrix.size(); }

  /// Reasons for a PUCCH allocation failure.
  enum class alloc_failure_reason {
    PUCCH_COLLISION,
    UL_GRANT_COLLISION,
  };
  using alloc_result_t = error_type<alloc_failure_reason>;

  /// \brief Allocate a common PUCCH resource at a given slot.
  /// \return Success if the allocation was successful, otherwise an error indicating the reason of failure.
  alloc_result_t alloc_common(slot_point sl, r_pucch_t r_pucch);

  /// \brief Allocate a dedicated PUCCH resource at a given slot.
  /// \return Success if the allocation was successful, otherwise an error indicating the reason of failure.
  alloc_result_t alloc_ded(slot_point sl, unsigned cell_res_id);

  /// Free a common PUCCH resource at a given slot.
  void free_common(slot_point sl, r_pucch_t r_pucch);

  /// Free a dedicated PUCCH resource at a given slot.
  void free_ded(slot_point sl, unsigned cell_res_id);

private:
  /// Maximum number of common PUCCH resources managed by the collision manager.
  static constexpr unsigned nof_common_res = pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES;
  /// Maximum number of PUCCH resources managed by the collision manager (common + dedicated).
  static constexpr unsigned max_nof_cell_resources = nof_common_res + pucch_constants::MAX_NOF_CELL_PUCCH_RESOURCES;

  /// \brief List of all PUCCH resources (common + dedicated) in the cell configuration.
  using cell_resources_t = static_vector<detail::resource_info, max_nof_cell_resources>;
  /// \brief Collision matrix indicating which resources collide with each other.
  ///  - C[i][j] = 1 if resource i collides with resource j, 0 otherwise.
  using collision_matrix_t = static_vector<bounded_bitset<max_nof_cell_resources>, max_nof_cell_resources>;
  /// \brief Matrix of multiplexing regions indicating which resources can be multiplexed together.
  ///  - Each row represents a multiplexing region.
  ///  - M[i][j] = 1 if resource j belongs to multiplexing region i, 0 otherwise.
  /// \remark Multiplexing regions with only one resource (i.e., non-multiplexed resources) are not represented in this
  ///         matrix. Therefore, we can have a maximum of $max_nof_cell_resources / 2$ rows.
  using mux_regions_matrix_t = static_vector<bounded_bitset<max_nof_cell_resources>, max_nof_cell_resources / 2>;

  const cell_configuration&  cell_cfg;
  const cell_resources_t     resources;
  const collision_matrix_t   collision_matrix;
  const mux_regions_matrix_t mux_matrix;

  /// Allocation context for a specific slot.
  struct slot_context {
    /// Bitset representing the current usage state of all PUCCH resources (common and dedicated) in this slot.
    ///  - S[i] = 1 if resource i is in use, 0 otherwise.
    bounded_bitset<max_nof_cell_resources> current_state;
  };

  // Ring buffer of slot contexts to keep track of PUCCH resource usage in recent slots.
  circular_array<slot_context, cell_resource_allocator::RING_ALLOCATOR_SIZE> slots_ctx;

  // Keeps track of the last slot_point used by the resource manager.
  slot_point last_sl_ind;

  /// Compute the collision matrix for all PUCCH resources in the cell configuration.
  static cell_resources_t compute_resources(const cell_configuration& cell_cfg);

  /// Compute the collision matrix for all PUCCH resources in the cell configuration.
  static collision_matrix_t compute_collisions(span<const detail::resource_info> resources);

  /// Compute the multiplexing matrix for all PUCCH resources in the cell configuration.
  static mux_regions_matrix_t compute_mux_regions(span<const detail::resource_info> resources);

  /// Allocate the PUCCH resource at \ref res_idx at a given slot.
  alloc_result_t alloc_resource(slot_point sl, unsigned res_idx);

  /// Free the PUCCH resource at \ref res_idx at a given slot.
  void free_resource(slot_point sl, unsigned res_idx);
};

} // namespace srsran
