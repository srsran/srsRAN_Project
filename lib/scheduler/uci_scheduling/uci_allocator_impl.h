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

#include "../pucch_scheduling/pucch_allocator.h"
#include "uci_allocator.h"

namespace srsgnb {

/// Implementation of \ref uci_allocator interface.
class uci_allocator_impl final : public uci_allocator
{
public:
  explicit uci_allocator_impl(pucch_allocator& pucch_alloc_);

  ~uci_allocator_impl() override;

  uci_allocation alloc_uci_harq_ue(cell_resource_allocator&     res_alloc,
                                   rnti_t                       crnti,
                                   const ue_cell_configuration& ue_cell_cfg,
                                   unsigned                     pdsch_time_domain_resource,
                                   unsigned                     k1) override;

  void multiplex_uci_on_pusch(ul_sched_info&                pusch_grant,
                              cell_slot_resource_allocator& slot_alloc,
                              const ue_cell_configuration&  ue_cell_cfg,
                              rnti_t                        crnti) override;

  void uci_allocate_sr_opportunity(cell_slot_resource_allocator& slot_alloc,
                                   rnti_t                        crnti,
                                   const ue_cell_configuration&  ue_cell_cfg) override;

private:
  // Allocates the HARQ-ACK on the PUSCH.
  // \param[in] pusch_grant PUSCH grant where UCI has to be allocated.
  // \param[in] uci_cfg UCI configuration for PUSCH.
  // \param[in] harq_ack_nof_bits num. of HARQ-ACK bits to reported by the UE.
  void allocate_uci_harq_on_pusch(ul_sched_info& pusch_grant, const uci_on_pusch& uci_cfg, unsigned harq_ack_nof_bits);

  pucch_allocator& pucch_alloc;

  srslog::basic_logger& logger;
};

} // namespace srsgnb
