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

/// PUCCH scheduling interface.
class pucch_scheduler
{
public:
  virtual ~pucch_scheduler() = default;

  /// Allocates RE space for UE-dedicated DL PDCCH, avoiding in the process collisions with other PDCCH allocations.
  /// \param[out] slot_alloc struct with scheduling results.
  /// \param[out,in] dl_alloc_info information with DL DCI, needed for HARQ-(N)-ACK scheduling info.
  /// \param rnti RNTI of UE being allocated.
  /// \param user UE configuration for the provided cell.
  /// \param bwp_id BWP Id to use.
  /// \return Allocated PUCCH if successful, if any.
  virtual ul_pucch_info* alloc_pucch_ue(cell_slot_resource_allocator& slot_alloc,
                                        pdcch_dl_information&         dl_alloc_info,
                                        rnti_t                        rnti,
                                        const ue_cell_configuration&  user,
                                        bwp_id_t                      bwpid) = 0;
};

} // namespace srsgnb