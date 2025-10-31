/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/resource_grid.h"
#include "../config/cell_configuration.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/bounded_integer.h"
#include "srsran/adt/circular_array.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// \brief This class manages PUCCH resource collisions within a cell.
/// It keeps track of the usage of both common and dedicated resources for each slot.
class pucch_collision_manager
{
public:
  /// Maximum number of common PUCCH resources managed by the collision manager.
  static constexpr unsigned nof_common_res = pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES;
  /// Maximum number of PUCCH resources managed by the collision manager (common + dedicated).
  static constexpr unsigned max_nof_cell_resources = nof_common_res + pucch_constants::MAX_NOF_CELL_PUCCH_RESOURCES;

  /// Bounded integer representing a common PUCCH resource index (r_pucch).
  using r_pucch_t = bounded_integer<unsigned, 0, 15>;

  pucch_collision_manager(const cell_configuration& cell_cfg);

  void slot_indication(slot_point sl_tx);
  void stop();

  /// Checks if two dedicated PUCCH resources collide.
  bool check_ded_to_ded_collision(unsigned cell_res_id1, unsigned cell_res_id2) const;

  /// Checks if a common PUCCH resource collides with a dedicated PUCCH resource.
  bool check_common_to_ded_collision(r_pucch_t r_pucch, unsigned cell_res_id) const;

  /// Returns the number of multiplexing regions defined in the cell configuration.
  unsigned nof_mux_regions() const { return mux_matrix.size(); }

  /// \brief Checks if a common PUCCH resource can be allocated at a given slot.
  /// A resource can be allocated if it is not in use and it does not collide with any other resource already allocated
  /// at the same slot.
  bool can_alloc_common(slot_point sl, r_pucch_t r_pucch) const;

  /// \brief Checks if a dedicated PUCCH resource can be allocated at a given slot.
  /// A resource can be allocated if it is not in use and it does not collide with any other resource already allocated
  /// at the same slot.
  bool can_alloc_ded(slot_point sl, unsigned cell_res_id) const;

  /// Allocates a common PUCCH resource at a given slot.
  void alloc_common(slot_point sl, r_pucch_t r_pucch);

  /// Allocates a dedicated PUCCH resource at a given slot.
  void alloc_ded(slot_point sl, unsigned cell_res_id);

  /// Frees a common PUCCH resource at a given slot.
  void free_common(slot_point sl, r_pucch_t r_pucch);

  /// Frees a dedicated PUCCH resource at a given slot.
  void free_ded(slot_point sl, unsigned cell_res_id);

private:
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

  /// Computes the collision matrix for all PUCCH resources in the cell configuration.
  static collision_matrix_t compute_collisions(const cell_configuration& cell_cfg);

  /// Computes the multiplexing matrix for all PUCCH resources in the cell configuration.
  static mux_regions_matrix_t compute_mux_regions(const cell_configuration& cell_cfg);
};

} // namespace srsran
