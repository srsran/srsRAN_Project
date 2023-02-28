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

/// Class that is in charge of providing the scheduler an available PUCCH resource to be used, either for HARQ or SR.
class pucch_resource_manager
{
public:
  pucch_resource_manager();

  // Reset all resources to "unused".
  void slot_indication(slot_point slot_tx);

  /// Returns the PUCCH resource to be used for HARQ-ACK (format 1).
  /// \remark This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  /// \return If any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the PUCCH
  /// resource indicator corresponding to the PUCCH resource that will be used by the UE. If there are no PUCCH
  /// resources available, the pointer passed will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  pucch_harq_resource_alloc_record
  reserve_next_harq_res_available(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// Returns the PUCCH format 2 resource to be used (SR / HARQ-ACK / CSI).
  /// \remark This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  /// \return If any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the PUCCH
  /// resource indicator corresponding to the PUCCH resource that will be used by the UE. If there are no PUCCH
  /// resources available, the pointer passed will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  pucch_harq_resource_alloc_record
  reserve_next_format2_res_available(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// Returns the PUCCH format 2 resource to be used (SR / HARQ-ACK / CSI).
  /// \remark This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  /// \return If any PUCCH resource available, it returns the pointer to the configuration. If there are no PUCCH
  /// resources available, the pointer passed will be \c nullptr..
  const pucch_resource* reserve_specific_format2_res(slot_point          slot_harq,
                                                     rnti_t              crnti,
                                                     unsigned            res_indicator,
                                                     const pucch_config& pucch_cfg);

  /// Returns the PUCCH format 2 resource to be used (SR / CSI).
  /// \remark This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  /// \return If any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the PUCCH
  /// resource indicator corresponding to the PUCCH resource that will be used by the UE. If there are no PUCCH
  /// resources available, the pointer passed will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  const pucch_resource*
  reserve_csi_resource(slot_point slot_harq, rnti_t crnti, const ue_cell_configuration& ue_cell_cfg);

  /// Returns the pointer to the configuration of the PUCCH resource to be used for SR.
  /// \remark There is only one resource used for SR.
  /// \return the pointer to the configuration of the PUCCH resource to be used for SR, if available; else, it returns
  /// \c nullptr.
  const pucch_resource* reserve_sr_res_available(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 1) resource from being allocated to a given UE.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_harq_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 2) resource from being allocated to a given UE.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_format2_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 1) resource used for SR from being allocated to a given UE.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_sr_resource(slot_point slot_sr, rnti_t crnti);

  /// \brief Release PUCCH (format 2) resource used for CSI from being allocated to a given UE.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_csi_resource(slot_point slot_sr, rnti_t crnti);

  /// \brief Returns the PUCCH resource indicator (format 1) of the resource used for a given RNTI at a given slot.
  /// \return PUCCH resource indicator of the resource used allocated to the UE; if UE is not found, returns -1.
  int fetch_f1_pucch_res_indic(slot_point slot_tx, rnti_t crnti);

  /// \brief Returns the PUCCH resource indicator (format 2) of the resource used for a given RNTI at a given slot.
  /// \return PUCCH resource indicator of the resource used allocated to the UE; if UE is not found, returns -1.
  int fetch_f2_pucch_res_indic(slot_point slot_tx, rnti_t crnti);

  /// \brief Returns the configuration of the PUCCH resource used for CSI (format 2) for a given RNTI at a given slot.
  /// \return Pointer to the resource configuration used allocated to the UE; if UE is not found, returns \c nullptr.
  const pucch_resource*
  fetch_csi_pucch_res_config(slot_point slot_tx, rnti_t crnti, const ue_cell_configuration& ue_cell_cfg);

private:
  /// Size of the ring buffer of pucch_resource_manager. This size sets a limit on how far in advance a PUCCH can be
  /// allocated.
  static const size_t RES_MANAGER_RING_BUFFER_SIZE =
      get_allocator_ring_size_gt_min(SCHEDULER_MAX_K0 + SCHEDULER_MAX_K1);

  // [Implementation-defined] Number of PUCCH resources (of single format) that can be handled by the resource manager.
  // NOTE: this number allows us to have a 1-to-1 match between PUCCH resource indicator and index of the PUCCH resource
  // in its corresponding PUCCH resource set.
  static const size_t MAX_HARQ_PUCCH_RESOURCES{8};
  static const size_t MAX_SR_PUCCH_RESOURCES{4};

  using pucch_res_record_array = std::array<rnti_t, MAX_HARQ_PUCCH_RESOURCES>;
  struct sr_record {
    int    pucch_res_id;
    rnti_t allocated_ue{INVALID_RNTI};
  };

  // Record for the RNTI and PUCCH resource indicator used for a given resource at a given slot.
  struct rnti_pucch_res_id_slot_record {
    // Keep track of the UE allocated to the SR resource.
    std::array<sr_record, MAX_SR_PUCCH_RESOURCES> ues_using_sr_resources;
    // Keep track of the UE allocated to the SR resource.
    rnti_t ue_using_csi_resource{INVALID_RNTI};
    // Keeps track of the RNTI of the UE using a given PUCCH resource format 1 (indexed by the PUCCH res. indicator).
    pucch_res_record_array ues_using_format1_res;
    // Keeps track of the RNTI of the UE using a given PUCCH resource format 2 (indexed by the PUCCH res. indicator).
    pucch_res_record_array ues_using_format2_res;
  };

  // Returns the resource manager allocation record for a given slot.
  rnti_pucch_res_id_slot_record& get_slot_resource_counter(slot_point sl);

  // Ring buffer of rnti_pucch_res_id_slot_record for PUCCH resources.
  std::array<rnti_pucch_res_id_slot_record, RES_MANAGER_RING_BUFFER_SIZE> resource_slots;

  // Keeps track of the last slot_point used by the resource manager.
  slot_point last_sl_ind;
};

} // namespace srsran
