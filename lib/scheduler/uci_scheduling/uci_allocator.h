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
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

/// Contains the results of the UCI allocation.
struct uci_allocation {
  /// true if the allocation was successful, false otherwise.
  bool alloc_successful{false};
  /// If UCI is allocated on the PUCCH, contains the PUCCH grant info; else, this is to be ignored.
  pucch_harq_ack_grant pucch_grant;
};

/// \brief UCI allocator interface.
/// Depending on whether there is an existing PUSCH grant, it allocates the UCI either on the PUSCH or PUCCH.
class uci_allocator
{
public:
  virtual ~uci_allocator() = default;

  /// Allocate the common PUCCH resource for HARQ-ACK for a given UE.
  /// \param[out,in] res_alloc struct with scheduling results.
  /// \param[in] crnti RNTI of the UE.
  /// \param[in] ue_cell_cfg user configuration.
  /// \param[in] pdsch_time_domain_resource k0 value, or delay (in slots) of PDSCH slot vs the corresponding PDCCH slot.
  /// \param[in] k1 delay in slots of the UE's PUCCH HARQ-ACK report with respect to the PDSCH.
  virtual uci_allocation alloc_uci_harq_ue(cell_resource_allocator&     res_alloc,
                                           rnti_t                       crnti,
                                           const ue_cell_configuration& ue_cell_cfg,
                                           unsigned                     pdsch_time_domain_resource,
                                           unsigned                     k1) = 0;

  virtual void move_uci_from_pucch_to_pusch(ul_sched_info&                pusch,
                                            cell_slot_resource_allocator& slot_alloc,
                                            const ue_cell_configuration&  ue_cell_cfg,
                                            rnti_t                        crnti) = 0;
};

} // namespace srsgnb
