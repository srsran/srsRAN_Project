/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PDCCH_SCHEDULER_H
#define SRSGNB_PDCCH_SCHEDULER_H

#include "../ue/ue_configuration.h"
#include "resource_grid.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

class pdcch_scheduler
{
public:
  explicit pdcch_scheduler(cell_resource_allocator& res_grid_);
  ~pdcch_scheduler();

  void slot_indication(slot_point sl_tx);

  /// Allocates RE space for common PDCCH, avoiding in the process collisions with other PDCCH allocations.
  /// \param rnti RNTI of allocation. Values: SI_RNTI, RAR_RNTI.
  /// \param ss_id Search Space Id to use.
  /// \param L Aggregation Level of PDCCH allocation.
  /// \return Allocated PDCCH if successful.
  pdcch_information*
  alloc_pdcch_common(slot_point sl_tx, rnti_t rnti, search_space_id ss_id, aggregation_level aggr_lvl);

  pdcch_information* alloc_pdcch_ue(slot_point                      sl_tx,
                                    rnti_t                          rnti,
                                    const ue_carrier_configuration& user,
                                    du_bwp_id_t                     bwp_id,
                                    search_space_id                 ss_id,
                                    aggregation_level               aggr_lvl,
                                    dci_dl_format                   dci_fmt);

private:
  class pdcch_slot_allocator;

  /// Size of the ring buffer of pdcch_slot_allocator. This size sets a limit on how far in advance a PDCCH can be
  /// allocated.
  static const size_t SLOT_ALLOCATOR_RING_SIZE = 10;

  pdcch_slot_allocator& get_slot_alloc(slot_point sl);
  pdcch_information*    alloc_dl_pdcch_helper(slot_point                        sl_tx,
                                              rnti_t                            rnti,
                                              const bwp_configuration&          bwp_cfg,
                                              const coreset_configuration&      cs_cfg,
                                              const search_space_configuration& ss_cfg,
                                              aggregation_level                 L,
                                              dci_dl_format                     dci_fmt);

  cell_resource_allocator& res_alloc;

  slot_point last_sl_ind;

  /// Intermediate results used by PDCCH scheduler.
  std::array<std::unique_ptr<pdcch_slot_allocator>, SLOT_ALLOCATOR_RING_SIZE> slot_records;
};

} // namespace srsgnb

#endif // SRSGNB_PDCCH_SCHEDULER_H
