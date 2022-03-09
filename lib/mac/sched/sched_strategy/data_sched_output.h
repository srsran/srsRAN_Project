
#ifndef SRSGNB_DATA_SCHED_OUTPUT_H
#define SRSGNB_DATA_SCHED_OUTPUT_H

#include "data_sched_input.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Object where data scheduler strategy stores DL grants
class dl_data_sched_output
{
public:
  dl_data_sched_output(slot_point sl_tx_, du_bwp_id_t bwp_id_) : sl_tx(sl_tx_), bwp_id(bwp_id_) {}

  bool alloc_pdsch(ue_candidate ue)
  {
    // TODO
    return true;
  }

private:
  const slot_point  sl_tx;
  const du_bwp_id_t bwp_id;
};

/// Object where data scheduler strategy stores UL grants
class ul_data_sched_output
{
public:
  ul_data_sched_output(slot_point sl_tx_, du_bwp_id_t bwp_id_) : sl_tx(sl_tx_), bwp_id(bwp_id_) {}

  bool alloc_pusch(ue_candidate ue)
  {
    // TODO
    return true;
  }

private:
  const slot_point  sl_tx;
  const du_bwp_id_t bwp_id;
};

} // namespace srsgnb

#endif // SRSGNB_DATA_SCHED_OUTPUT_H