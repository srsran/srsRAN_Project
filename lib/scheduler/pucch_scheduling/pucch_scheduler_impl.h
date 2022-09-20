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
#include "pucch_scheduler.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

/// Implementation of the PUCCH scheduler interface.
class pucch_scheduler_impl final : public pucch_scheduler
{
public:
  explicit pucch_scheduler_impl(const cell_configuration& cell_cfg_);

  ~pucch_scheduler_impl() override;

  /// Allocate the PUCCH resource for HARQ-ACK for a given UE.
  /// \param[out] pucch_res_indicator PUCCH resource indicator field for DCI 1_0 and 1_1.
  /// \param[out] harq_feedback_timing_indicator PUCCH delay (with respect to PDSCH) to be encoded in DCI 1_0 and 1_1.
  /// \param[out,in] slot_alloc struct with scheduling results.
  /// \param[in] dci_info information with DL DCI, needed for HARQ-(N)-ACK scheduling info.
  /// \param[in] rnti RNTI of UE being allocated.
  /// \param[in] ue object that contain the PUCCH resource and Logical Channel configuration.
  /// \param[in] user UE configuration for the provided cell.
  /// \return[in] Allocated PUCCH pointer, if successful. Else, it returns \c nullptr.
  ul_pucch_info* alloc_pucch_harq_ack_ue(unsigned&                    pucch_res_indicator,
                                         unsigned&                    harq_feedback_timing_indicator,
                                         cell_resource_allocator&     slot_alloc,
                                         const pdcch_dl_information&  dci_info,
                                         rnti_t                       rnti,
                                         const ue&                    ue,
                                         const ue_cell_configuration& user) override;

private:
  // Structs with the info about the PUCCH resources.
  struct pucch_res_alloc_cfg {
    /// True if the struct has a valid config.
    bool       has_config;
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
  void fill_pucch_res_output(ul_pucch_info&               pucch_info,
                             rnti_t                       rnti,
                             pucch_res_alloc_cfg          pucch_res,
                             const ue_cell_configuration& ue_cell_cfg);

  // Retrieves delay in slot (i.e., k0) between the DCI and the PDSCH.
  const unsigned get_pdsch_k0() const;

  // Cell configuration.
  const cell_configuration& cell_cfg;
};

} // namespace srsgnb
