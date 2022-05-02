
#ifndef SRSGNB_PDCCH_SCHEDULER_H
#define SRSGNB_PDCCH_SCHEDULER_H

#include "resource_grid.h"

namespace srsgnb {

class pdcch_scheduler
{
public:
  pdcch_scheduler(cell_resource_grid& res_grid_);
  ~pdcch_scheduler();

  void slot_indication(slot_point sl_tx);

  void alloc_dl_pdcch(slot_point sl_tx);

  size_t nof_dcis() const { return 0; }

private:
  cell_resource_grid& res_grid;

  slot_point last_sl_ind;

  /// Intermediate results used by PDCCH scheduler.
  class alloc_record_list;
  std::unique_ptr<alloc_record_list> records;
};

} // namespace srsgnb

#endif // SRSGNB_PDCCH_SCHEDULER_H
