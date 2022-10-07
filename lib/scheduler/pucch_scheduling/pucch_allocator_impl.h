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
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

/// Implementation of the PUCCH scheduler interface.
class pucch_allocator_impl final : public pucch_allocator
{
public:
  explicit pucch_allocator_impl(const cell_configuration& cell_cfg_);

  ~pucch_allocator_impl() override;

  /// Allocate the PUCCH resource for HARQ-ACK for a given UE.
  /// \param[out,in] slot_alloc struct with scheduling results.
  /// \param[in] tcrnti temporary RNTI  of the UE.
  /// \param[in] pdsch_time_domain_resource k0 value, or delay (in slots) of PDSCH slot vs the corresponding PDCCH slot.
  /// \param[in] dci_info information with DL DCI, needed for HARQ-(N)-ACK scheduling info.
  /// \return The grant for the UE's PUCCH HARQ-(N)-ACK report.
  pucch_harq_ack_grant alloc_common_pucch_harq_ack_ue(cell_resource_allocator&    slot_alloc,
                                                      rnti_t                      tcrnti,
                                                      unsigned                    pdsch_time_domain_resource,
                                                      const pdcch_dl_information& dci_info) override;

private:
  // Structs with the info about the PUCCH resources.
  struct pucch_res_alloc_cfg {
    /// True if the struct has a valid config.
    bool       has_config{false};
    grant_info first_hop_res;
    // Contains grant only if intra-slot freq-hopping is active.
    optional<grant_info> second_hop_res;
    // Cyclic-shift.
    unsigned cs;
    // PUCCH format.
    pucch_format format;
  };

  // Allocates the PUCCH (common) resource for HARQ-(N)-ACK.
  pucch_res_alloc_cfg alloc_pucch_common_res_harq(unsigned&                         pucch_res_indicator,
                                                  cell_slot_resource_allocator&     pucch_alloc,
                                                  const dci_dl_context_information& dci_info);

  // Fills the output of the PUCCH scheduler.
  void fill_pucch_res_output(pucch_info& pucch_info, rnti_t rnti, pucch_res_alloc_cfg pucch_res);

  // Cell configuration.
  const cell_configuration& cell_cfg;
};

} // namespace srsgnb
