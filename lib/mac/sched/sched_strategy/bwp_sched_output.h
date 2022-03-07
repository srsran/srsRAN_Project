
#ifndef SRSGNB_BWP_SCHED_OUTPUT_H
#define SRSGNB_BWP_SCHED_OUTPUT_H

#include "bwp_sched_input.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Object where data scheduler strategy stores DL grants
class bwp_dl_sched_output
{
public:
  bwp_dl_sched_output(slot_point sl_tx_, du_bwp_id_t bwp_id_) : sl_tx(sl_tx_), bwp_id(bwp_id_) {}

  bool alloc_pdsch(ue_candidate ue)
  {
    // TODO
  }

private:
  const slot_point  sl_tx;
  const du_bwp_id_t bwp_id;
};

/// Object where data scheduler strategy stores UL grants
class bwp_ul_sched_output
{
public:
  bwp_ul_sched_output(slot_point sl_tx_, du_bwp_id_t bwp_id_) : sl_tx(sl_tx_), bwp_id(bwp_id_) {}

  bool alloc_pusch(ue_candidate ue)
  {
    // TODO
  }

private:
  const slot_point  sl_tx;
  const du_bwp_id_t bwp_id;
};

} // namespace srsgnb

#endif // SRSGNB_BWP_SCHED_OUTPUT_H