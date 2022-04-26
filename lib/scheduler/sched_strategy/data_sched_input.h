
#ifndef SRSGNB_DATA_SCHED_INPUT_H
#define SRSGNB_DATA_SCHED_INPUT_H

#include "../ue/ue.h"

namespace srsgnb {

/// Handle to UE carrier that is eligible for being scheduled
class ue_candidate
{
public:
  ue_candidate(ue_carrier& u_) : ue(&u_)
  {
    // :TODO: remove this when ue var is used.
    (void)(ue);
  }

private:
  ue_carrier* ue;
};

/// Container of eligible UEs for scheduling
using ue_candidate_map_t = circular_map<uint16_t, ue_candidate, MAX_NOF_DU_UES>;

/// Input parameters relative to a {slot, cell} which the scheduling strategy uses to allocate UEs
class data_sched_input
{
public:
  data_sched_input(slot_point sl_tx_, du_cell_index_t cell_index_) : sl_tx(sl_tx_), cell_index(cell_index_) {}

  const slot_point      sl_tx;
  const du_cell_index_t cell_index;

  ue_candidate_map_t eligible_ues;
};

/// Fill list of eligible UEs for data scheduling
inline void fill_ue_candidate_map(du_cell_index_t cell_index, const ue_map_t& ue_db, data_sched_input& sched_input)
{
  for (const std::unique_ptr<ue>& ueptr : ue_db) {
    ue_carrier* ue_cc = ueptr->find_cc(cell_index);
    if (ue_cc != nullptr and ue_cc->is_active() and ueptr->has_pending_txs()) {
      // TODO: Check measGaps and other factors
      sched_input.eligible_ues.insert(ueptr->ue_index, ue_candidate{*ue_cc});
    }
  }
}

} // namespace srsgnb

#endif // SRSGNB_DATA_SCHED_INPUT_H
