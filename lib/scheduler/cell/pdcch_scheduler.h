
#ifndef SRSGNB_PDCCH_SCHEDULER_H
#define SRSGNB_PDCCH_SCHEDULER_H

#include "../ue/ue_configuration.h"
#include "resource_grid.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

/// PDCCH scheduling algorithm for a single cell and for both common and UE-dedicated CORESETs.
class pdcch_scheduler
{
public:
  virtual ~pdcch_scheduler() = default;

  /// Allocates RE space for common PDCCH, avoiding in the process collisions with other PDCCH allocations.
  /// \param rnti RNTI of allocation. Values: SI_RNTI, RAR_RNTI.
  /// \param ss_id Search Space Id to use.
  /// \param L Aggregation Level of PDCCH allocation.
  /// \return Allocated PDCCH if successful.
  virtual pdcch_information* alloc_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                                rnti_t                        rnti,
                                                search_space_id               ss_id,
                                                aggregation_level             aggr_lvl) = 0;

  virtual pdcch_information* alloc_pdcch_ue(cell_slot_resource_allocator&   slot_alloc,
                                            rnti_t                          rnti,
                                            const ue_carrier_configuration& user,
                                            du_bwp_id_t                     bwp_id,
                                            search_space_id                 ss_id,
                                            aggregation_level               aggr_lvl,
                                            dci_dl_format                   dci_fmt) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PDCCH_SCHEDULER_H
