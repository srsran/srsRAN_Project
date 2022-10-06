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
#include "../ue_scheduling/ue.h"
#include "../ue_scheduling/ue_configuration.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

/// Contains the output of the PUCCH allocator for HARQ-ACK grant.
struct pucch_harq_ack_grant {
  /// \c pucch_res_indicator, or \f$\Delta_{PRI}\f$, is the <em>PUCCH resource indicator<\em> field for DCI 1_0 and 1_1
  /// as per TS 38.213, Section 9.2.1. It indicates the UE which PUCCH resource should be used for HACK-(N)ACK
  /// reporting.
  /// \remark This is valid only if \c pucch is NOT nullptr.
  unsigned pucch_res_indicator;
  /// Pointer of the allocated PUCCH PDU; if \c nullptr, the allocation wasn't successful.
  pucch_info* pucch_pdu{nullptr};
};

/// PUCCH scheduling interface.
class pucch_scheduler
{
public:
  virtual ~pucch_scheduler() = default;

  /// Allocate the PUCCH resource for HARQ-ACK for a given UE.
  /// \param[out,in] slot_alloc struct with scheduling results.
  /// \param[in] tcrnti temporary RNTI  of the UE.
  /// \param[in] pdsch_time_domain_resource k0 value, or delay (in slots) of PDSCH slot vs the corresponding PDCCH slot.
  /// \param[in] k1 delay in slots of the UE's PUCCH HARQ-ACK report with respect to the PDSCH.
  /// \param[in] dci_info information with DL DCI, needed for HARQ-(N)-ACK scheduling info.
  /// \return The grant for the UE's PUCCH HARQ-(N)-ACK report.
  virtual pucch_harq_ack_grant alloc_common_pucch_harq_ack_ue(cell_resource_allocator&    slot_alloc,
                                                              rnti_t                      tcrnti,
                                                              unsigned                    pdsch_time_domain_resource,
                                                              unsigned                    k1,
                                                              const pdcch_dl_information& dci_info) = 0;
};

} // namespace srsgnb
