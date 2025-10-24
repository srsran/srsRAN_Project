/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "dl_logical_channel_manager.h"
#include "ta_manager.h"
#include "ue_cell.h"
#include "ue_drx_controller.h"
#include "ul_logical_channel_manager.h"
#include "srsran/ran/du_types.h"
#include "srsran/scheduler/mac_scheduler.h"

namespace srsran {

/// Parameters used to create a UE.
struct ue_creation_command {
  const ue_configuration&   cfg;
  bool                      starts_in_fallback;
  cell_harq_manager&        pcell_harq_pool;
  std::optional<slot_point> ul_ccch_slot_rx;
};

/// Parameters used to reconfigure a UE.
struct ue_reconf_command {
  const ue_configuration& cfg;
};

class ue
{
public:
  ue(const ue_creation_command& cmd);
  ue(const ue&)            = delete;
  ue(ue&&)                 = delete;
  ue& operator=(const ue&) = delete;
  ue& operator=(ue&&)      = delete;

  const du_ue_index_t ue_index;
  const rnti_t        crnti;

  void slot_indication(slot_point sl_tx);

  void deactivate();

  void release_resources();

  ue_cell* find_cell(du_cell_index_t cell_index)
  {
    srsran_assert(cell_index < MAX_NOF_DU_CELLS, "Invalid cell_index={}", fmt::underlying(cell_index));
    return ue_du_cells[cell_index].get();
  }
  const ue_cell* find_cell(du_cell_index_t cell_index) const
  {
    srsran_assert(cell_index < MAX_NOF_DU_CELLS, "Invalid cell_index={}", fmt::underlying(cell_index));
    return ue_du_cells[cell_index].get();
  }

  /// \brief Fetch UE cell based on UE-specific cell identifier. E.g. PCell corresponds to ue_cell_index==0.
  ue_cell& get_cell(ue_cell_index_t ue_cell_index)
  {
    srsran_assert(ue_cell_index < ue_cells.size(), "Invalid cell_index={}", fmt::underlying(ue_cell_index));
    return *ue_cells[ue_cell_index];
  }
  const ue_cell& get_cell(ue_cell_index_t ue_cell_index) const
  {
    srsran_assert(ue_cell_index < ue_cells.size(), "Invalid cell_index={}", fmt::underlying(ue_cell_index));
    return *ue_cells[ue_cell_index];
  }

  /// \brief Fetch UE PCell.
  ue_cell&       get_pcell() { return *ue_cells[0]; }
  const ue_cell& get_pcell() const { return *ue_cells[0]; }

  /// \brief Number of cells configured for the UE.
  unsigned nof_cells() const { return ue_cells.size(); }

  /// \brief Returns dedicated configuration for the UE.
  const ue_configuration* ue_cfg_dedicated() const { return ue_ded_cfg; }

  bool is_ca_enabled() const { return ue_cells.size() > 1; }

  void activate_cells(bounded_bitset<MAX_NOF_DU_CELLS> activ_bitmap) {}

  /// \brief Handle received SR indication.
  void handle_sr_indication() { ul_lc_ch_mgr.handle_sr_indication(); }

  /// \brief Handles received BSR indication by updating UE UL logical channel states.
  void handle_bsr_indication(const ul_bsr_indication_message& msg) { ul_lc_ch_mgr.handle_bsr_indication(msg); }

  /// \brief Handles received N_TA update indication by forwarding it to Timing Advance manager.
  void handle_ul_n_ta_update_indication(du_cell_index_t cell_index, float ul_sinr, phy_time_unit n_ta_diff)
  {
    const ue_cell* ue_cc = find_cell(cell_index);
    ta_mgr.handle_ul_n_ta_update_indication(ue_cc->cfg().tag_id(), n_ta_diff.to_Tc(), ul_sinr);
  }

  /// \brief Handles MAC CE indication.
  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& msg)
  {
    if (not dl_lc_ch_mgr.handle_mac_ce_indication({.ce_lcid = msg.ce_lcid, .ce_payload = dummy_ce_payload{}})) {
      logger.warning("Dropped MAC CE, queue is full.");
    }
  }

  /// Called when a new UE configuration is passed to the scheduler, as part of the RRC Reconfiguration procedure.
  void handle_reconfiguration_request(const ue_reconf_command& params, bool reestablished_);

  /// Called when the UE confirms that it applied the new configuration.
  void handle_config_applied();

  /// \brief Handles DL Buffer State indication.
  void handle_dl_buffer_state_indication(lcid_t lcid, unsigned bs, slot_point hol_toa = {});

  /// \brief Computes the number of UL pending bytes that are not already allocated in a UL HARQ. The value is used
  /// to derive the required transport block size for an UL grant.
  unsigned pending_ul_newtx_bytes() const;

  /// \brief Retrieves UE DRX controller.
  ue_drx_controller& drx_controller() { return drx; }

  /// \brief UE DL logical channels.
  const dl_logical_channel_manager& dl_logical_channels() const { return dl_lc_ch_mgr; }
  dl_logical_channel_manager&       dl_logical_channels() { return dl_lc_ch_mgr; }

  /// \brief UE UL logical channels.
  const ul_logical_channel_manager& ul_logical_channels() const { return ul_lc_ch_mgr; }
  ul_logical_channel_manager&       ul_logical_channels() { return ul_lc_ch_mgr; }

private:
  /// Update UE configuration.
  void set_config(const ue_configuration& new_cfg, std::optional<slot_point> msg3_rx_slot = std::nullopt);

  // Expert config parameters used for UE scheduler.
  const scheduler_ue_expert_config& expert_cfg;
  // Cell configuration. This is common to all UEs within the same cell.
  const cell_configuration& cell_cfg_common;
  // Dedicated configuration for the UE.
  const ue_configuration* ue_ded_cfg = nullptr;
  cell_harq_manager&      pcell_harq_pool;
  srslog::basic_logger&   logger;

  /// List of UE cells indexed by \c du_cell_index_t. If an element is null, it means that the DU cell is not
  /// configured to be used by the UE.
  std::array<std::unique_ptr<ue_cell>, MAX_NOF_DU_CELLS> ue_du_cells;

  /// List of UE cells indexed by \c ue_cell_index_t. The size of the list is equal to the number of cells aggregated
  /// and configured for the UE. PCell corresponds to ue_cell_index=0. the first SCell corresponds to ue_cell_index=1,
  /// etc.
  static_vector<ue_cell*, MAX_NOF_DU_CELLS> ue_cells;

  /// UE DL Logical Channel Manager.
  dl_logical_channel_manager dl_lc_ch_mgr;

  /// UE UL Logical Channel Manager.
  ul_logical_channel_manager ul_lc_ch_mgr;

  slot_point last_sl_tx;

  /// UE Timing Advance Manager.
  ta_manager ta_mgr;

  // Controller of DRX active timer.
  ue_drx_controller drx;
};

} // namespace srsran
