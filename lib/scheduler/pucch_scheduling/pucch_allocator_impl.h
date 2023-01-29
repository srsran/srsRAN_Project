/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/resource_grid.h"
#include "../ue_scheduling/ue_configuration.h"
#include "pucch_allocator.h"
#include "pucch_resource_manager.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

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
                                          const pucch_config&           pucch_cfg) override;

  /// Updates the internal slot_point and tracking of PUCCH resource usage over time.
  void slot_indication(slot_point sl_tx);

private:
  // Structs with the info about the PUCCH resources.
  struct pucch_res_alloc_cfg {
    // True if the struct has a valid config.
    bool       has_config{false};
    grant_info first_hop_res;
    // Contains grant only if intra-slot freq-hopping is active.
    grant_info second_hop_res;
    // Cyclic-shift.
    unsigned cs;
    // PUCCH format.
    pucch_format format;
  };

  // Allocates the PUCCH (common) resource for HARQ-(N)-ACK.
  pucch_res_alloc_cfg alloc_pucch_common_res_harq(unsigned&                      pucch_res_indicator,
                                                  cell_slot_resource_allocator&  pucch_alloc,
                                                  const dci_context_information& dci_info);

  // Helper that allocates a NEW PUCCH HARQ grant (Format 1).
  pucch_harq_ack_grant allocate_new_pucch_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                     rnti_t                        crnti,
                                                     const ue_cell_configuration&  ue_cell_cfg,
                                                     pucch_info*                   existing_sr_grant);

  // Helper that updates an existing PUCCH HARQ grant (Format 1).
  pucch_harq_ack_grant update_existing_pucch_harq_grant(pucch_info& existing_harq_grant,
                                                        pucch_info* existing_sr_grant,
                                                        rnti_t      rnti,
                                                        slot_point  sl_tx);

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
                                          unsigned                      csi_part1_nof_bits);

  // Helper that updates PUCCH Format 2 grant.
  pucch_harq_ack_grant update_format2_grant(pucch_info&                  existing_f2_grant,
                                            slot_point                   sl_tx,
                                            const ue_cell_configuration& ue_cell_cfg,
                                            unsigned                     harq_ack_bits,
                                            sr_nof_bits                  sr_bits,
                                            unsigned                     csi_part1_bits);

  // Helper that removes the existing PUCCH Format 1 grants (both HARQ-ACK and SR).
  void remove_pucch_format1_from_grants(cell_slot_resource_allocator& slot_alloc,
                                        rnti_t                        crnti,
                                        const pucch_config&           pucch_cfg);

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

#if 0

  // Allocates the PUCCH SR resource on grid.
  void allocate_pucch_ded_res_on_grid(cell_slot_resource_allocator& pucch_slot_alloc,
                                      const pucch_resource&         pucch_res,
                                      unsigned                      nof_prbs);

  // Helper that updates an existing PUCCH HARQ grant (Format 2).
  pucch_harq_ack_grant update_existing_format2_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                     pucch_info&                   existing_format2_grant,
                                                     rnti_t                        rnti,
                                                     slot_point                    sl_tx);

  pucch_harq_ack_grant replace_format2_res_with_csi_specific(cell_slot_resource_allocator& pucch_slot_alloc,
                                                             rnti_t                        crnti,
                                                             pucch_info&                   existing_f2_grant,

                                                             unsigned                     existing_grant_res_indic,
                                                             const ue_cell_configuration& ue_cell_cfg);
#endif
};

} // namespace srsgnb
