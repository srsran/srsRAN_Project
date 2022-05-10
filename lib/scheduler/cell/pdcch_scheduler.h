
#ifndef SRSGNB_PDCCH_SCHEDULER_H
#define SRSGNB_PDCCH_SCHEDULER_H

#include "../sched_dci.h"
#include "../ue/ue_configuration.h"
#include "resource_grid.h"

namespace srsgnb {

class pdcch_scheduler
{
public:
  pdcch_scheduler(cell_resource_grid& res_grid_);
  ~pdcch_scheduler();

  void slot_indication(slot_point sl_tx);

  /// Allocates RE space for common PDCCH, avoiding in the process collisions with other PDCCH allocations.
  /// \param rnti RNTI of allocation. Values: SI_RNTI, RAR_RNTI.
  /// \param ss_id Search Space Id to use.
  /// \param L Aggregation Level of PDCCH allocation.
  /// \param dci_fmt DCI Format to use.
  /// \return Allocated PDCCH if successful.
  pdcch_information*
  alloc_pdcch_common(slot_point sl_tx, rnti_t rnti, search_space_id ss_id, aggregation_level L, dci_format dci_fmt);

  pdcch_information* alloc_pdcch_ue(slot_point                      sl_tx,
                                    rnti_t                          rnti,
                                    const ue_carrier_configuration& user,
                                    du_bwp_id_t                     bwp_id,
                                    search_space_id                 ss_id,
                                    aggregation_level               L,
                                    dci_format                      dci_fmt);

  size_t nof_dcis() const { return 0; }

private:
  pdcch_information* alloc_dl_pdcch_helper(slot_point                        sl_tx,
                                           rnti_t                            rnti,
                                           const bwp_configuration&          bwp_cfg,
                                           const coreset_configuration&      cs_cfg,
                                           const search_space_configuration& ss_cfg,
                                           aggregation_level                 L,
                                           dci_format                        dci_fmt);

  cell_resource_grid& res_grid;

  slot_point last_sl_ind;

  /// Intermediate results used by PDCCH scheduler.
  class alloc_record_list;
  std::unique_ptr<alloc_record_list> records;
};

} // namespace srsgnb

#endif // SRSGNB_PDCCH_SCHEDULER_H
