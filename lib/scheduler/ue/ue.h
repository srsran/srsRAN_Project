/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UE_H
#define SRSGNB_UE_H

#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/stable_id_map.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/scheduler/mac_scheduler.h"
#include "ue_configuration.h"

namespace srsgnb {

class ue_carrier
{
public:
  ue_carrier(du_ue_index_t                                ue_index,
             rnti_t                                       crnti_val,
             du_cell_index_t                              cell_index,
             const cell_configuration&                    cell_cfg_common_,
             const serving_cell_ue_configuration_request& ue_serv_cell) :
    ue_index(ue_index), cell_index(cell_index), crnti_(crnti_val), ue_cfg(cell_cfg_common_, ue_serv_cell)
  {}

  const du_ue_index_t   ue_index;
  const du_cell_index_t cell_index;

  rnti_t rnti() const { return crnti_; }

  bwp_id_t active_bwp_id() const { return to_bwp_id(0); }
  bool     is_active() const { return true; }

  const ue_cell_configuration& cfg() const { return ue_cfg; }

private:
  rnti_t                crnti_;
  ue_cell_configuration ue_cfg;
};

class ue
{
public:
  ue(const cell_configuration& cell_cfg_common_, const sched_ue_creation_request_message& req) :
    ue_index(req.ue_index), crnti(req.crnti), cell_cfg_common(cell_cfg_common_)
  {
    cells[0] = std::make_unique<ue_carrier>(ue_index, req.crnti, req.pcell_index, cell_cfg_common, req.serv_cell_cfg);
  }
  ue(const ue&) = delete;
  ue(ue&&)      = delete;
  ue& operator=(const ue&) = delete;
  ue& operator=(ue&&) = delete;

  const du_ue_index_t ue_index;
  const rnti_t        crnti;

  void slot_indication(slot_point sl_tx) {}

  ue_carrier* find_cc(du_cell_index_t cell_index)
  {
    srsran_assert(cell_index < MAX_CELLS, "Invalid cell_index={}", cell_index);
    return cells[cell_index].get();
  }

  const ue_carrier* find_cc(du_cell_index_t cell_index) const
  {
    srsran_assert(cell_index < MAX_CELLS, "Invalid cell_index={}", cell_index);
    return cells[cell_index].get();
  }

  bool has_pending_txs() const { return true; }

  bool is_ca_enabled() const { return false; }

  void activate_cells(bounded_bitset<MAX_NOF_DU_CELLS> activ_bitmap) {}

  void handle_sr_indication(const sr_indication_message& msg);

  void handle_reconfiguration_request(const sched_ue_reconfiguration_message& msg) {}

  span<const du_cell_index_t> ue_cell_indexes() const
  {
    const static static_vector<du_cell_index_t, 1> cells{0};
    return cells;
  }

private:
  static const size_t MAX_CELLS = 4;

  const cell_configuration& cell_cfg_common;

  std::array<std::unique_ptr<ue_carrier>, MAX_CELLS> cells;

  sr_indication_message last_sr;
};

/// Container that stores all scheduler UEs.
using ue_list = stable_id_map<du_ue_index_t, ue, MAX_NOF_DU_UES>;

} // namespace srsgnb

#endif // SRSGNB_UE_H
