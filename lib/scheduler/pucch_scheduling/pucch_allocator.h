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

/// PUCCH scheduling interface.
class pucch_allocator
{
public:
  virtual ~pucch_allocator() = default;

  /// Allocate the PUCCH resource for HARQ-ACK for a given UE.
  /// \param[out] pucch_res_indicator <em>PUCCH resource indicator<\em> field for DCI 1_0 and 1_1.
  /// \param[out] harq_feedback_timing_indicator <em>PDSCH-to-HARQ_feedback timing indicator<\em>, as per TS 38.213,
  /// Section 9.2.3; it represents the PUCCH delay (with respect to PDSCH) to be encoded in DCI 1_0 and 1_1.
  /// \param[out,in] slot_alloc struct with scheduling results.
  /// \param[in] pdsch_time_domain_resource k0 value, or delay (in slots) of PDSCH slot vs the corresponding PDCCH slot.
  /// \param[in] dci_info information with DL DCI, needed for HARQ-(N)-ACK scheduling info.
  /// \param[in] ue object that contain the PUCCH resource and Logical Channel configuration.
  /// \param[in] user UE configuration for the provided cell.
  /// \return Allocated PUCCH pointer, if successful. Else, it returns \c nullptr.
  virtual pucch_info* alloc_pucch_harq_ack_ue(unsigned&                    pucch_res_indicator,
                                              unsigned&                    harq_feedback_timing_indicator,
                                              cell_resource_allocator&     slot_alloc,
                                              unsigned                     pdsch_time_domain_resource,
                                              const pdcch_dl_information&  dci_info,
                                              const ue&                    ue,
                                              const ue_cell_configuration& ue_cell_cfg) = 0;
};

} // namespace srsgnb
