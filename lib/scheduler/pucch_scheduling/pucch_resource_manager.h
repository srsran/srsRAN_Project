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
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/scheduler/resource_grid_util.h"

namespace srsran {

/// Containers used to return the output of the PUCCH resource manager.
struct pucch_harq_resource_alloc_record {
  /// Pointer to PUCCH resource configuration to be used.
  const pucch_resource* pucch_res;
  /// PUCCH resource indicator corresponding to the resource that will be used by the UE.
  unsigned pucch_res_indicator;
};

/// Defines the PUCCH resource usage.
enum class pucch_resource_usage { NOT_USED = 0, HARQ_SET_0, HARQ_SET_1, SR, CSI };

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
/// (v)   Indexing of the PUCCH F0/F1 and PUCCH F2/F3/F4 resources for HARQ-ACK reporting must be contiguous within the
///       F0/F1 group and with F2/F3/F4 group. However, the last PUCCH F0/F1 group resource's and the first PUCCH
///       F2/F3/F4 group resource's indices need not be contiguous. E.g., PUCCH F0/F1 indices (for HARQ-ACK reporting) =
///       {0, ..., 7}, and PUCCH F2/F3/F4 indices (for HARQ-ACK reporting) = {10, ..., 17}.
class pucch_resource_manager
{
public:
  pucch_resource_manager();

  /// Reset all resources to "unused".
  void slot_indication(slot_point slot_tx);

  void stop();

  /// Returns true if the common PUCCH resource indexed by r_pucch is available at the given slot.
  bool is_common_resource_available(slot_point sl, size_t r_pucch);

  /// Set the common PUCCH resource indexed by r_pucch at the given slot as currently "not available".
  void reserve_common_resource(slot_point sl, size_t r_pucch);

  /// \brief Returns the next PUCCH resource available to be used for HARQ-ACK (format 0 or 1).
  /// \remark The format of the resource to be reserved depends on how PUCCH resource set 0 is configured.
  /// \return If any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the PUCCH
  /// resource indicator corresponding to the PUCCH resource that will be used by the UE. If there are no PUCCH
  /// resources available, the pointer passed will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  pucch_harq_resource_alloc_record
  reserve_next_set_0_harq_res_available(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Returns the next PUCCH resource available to be used for HARQ-ACK (format 2, 3 or 4).
  /// \remark If SR and CSI multiplexing is enabled, this resource can be used for HARQ-ACK + SR and/or CSI.
  /// \return If any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the PUCCH
  /// resource indicator corresponding to the PUCCH resource that will be used by the UE. If there are no PUCCH
  /// resources available, the pointer passed will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  pucch_harq_resource_alloc_record
  reserve_next_set_1_harq_res_available(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Returns a specific PUCCH format 0 or 1 resource (identified by the res. indicator) to be used for HARQ-ACK.
  /// \remark The format of the resource to be reserved depends on how PUCCH resource set 0 is configured.
  /// \return If the specific PUCCH resource is available, it returns the pointer to the configuration. Else, it returns
  /// \c nullptr.
  const pucch_resource* reserve_set_0_res_by_res_indicator(slot_point          slot_harq,
                                                           rnti_t              crnti,
                                                           unsigned            res_indicator,
                                                           const pucch_config& pucch_cfg);

  /// \brief Returns a specific PUCCH format 2, 3 or 4 resource (identified by the res. indicator) to be used for
  /// HARQ-ACK.
  /// \remark If SR and CSI multiplexing is enabled, this resource can be used for HARQ-ACK + SR and/or CSI.
  /// \return If the specific PUCCH resource is available, it returns the pointer to the configuration. Else, it returns
  /// \c nullptr.
  const pucch_resource* reserve_set_1_res_by_res_indicator(slot_point          slot_harq,
                                                           rnti_t              crnti,
                                                           unsigned            res_indicator,
                                                           const pucch_config& pucch_cfg);

  /// \brief Returns the specific PUCCH resource config to be used for CSI, if available.
  /// \remark If SR multiplexing is enabled, this resource can be used for CSI + SR.
  /// \return If the specific PUCCH resource is available, it returns (i) the pointer to the configuration and (ii) the
  /// PUCCH resource indicator corresponding to the PUCCH resource that will be used by the UE. Else, the pointer passed
  /// will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  const pucch_resource*
  reserve_csi_resource(slot_point slot_harq, rnti_t crnti, const ue_cell_configuration& ue_cell_cfg);

  /// \brief Returns the specific PUCCH resource config to be used for SR, if available.
  /// \return If the specific PUCCH resource is available, it returns (i) the pointer to the configuration and (ii) the
  /// PUCCH resource indicator corresponding to the PUCCH resource that will be used by the UE. Else, the pointer passed
  /// will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  const pucch_resource* reserve_sr_res_available(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 0 or 1) resource from being allocated to a given UE.
  /// \remark The format of the resource to be released depends on how PUCCH resource set 0 is configured.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_harq_set_0_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 2, 3 or 4) resource from being allocated to a given UE.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_harq_set_1_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 0 or 1) resource used for SR from being allocated to a given UE.
  /// \param[in] slot_sr slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_sr_resource(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 2, 3 or 4) resource used for CSI from being allocated to a given UE.
  /// \param[in] slot_csi slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] ue_cell_cfg UE's dedicated cell configuration.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_csi_resource(slot_point slot_csi, rnti_t crnti, const ue_cell_configuration& ue_cell_cfg);

  /// \brief Reset the record of the PUCCH resources reserved to UE at the current slot.
  /// \remark This function doesn't release the resources, it only resets the record of which resources have been
  /// reserved.
  void reset_latest_reserved_res_tracker();

  /// \brief Adds a records that a given resource has been reserved for the current slot for a given UE.
  void set_new_resource_allocation(rnti_t rnti, pucch_resource_usage res_type);

  /// \brief Checks whether a given resource has been reserved for the current slot for a given UE.
  [[nodiscard]] bool is_resource_allocated(rnti_t rnti, pucch_resource_usage res_type) const;

  /// \brief Releases the resources that have been recorded as reserved for the current slot for a given UE.
  void cancel_last_ue_res_reservations(slot_point slot_tx, rnti_t crnti, const ue_cell_configuration& ue_cell_cfg);

private:
  /// Size of the ring buffer of pucch_resource_manager. This size sets a limit on how far in advance a PUCCH can be
  /// allocated.
  static const size_t RES_MANAGER_RING_BUFFER_SIZE =
      get_allocator_ring_size_gt_min(SCHEDULER_MAX_K0 + SCHEDULER_MAX_K1 + NTN_CELL_SPECIFIC_KOFFSET_MAX);

  // Tracks usage of PUCCH resources.
  struct resource_tracker {
    rnti_t               rnti;
    pucch_resource_usage resource_usage;
  };

  using pucch_res_record_array  = std::array<resource_tracker, pucch_constants::MAX_NOF_CELL_PUCCH_RESOURCES>;
  using common_res_record_array = std::array<bool, pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES>;

  // Record for the RNTI and PUCCH resource indicator used for a given resource at a given slot; this information is
  // used to keep track of which resources are available and which UE is using them. This information is preserved over
  // the slots.
  struct rnti_pucch_res_id_slot_record {
    common_res_record_array used_common_resources;
    pucch_res_record_array  ues_using_pucch_res;
  };

  // Keeps track of which PUCCH cell resources have been allocated to a UE at the current slot.
  // This info is needed to release some resources after the PUCCH multiplexing (by the PUCCH allocator), during which
  // resources can be reserved and then released, depending on the multiplexing algorithm.
  // The lifespan of this information lasts for a single UE PUCCH allocation.
  struct current_slot_ue_allocations {
    rnti_t rnti       = rnti_t::INVALID_RNTI;
    bool   harq_set_0 = false;
    bool   harq_set_1 = false;
    bool   csi        = false;
    bool   sr         = false;
  };

  // Returns the resource manager allocation record for a given slot.
  rnti_pucch_res_id_slot_record& get_slot_resource_counter(slot_point sl);

  // Helper functions that implement the public interface methods.
  pucch_harq_resource_alloc_record reserve_next_harq_res_available(slot_point          slot_harq,
                                                                   rnti_t              crnti,
                                                                   const pucch_config& pucch_cfg,
                                                                   pucch_res_set_idx   res_set_idx);

  const pucch_resource* reserve_harq_res_by_res_indicator(slot_point          slot_harq,
                                                          rnti_t              crnti,
                                                          unsigned            res_indicator,
                                                          const pucch_config& pucch_cfg,
                                                          pucch_res_set_idx   res_set_idx);

  bool release_harq_resource(slot_point          slot_harq,
                             rnti_t              crnti,
                             const pucch_config& pucch_cfg,
                             pucch_res_set_idx   res_set_idx);

  // Ring buffer of rnti_pucch_res_id_slot_record for PUCCH resources.
  std::array<rnti_pucch_res_id_slot_record, RES_MANAGER_RING_BUFFER_SIZE> resource_slots;

  current_slot_ue_allocations last_ue_allocations;

  // Keeps track of the last slot_point used by the resource manager.
  slot_point last_sl_ind;
};

} // namespace srsran
