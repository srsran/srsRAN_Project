/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PDCCH_SCHEDULER_IMPL_H
#define SRSGNB_PDCCH_SCHEDULER_IMPL_H

#include "../ue/ue_configuration.h"
#include "pdcch_scheduler.h"
#include "resource_grid.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

class pdcch_scheduler_impl final : public pdcch_scheduler
{
public:
  explicit pdcch_scheduler_impl(const cell_configuration& cell_cfg_);
  ~pdcch_scheduler_impl() override;

  void slot_indication(slot_point sl_tx);

  pdcch_information* alloc_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                        rnti_t                        rnti,
                                        search_space_id               ss_id,
                                        aggregation_level             aggr_lvl) override;

  pdcch_information* alloc_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                    rnti_t                        rnti,
                                    const ue_cell_configuration&  user,
                                    du_bwp_id_t                   bwp_id,
                                    search_space_id               ss_id,
                                    aggregation_level             aggr_lvl,
                                    dci_dl_format                 dci_fmt) override;

private:
  class pdcch_slot_allocator;

  /// Size of the ring buffer of pdcch_slot_allocator. This size sets a limit on how far in advance a PDCCH can be
  /// allocated.
  static const size_t SLOT_ALLOCATOR_RING_SIZE = 10;

  pdcch_slot_allocator& get_pdcch_slot_alloc(slot_point sl);
  pdcch_information*    alloc_dl_pdcch_helper(cell_slot_resource_allocator&     slot_alloc,
                                              rnti_t                            rnti,
                                              const bwp_configuration&          bwp_cfg,
                                              const coreset_configuration&      cs_cfg,
                                              const search_space_configuration& ss_cfg,
                                              aggregation_level                 L,
                                              dci_dl_format                     dci_fmt);

  const cell_configuration& cell_cfg;

  /// Last slot for which slot_indication has been called.
  slot_point last_sl_ind;

  /// Intermediate results used by PDCCH scheduler.
  std::array<std::unique_ptr<pdcch_slot_allocator>, SLOT_ALLOCATOR_RING_SIZE> slot_records;
};

} // namespace srsgnb

#endif // SRSGNB_PDCCH_SCHEDULER_IMPL_H
