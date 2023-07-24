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

#include "../cell/resource_grid.h"
#include "../ue_scheduling/ue_configuration.h"
#include "pucch_allocator.h"
#include "pucch_resource_manager.h"
#include "srsran/scheduler/scheduler_dci.h"

namespace srsran {

/// Implementation of the PUCCH allocator interface.
class pucch_allocator_impl final : public pucch_allocator
{
public:
  explicit pucch_allocator_impl(const cell_configuration& cell_cfg_);

  ~pucch_allocator_impl() override;

  pucch_harq_ack_grant alloc_common_pucch_harq_ack_ue(cell_resource_allocator&    res_alloc,
                                                      rnti_t                      tcrnti,
                                                      unsigned                    pdsch_time_domain_resource,
                                                      unsigned                    k1,
                                                      const pdcch_dl_information& dci_info) override;

  pucch_harq_ack_grant alloc_ded_pucch_harq_ack_ue(cell_resource_allocator&     res_alloc,
                                                   rnti_t                       crnti,
                                                   const ue_cell_configuration& ue_cell_cfg,
                                                   unsigned                     pdsch_time_domain_resource,
                                                   unsigned                     k1) override;

  void pucch_allocate_sr_opportunity(cell_slot_resource_allocator& slot_alloc,
                                     rnti_t                        crnti,
                                     const ue_cell_configuration&  ue_cell_cfg) override;

  void pucch_allocate_csi_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                      rnti_t                        crnti,
                                      const ue_cell_configuration&  ue_cell_cfg,
                                      unsigned                      csi_part1_nof_bits) override;

  pucch_uci_bits remove_ue_uci_from_pucch(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        crnti,
                                          const ue_cell_configuration&  ue_cell_cfg) override;

  /// Updates the internal slot_point and tracking of PUCCH resource usage over time.
  void slot_indication(slot_point sl_tx);

private:
  // Structs with the info about the PUCCH resources.
  struct pucch_res_alloc_cfg {
    // True if the struct has a valid config.
    unsigned   pucch_res_indicator;
    grant_info first_hop_res;
    // Contains grant only if intra-slot freq-hopping is active.
    grant_info second_hop_res;
    // Cyclic-shift.
    unsigned cs;
    // PUCCH format.
    pucch_format format;
  };

  // Allocates the PUCCH (common) resource for HARQ-(N)-ACK.
  optional<pucch_res_alloc_cfg> alloc_pucch_common_res_harq(cell_slot_resource_allocator&  pucch_alloc,
                                                            const dci_context_information& dci_info);

  // Helper that allocates a NEW PUCCH HARQ grant (Format 1).
  pucch_harq_ack_grant allocate_new_pucch_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                     rnti_t                        crnti,
                                                     const ue_cell_configuration&  ue_cell_cfg,
                                                     pucch_info*                   existing_sr_grant);

  // Helper that updates an existing PUCCH HARQ grant (Format 1).
  pucch_harq_ack_grant update_existing_pucch_harq_grant(pucch_info&         existing_harq_grant,
                                                        pucch_info*         existing_sr_grant,
                                                        rnti_t              rnti,
                                                        slot_point          sl_tx,
                                                        const pucch_config& pucch_cfg);

  // Helper that allocates a NEW PUCCH HARQ grant (Format 2).
  pucch_harq_ack_grant allocate_new_format2_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                  rnti_t                        crnti,
                                                  const ue_cell_configuration&  ue_cell_cfg,
                                                  unsigned                      harq_ack_bits,
                                                  sr_nof_bits                   sr_bits,
                                                  unsigned                      csi_part1_bits);

  // Helper that replaces PUCCH grant Format 1 with Format 2 grant.
  pucch_harq_ack_grant convert_to_format2(cell_slot_resource_allocator& pucch_slot_alloc,
                                          pucch_info*                   existing_harq_grant,
                                          pucch_info*                   existing_sr_grant,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  ue_cell_cfg,
                                          unsigned                      harq_ack_nof_bits,
                                          unsigned                      csi_part1_nof_bits);

  // Helper that changes the current PUCCH Format 2 and allocates PUCCH grant. Used to change PUCCH resource used for
  // CSI into a PUCCH Format 2 resource for the
  pucch_harq_ack_grant change_format2_resource(cell_slot_resource_allocator& pucch_slot_alloc,
                                               pucch_info&                   existing_grant,
                                               rnti_t                        rnti,
                                               const ue_cell_configuration&  ue_cell_cfg,
                                               unsigned                      harq_ack_bits_increment);

  // Helper that updates PUCCH Format 2 grant.
  pucch_harq_ack_grant update_format2_grant(pucch_info&                  existing_f2_grant,
                                            slot_point                   sl_tx,
                                            const ue_cell_configuration& ue_cell_cfg,
                                            unsigned                     harq_ack_bits_increment,
                                            sr_nof_bits                  sr_bits_increment,
                                            unsigned                     csi_part1_bits_increment);

  // Helper that removes the existing PUCCH Format 1 grants (both HARQ-ACK and SR).
  void remove_pucch_format1_from_grants(cell_slot_resource_allocator& slot_alloc,
                                        rnti_t                        crnti,
                                        const pucch_config&           pucch_cfg);

  // Helper that removes the existing PUCCH Format 1 grants (both HARQ-ACK and SR).
  void remove_format2_csi_from_grants(cell_slot_resource_allocator& slot_alloc,
                                      rnti_t                        crnti,
                                      const ue_cell_configuration&  ue_cell_cfg);

  // Fills the PUCCH HARQ grant for common resources.
  void fill_pucch_harq_common_grant(pucch_info& pucch_info, rnti_t rnti, const pucch_res_alloc_cfg& pucch_res);

  // Fills the PUCCH Format 1 grant.
  void fill_pucch_ded_format1_grant(pucch_info&           pucch_grant,
                                    rnti_t                crnti,
                                    const pucch_resource& pucch_ded_res_cfg,
                                    unsigned              harq_ack_bits,
                                    sr_nof_bits           sr_bits);

  // Fills the PUCCH Format 2 grant.
  void fill_pucch_format2_grant(pucch_info&                  pucch_grant,
                                rnti_t                       crnti,
                                const pucch_resource&        pucch_ded_res_cfg,
                                const ue_cell_configuration& ue_cell_cfg,
                                unsigned                     nof_prbs,
                                unsigned                     harq_ack_bits,
                                sr_nof_bits                  sr_bits,
                                unsigned                     csi_part1_bits);

  const unsigned            PUCCH_FORMAT_1_NOF_PRBS{1};
  const cell_configuration& cell_cfg;
  slot_point                last_sl_ind;
  pucch_resource_manager    resource_manager;

  srslog::basic_logger& logger;
};

} // namespace srsran
