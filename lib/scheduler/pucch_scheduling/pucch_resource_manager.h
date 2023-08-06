/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../ue_scheduling/ue_configuration.h"
#include "pucch_allocator.h"

namespace srsran {

/// Containers used to return the output of the PUCCH resource manager.
struct pucch_harq_resource_alloc_record {
  /// Pointer to PUCCH resource configuration to be used.
  const pucch_resource* pucch_res;
  /// PUCCH resource indicator corresponding to the resource that will be used by the UE.
  unsigned pucch_res_indicator;
};

/// \brief Class that manages the cell allocation of PUCCH resources across UEs.
/// The correct functioning of pucch_resource_manager is based on the following assumptions:
/// (i)   Each UE has max 8 PUCCH F1 and max 8 PUCCH F2 dedicated to HARQ-ACK reporting.
/// (ii)  Each UE has max 1 SR-dedicated PUCCH F1 resource and max 1 CSI-dedicated PUCCH F2 resource.
/// (iii) The cell PUCCH resource list can have max 128 PUCCH resource, including all formats; at cell level, there is
///       no constraint on how many resource must be F1, F2, or for SR or for CSI.
/// (vi)  UEs can have different PUCCH resource lists; however the PUCCH resource ID is unique with the cell. This
///       implies that if two UEs have the same PUCCH resource within their lists, their PUCCH resource ID must be the
///       same.
/// (v)   Indexing of the PUCCH F1 and PUCCH F2 resources for HARQ-ACK reporting must be contiguous within the F1 group
///       and with F2 group. However, the last PUCCH F1 group resource's and the first PUCCH F2 group resource's indices
///       need not be contiguous. E.g., PUCCH F1 indices (for HARQ-ACK reporting) = {0, ..., 7}, and PUCCH F2 indices
///       (for HARQ-ACK reporting) = {10, ..., 17}.
class pucch_resource_manager
{
public:
  pucch_resource_manager();

  /// Reset all resources to "unused".
  void slot_indication(slot_point slot_tx);

  /// Returns true if the common PUCCH resource indexed by r_pucch is available at the given slot.
  bool is_common_resource_available(slot_point sl, size_t r_pucch);

  /// Set the common PUCCH resource indexed by r_pucch at the given slot as currently "not available".
  void reserve_common_resource(slot_point sl, size_t r_pucch);

  /// \brief Returns the next PUCCH resource available to be used for HARQ-ACK (format 1).
  /// \return If any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the PUCCH
  /// resource indicator corresponding to the PUCCH resource that will be used by the UE. If there are no PUCCH
  /// resources available, the pointer passed will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  pucch_harq_resource_alloc_record
  reserve_next_f1_harq_res_available(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Returns the next PUCCH format 2 resource available to be used for HARQ-ACK.
  /// \remark If SR and CSI multiplexing is enabled, this resource can be used for HARQ-ACK + SR and/or CSI.
  /// \return If any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the PUCCH
  /// resource indicator corresponding to the PUCCH resource that will be used by the UE. If there are no PUCCH
  /// resources available, the pointer passed will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  pucch_harq_resource_alloc_record
  reserve_next_f2_harq_res_available(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Returns a specific PUCCH format 2 resource (identified by the res. indicator) to be used for HARQ-ACK.
  /// \remark If SR and CSI multiplexing is enabled, this resource can be used for HARQ-ACK + SR and/or CSI.
  /// \return If the specific PUCCH resource is available, it returns the pointer to the configuration. Else, it returns
  /// \c nullptr.
  const pucch_resource* reserve_specific_format2_res(slot_point          slot_harq,
                                                     rnti_t              crnti,
                                                     unsigned            res_indicator,
                                                     const pucch_config& pucch_cfg);

  /// \brief Returns the specific PUCCH format 2 resource config to be used for CSI, if available.
  /// \remark If SR multiplexing is enabled, this resource can be used for CSI + SR.
  /// \return If the specific PUCCH resource is available, it returns (i) the pointer to the configuration and (ii) the
  /// PUCCH resource indicator corresponding to the PUCCH resource that will be used by the UE. Else, the pointer passed
  /// will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  const pucch_resource*
  reserve_csi_resource(slot_point slot_harq, rnti_t crnti, const ue_cell_configuration& ue_cell_cfg);

  /// \brief Returns the specific PUCCH format 1 resource config to be used for SR, if available.
  /// \return If the specific PUCCH resource is available, it returns (i) the pointer to the configuration and (ii) the
  /// PUCCH resource indicator corresponding to the PUCCH resource that will be used by the UE. Else, the pointer passed
  /// will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  const pucch_resource* reserve_sr_res_available(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 1) resource from being allocated to a given UE.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_harq_f1_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 2) resource from being allocated to a given UE.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_harq_f2_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 1) resource used for SR from being allocated to a given UE.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_sr_resource(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 2) resource used for CSI from being allocated to a given UE.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_csi_resource(slot_point slot_sr, rnti_t crnti, const ue_cell_configuration& ue_cell_cfg);

  /// \brief Returns the PUCCH resource indicator (format 1) of the resource used for a given RNTI at a given slot.
  /// \return PUCCH resource indicator of the resource used allocated to the UE; if UE is not found, returns -1.
  int fetch_f1_pucch_res_indic(slot_point slot_tx, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Returns the PUCCH resource indicator (format 2) of the resource used for a given RNTI at a given slot.
  /// \return PUCCH resource indicator of the resource used allocated to the UE; if UE is not found, returns -1.
  int fetch_f2_pucch_res_indic(slot_point slot_tx, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Fetches the configuration of the PUCCH Format 2 resource used for HARQ previously allocated to a given RNTI
  /// for a given slot.
  /// \return If the resource is found, it returns (i) the pointer to the configuration and (ii) the PUCCH resource
  /// indicator corresponding to the PUCCH resource that previously allocated to the UE. Else, the pointer passed will
  /// be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  const pucch_harq_resource_alloc_record
  fetch_allocated_f2_harq_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Returns the configuration of the PUCCH resource used for CSI (format 2) for a given RNTI at a given slot.
  /// \return Pointer to the resource configuration used allocated to the UE; if UE is not found, returns \c nullptr.
  const pucch_resource*
  fetch_csi_pucch_res_config(slot_point slot_tx, rnti_t crnti, const ue_cell_configuration& ue_cell_cfg);

private:
  /// Size of the ring buffer of pucch_resource_manager. This size sets a limit on how far in advance a PUCCH can be
  /// allocated.
  static const size_t RES_MANAGER_RING_BUFFER_SIZE =
      get_allocator_ring_size_gt_min(SCHEDULER_MAX_K0 + SCHEDULER_MAX_K1);

  static const unsigned PUCCH_HARQ_F1_RES_SET_ID = 0;
  static const unsigned PUCCH_HARQ_F2_RES_SET_ID = 1;

  // [Implementation-defined] We assume as the maximum number of PUCCH resources that can be handled by the resource
  // manager \c maxNrofPUCCH-Resources, TS 38.331.
  static const size_t MAX_PUCCH_RESOURCES{128};
  // As per Section 9.2.1, TS 38.213, this is given by the number of possible values of r_PUCCH, which is 16.
  static const size_t MAX_COMMON_PUCCH_RESOURCES{16};

  enum class pucch_resource_usage { NOT_USED = 0, HARQ_F1, HARQ_F2, SR, CSI };

  // Tracks usage of PUCCH resources.
  struct resource_tracker {
    rnti_t               rnti;
    pucch_resource_usage resource_usage;
  };

  using pucch_res_record_array  = std::array<resource_tracker, MAX_PUCCH_RESOURCES>;
  using common_res_record_array = std::array<bool, MAX_COMMON_PUCCH_RESOURCES>;

  // Record for the RNTI and PUCCH resource indicator used for a given resource at a given slot.
  struct rnti_pucch_res_id_slot_record {
    common_res_record_array used_common_resources;
    pucch_res_record_array  ues_using_pucch_res;
  };

  // Returns the resource manager allocation record for a given slot.
  rnti_pucch_res_id_slot_record& get_slot_resource_counter(slot_point sl);

  // Helper functions that implement the public interface methods.
  pucch_harq_resource_alloc_record reserve_next_harq_res_available(slot_point          slot_harq,
                                                                   rnti_t              crnti,
                                                                   const pucch_config& pucch_cfg,
                                                                   pucch_format        format);

  bool release_harq_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg, pucch_format format);

  int fetch_pucch_res_indic(slot_point slot_tx, rnti_t crnti, const pucch_config& pucch_cfg, pucch_format format);

  // Ring buffer of rnti_pucch_res_id_slot_record for PUCCH resources.
  std::array<rnti_pucch_res_id_slot_record, RES_MANAGER_RING_BUFFER_SIZE> resource_slots;

  // Keeps track of the last slot_point used by the resource manager.
  slot_point last_sl_ind;
};

} // namespace srsran
