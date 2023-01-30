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
#include "srsgnb/adt/circular_array.h"

namespace srsgnb {

/// Implementation of \ref uci_allocator interface.
class uci_allocator_impl final : public uci_allocator
{
public:
  explicit uci_allocator_impl(pucch_allocator& pucch_alloc_);

  void slot_indication(slot_point sl_tx) override;

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

  void uci_allocate_csi_opportunity(cell_slot_resource_allocator& slot_alloc,
                                    rnti_t                        crnti,
                                    const ue_cell_configuration&  ue_cell_cfg) override;

private:
  /// \brief Information cached by the UCI scheduler relative to the UCIs scheduled in the cell resource grid. Store
  /// here any information that does not need to be stored in the PUCCH and PUSCH PDUs and does not need to be sent to
  /// the PHY.
  struct slot_alloc_list {
    struct ue_uci {
      /// RNTI of this UCI.
      rnti_t rnti = rnti_t::INVALID_RNTI;
      /// Number of HARQs currently scheduled for this UE UCI.
      unsigned harq_ack_counter = 0;
    };
    static_vector<ue_uci, MAX_PUCCH_PDUS_PER_SLOT> ucis;
  };

  // Fill the UCI on the PUSCH with the number of bits and alpha and beta parameters.
  // \param[in] uci UCI PDU grant.
  // \param[in] uci_cfg UCI configuration for PUSCH.
  // \param[in] harq_ack_nof_bits num. of HARQ-ACK bits to reported by the UE.
  // \param[in] csi_part1_nof_bits num. of CSI part1 bits to reported by the UE.
  void fill_uci_on_pusch(uci_info&           uci,
                         const uci_on_pusch& uci_cfg,
                         unsigned            harq_ack_nof_bits,
                         unsigned            csi_part1_nof_bits);

  /// \brief Fetches UCI alloc information for a given slot and UE. Returns nullptr if no UCI allocation was found.
  /// \return The UE UCI information for a given UCI slot and RNTI. If no UCI exists with the provided params, returns
  /// nullptr.
  slot_alloc_list::ue_uci* get_uci_alloc(slot_point uci_slot, rnti_t rnti);

  pucch_allocator& pucch_alloc;

  srslog::basic_logger& logger;

  /// \brief Ring of UCI allocations indexed by slot.
  circular_array<slot_alloc_list, cell_resource_allocator::RING_ALLOCATOR_SIZE> uci_alloc_grid;
};

} // namespace srsgnb
