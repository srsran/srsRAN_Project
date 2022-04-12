
#ifndef SRSGNB_UE_H
#define SRSGNB_UE_H

#include "srsgnb/adt/circular_map.h"
#include "srsgnb/mac/sched_interface.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/du_ue_list.h"

namespace srsgnb {

class ue_carrier
{
public:
  ue_carrier(du_ue_index_t ue_index, rnti_t crnti_val, du_cell_index_t cell_index) :
    ue_index(ue_index), cell_index(cell_index), crnti_(crnti_val)
  {}

  const du_ue_index_t   ue_index;
  const du_cell_index_t cell_index;

  rnti_t rnti() const { return crnti_; }

  unsigned active_bwp_id() const { return 0; }
  bool     is_active() const { return true; }

private:
  rnti_t crnti_;
};

class ue
{
public:
  ue(const sched_ue_creation_request_message& req) : ue_index(req.ue_index), crnti(req.crnti)
  {
    cells[0] = std::make_unique<ue_carrier>(ue_index, req.crnti, req.pcell_index);
  }

  const du_ue_index_t ue_index;
  const rnti_t        crnti;

  void slot_indication(slot_point sl_tx) {}

  ue_carrier* find_cc(du_cell_index_t cell_index)
  {
    srsran_assert(cell_index < MAX_CELLS, "Invalid cell_index={}", cell_index);
    return cells[cell_index].get();
  }

  bool has_pending_txs() const { return true; }

  bool is_ca_enabled() const { return false; }

  void activate_cells(bounded_bitset<MAX_NOF_CELLS> activ_bitmap) {}

  void handle_sr_indication(const sr_indication_message& msg);

  void handle_reconfiguration_request(const sched_ue_reconfiguration_message& msg) {}

private:
  static const size_t MAX_CELLS = 4;

  std::array<std::unique_ptr<ue_carrier>, MAX_CELLS> cells;

  sr_indication_message last_sr;
};

using ue_map_t = du_ue_list<std::unique_ptr<ue> >;

} // namespace srsgnb

#endif // SRSGNB_UE_H
