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

#include "../config/ue_configuration.h"
#include "pucch_collision_manager.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/resource_grid_util.h"
#include <array>
#include <optional>

namespace srsran {

/// Container used to return the result of a PUCCH HARQ resource allocation request.
struct pucch_harq_resource_alloc_record {
  /// Pointer to PUCCH resource configuration to be used.
  const pucch_resource* resource;
  /// PUCCH resource indicator of the resource to be used.
  uint8_t pucch_res_indicator;
};

/// \brief Class that manages the cell allocation of PUCCH resources across UEs.
/// The correct functioning of pucch_resource_manager is based on the following assumptions:
/// (i)   Each UE has max 8 PUCCH F0/F1 and max 8 PUCCH F2/F3/F4 dedicated to HARQ-ACK reporting.
/// (ii)  Each UE has max 1 SR-dedicated PUCCH F0/F1 resource and max 1 CSI-dedicated PUCCH F2/F3/F4 resource.
/// (iii) The cell PUCCH resource list can have max \c MAX_NOF_CELL_PUCCH_RESOURCES PUCCH resource, including all
///       formats; at cell level, there is no constraint on how many resource must be F0/F1, F2/F3/F4, or for SR or for
///       CSI.
/// (iv)  UEs can have different PUCCH resource lists; however the PUCCH resource ID is unique within the cell. This
///       implies that if two UEs have the same PUCCH resource within their lists, their PUCCH resource ID must be the
///       same.
class pucch_resource_manager
{
public:
  pucch_resource_manager(const cell_configuration& cell_cfg_);

  /// Reset all resources to "unused".
  void slot_indication(slot_point slot_tx);

  void stop();

  /// Returns true if the common PUCCH resource indexed by r_pucch is available at the given slot.
  bool is_harq_common_resource_available(slot_point sl, size_t r_pucch);

  /// Set the common PUCCH resource indexed by r_pucch at the given slot as currently "not available".
  void reserve_harq_common_resource(slot_point sl, size_t r_pucch);

  /// \brief RAII helper class that manages the reservation of dedicated PUCCH resources for a given UE at a given slot.
  /// The reservation is temporary until \c commit() is called, which makes the reservation permanent.
  /// \remark If \c commit() is not called before the destructor is invoked, all reservations are cancelled.
  class ue_reservation_guard
  {
  public:
    /// \brief RAII helper class that manages the reservation of dedicated PUCCH resources for a given UE.
    /// The reservation is temporary until \c commit() is called, which makes the reservation permanent.
    /// \remark If \c commit() is not called before the destructor is invoked, all reservations are cancelled.
    ue_reservation_guard(pucch_resource_manager*      parent_,
                         rnti_t                       rnti_,
                         slot_point                   sl_,
                         const ue_cell_configuration& ue_cfg_);
    // Disable copy and move semantics.
    ue_reservation_guard(const ue_reservation_guard&)                = delete;
    ue_reservation_guard& operator=(const ue_reservation_guard&)     = delete;
    ue_reservation_guard(ue_reservation_guard&&) noexcept            = delete;
    ue_reservation_guard& operator=(ue_reservation_guard&&) noexcept = delete;
    ~ue_reservation_guard();

    rnti_t                       get_rnti() const { return rnti; }
    slot_point                   get_slot() const { return sl; }
    const ue_cell_configuration& get_ue_cfg() const { return ue_cfg; }

    /// \brief Reserve the next available PUCCH resource in PUCCH Resource Set ID 0.
    /// \return If there is any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the
    /// PUCCH resource indicator of the resource that will be used by the UE. Otherwise, the pointer will be \c nullptr,
    /// and \c d_pri is to be ignored.
    pucch_harq_resource_alloc_record reserve_harq_set_0_resource_next_available();

    /// \brief Reserve the next available PUCCH resource in PUCCH Resource Set ID 1.
    /// \remark If SR and CSI multiplexing is enabled, this resource can be used for HARQ-ACK + SR and/or CSI.
    /// \return If there is any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the
    /// PUCCH resource indicator of the resource that will be used by the UE. Otherwise, the pointer will be \c nullptr,
    /// and \c d_pri is to be ignored.
    pucch_harq_resource_alloc_record reserve_harq_set_1_resource_next_available();

    /// \brief Reserve a specific PUCCH Resource Set ID 0 resource by its PUCCH resource indicator.
    /// \return A pointer to the resource configuration, if available. Otherwise, \c nullptr.
    const pucch_resource* reserve_harq_set_0_resource_by_res_indicator(unsigned d_pri);

    /// \brief Reserve a specific PUCCH Resource Set ID 1 resource by its PUCCH resource indicator.
    /// \remark If SR and CSI multiplexing is enabled, this resource can be used for HARQ-ACK + SR and/or CSI.
    /// \return A pointer to the resource configuration, if available. Otherwise, \c nullptr.
    const pucch_resource* reserve_harq_set_1_resource_by_res_indicator(unsigned d_pri);

    /// \brief Reserve the UE's SR PUCCH resource, if available.
    /// \return A pointer to the resource configuration, if available. Otherwise, \c nullptr.
    const pucch_resource* reserve_sr_resource();

    /// \brief Reserve the UE's CSI PUCCH resource, if available.
    /// \remark If SR multiplexing is enabled, this resource can be used for CSI + SR.
    /// \return A pointer to the resource configuration, if available. Otherwise, \c nullptr.
    const pucch_resource* reserve_csi_resource();

    /// Peek at the configured SR PUCCH resource for the UE without reserving it.
    const pucch_resource* peek_sr_resource() const;

    /// Peek at the configured CSI PUCCH resource for the UE without reserving it.
    const pucch_resource* peek_csi_resource() const;

    /// \brief Release PUCCH resource from Resource Set ID 0 from being allocated to a given UE.
    /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
    /// \return True if the resource for the UE was found in the allocation records for the given slot.
    bool release_harq_set_0_resource();

    /// \brief Release PUCCH resource from Resource Set ID 1 from being allocated to a given UE.
    /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
    /// \return True if the resource for the UE was found in the allocation records for the given slot.
    bool release_harq_set_1_resource();

    /// \brief Release SR PUCCH resource from being allocated to a given UE.
    /// \param[in] slot_sr slot for which the PUCCH resource was scheduled.
    /// \return True if the resource for the UE was found in the allocation records for the given slot.
    bool release_sr_resource();

    /// \brief Release CSI PUCCH resource from being allocated to a given UE.
    /// \return True if the resource for the UE was found in the allocation records for the given slot.
    bool release_csi_resource();

    /// \brief Commit all the reservations made so far.
    void commit();

    /// \brief Rollback all the reservations made so far.
    void rollback();

  private:
    /// Defines the PUCCH resource usage.
    enum class resource_usage_type { harq_set_0, harq_set_1, sr, csi, nof_usage_types };

    struct reservation {
      std::optional<unsigned> cell_res_id;
    };
    pucch_resource_manager*      parent;
    const rnti_t                 rnti;
    const slot_point             sl;
    const ue_cell_configuration& ue_cfg;

    // Tracks the reservations made for the UE before commit().
    // Does not include reservations made prior to the creation of this guard.
    std::array<reservation, static_cast<size_t>(resource_usage_type::nof_usage_types)> reservations;

    // Helper functions that implement the public interface methods.
    pucch_harq_resource_alloc_record reserve_next_harq_res_available(pucch_res_set_idx res_set_idx);

    const pucch_resource* reserve_harq_resource_by_res_indicator(unsigned d_pri, pucch_res_set_idx res_set_idx);

    bool release_harq_resource(pucch_res_set_idx res_set_idx);
  };

private:
  friend class ue_reservation_guard;

  /// Size of the ring buffer of pucch_resource_manager. This size sets a limit on how far in advance a PUCCH can be
  /// allocated.
  static const size_t RES_MANAGER_RING_BUFFER_SIZE =
      get_allocator_ring_size_gt_min(SCHEDULER_MAX_K0 + SCHEDULER_MAX_K1 + NTN_CELL_SPECIFIC_KOFFSET_MAX);

  // Record for the RNTI and PUCCH resource indicator used for a given resource at a given slot; this information is
  // used to keep track of which resources are available and which UE is using them. This information is preserved over
  // the slots.
  struct slot_context {
    static_vector<rnti_t, pucch_constants::MAX_NOF_CELL_PUCCH_RESOURCES> ues_using_pucch_res;

    /// Find the index of the first PUCCH resource used by a given UE among a list of resources to check.
    std::optional<unsigned> find_res_used_by_ue(span<const pucch_res_id_t> res_to_check, rnti_t rnti) const
    {
      for (unsigned r_pucch = 0; r_pucch != res_to_check.size(); ++r_pucch) {
        const unsigned cell_res_id = res_to_check[r_pucch].cell_res_id;
        srsran_assert(cell_res_id < ues_using_pucch_res.size(),
                      "PUCCH resource index from PUCCH resource set exceeds the size of the cell resource array");
        if (ues_using_pucch_res[cell_res_id] == rnti) {
          return r_pucch;
        }
      }
      return std::nullopt;
    }
  };

  const cell_configuration& cell_cfg;
  pucch_collision_manager   collision_manager;

  // Ring buffer of slot contexts to keep track of PUCCH resource usage in recent slots.
  circular_array<slot_context, cell_resource_allocator::RING_ALLOCATOR_SIZE> slots_ctx;

  // Keeps track of the last slot_point used by the resource manager.
  slot_point last_sl_ind;
};

} // namespace srsran
