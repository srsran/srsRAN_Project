/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "ul_logical_channel_manager.h"
#include "srsran/ran/du_types.h"
#include "srsran/scheduler/mac_scheduler.h"

namespace srsran {

/// Parameters used to create a UE.
struct ue_creation_command {
  const ue_configuration& cfg;
  bool                    starts_in_fallback;
  harq_timeout_handler&   harq_timeout_notifier;
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

  ue_cell* find_cell(du_cell_index_t cell_index)
  {
    srsran_assert(cell_index < MAX_NOF_DU_CELLS, "Invalid cell_index={}", cell_index);
    return ue_du_cells[cell_index].get();
  }
  const ue_cell* find_cell(du_cell_index_t cell_index) const
  {
    srsran_assert(cell_index < MAX_NOF_DU_CELLS, "Invalid cell_index={}", cell_index);
    return ue_du_cells[cell_index].get();
  }

  /// \brief Fetch UE cell based on UE-specific cell identifier. E.g. PCell corresponds to ue_cell_index==0.
  ue_cell& get_cell(ue_cell_index_t ue_cell_index)
  {
    srsran_assert(ue_cell_index < ue_cells.size(), "Invalid cell_index={}", ue_cell_index);
    return *ue_cells[ue_cell_index];
  }
  const ue_cell& get_cell(ue_cell_index_t ue_cell_index) const
  {
    srsran_assert(ue_cell_index < ue_cells.size(), "Invalid cell_index={}", ue_cell_index);
    return *ue_cells[ue_cell_index];
  }

  /// \brief Fetch UE PCell.
  ue_cell&       get_pcell() { return *ue_cells[0]; }
  const ue_cell& get_pcell() const { return *ue_cells[0]; }

  /// \brief Number of cells configured for the UE.
  unsigned nof_cells() const { return ue_cells.size(); }

  bool is_ca_enabled() const { return ue_cells.size() > 1; }

  void activate_cells(bounded_bitset<MAX_NOF_DU_CELLS> activ_bitmap) {}

  /// \brief Handle received SR indication.
  void handle_sr_indication()
  {
    // Reception of SR means that the UE has applied its dedicated configuration.
    ue_cells[0]->set_fallback_state(false);
    ul_lc_ch_mgr.handle_sr_indication();
  }

  /// \brief Once an UL grant is given, the SR status of the UE must be reset.
  void reset_sr_indication() { ul_lc_ch_mgr.reset_sr_indication(); }

  /// \brief Handles received BSR indication by updating UE UL logical channel states.
  void handle_bsr_indication(const ul_bsr_indication_message& msg) { ul_lc_ch_mgr.handle_bsr_indication(msg); }

  /// \brief Handles received N_TA update indication by forwarding it to Timing Advance manager.
  void handle_ul_n_ta_update_indication(du_cell_index_t cell_index, float ul_sinr, phy_time_unit n_ta_diff)
  {
    const ue_cell* ue_cc = find_cell(cell_index);
    ta_mgr.handle_ul_n_ta_update_indication(ue_cc->cfg().cfg_dedicated().tag_id, n_ta_diff.to_Tc(), ul_sinr);
  }

  /// \brief Handles MAC CE indication.
  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& msg)
  {
    dl_lc_ch_mgr.handle_mac_ce_indication({.ce_lcid = msg.ce_lcid, .ce_payload = dummy_ce_payload{}});
  }

  /// \brief Handles DL Buffer State indication.
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& msg)
  {
    dl_lc_ch_mgr.handle_dl_buffer_status_indication(msg.lcid, msg.bs);
  }

  void handle_reconfiguration_request(const ue_reconf_command& params);

  /// \brief Checks if there are DL pending bytes that are yet to be allocated in a DL HARQ.
  /// This method is faster than computing \c pending_dl_newtx_bytes() > 0.
  /// \remark Excludes SRB0 and UE Contention Resolution Identity CE.
  bool has_pending_dl_newtx_bytes() const { return dl_lc_ch_mgr.has_pending_bytes(); }

  /// \brief Checks if there are DL pending bytes for a specific LCID that are yet to be allocated in a DL HARQ.
  bool has_pending_dl_newtx_bytes(lcid_t lcid) const { return dl_lc_ch_mgr.has_pending_bytes(lcid); }

  /// \brief Whether MAC ConRes CE is pending.
  bool is_conres_ce_pending() const { return dl_lc_ch_mgr.is_con_res_id_pending(); }

  /// \brief Computes the number of DL pending bytes that are not already allocated in a DL HARQ. The value is used
  /// to derive the required transport block size for an DL grant.
  /// param[in] lcid If the LCID is provided, the method will return the number of pending bytes for that LCID.
  ///           Otherwise it will return the sum of all LCIDs pending bytes, excluding SRB0.
  /// \return The number of DL pending bytes that are not already allocated in a DL HARQ.
  unsigned pending_dl_newtx_bytes(lcid_t lcid = lcid_t::INVALID_LCID) const;

  /// \brief Computes the number of DL pending bytes that are not already allocated in a DL HARQ for SRB0 or SRB1. The
  /// value is used to derive the required transport block size for an DL grant.
  /// param[in] is_srb0 tells whether to computes the number of DL pending bytes for SRB0, if true; for SRB1 otherwise.
  /// \return The number of DL pending bytes.
  unsigned pending_dl_srb0_or_srb1_newtx_bytes(bool is_srb0) const;

  /// \brief Computes the number of UL pending bytes that are not already allocated in a UL HARQ. The value is used
  /// to derive the required transport block size for an UL grant.
  unsigned pending_ul_newtx_bytes() const;

  /// \brief Returns whether a SR indication handling is pending.
  bool has_pending_sr() const;

  /// \brief Defines the list of subPDUs, including LCID and payload size, that will compose the transport block.
  /// \return Returns the number of bytes reserved in the TB for subPDUs (other than padding).
  /// \remark Excludes SRB0.
  unsigned build_dl_transport_block_info(dl_msg_tb_info& tb_info, unsigned tb_size_bytes, lcid_t lcid = INVALID_LCID);

  /// \brief Defines the list of subPDUs, including LCID and payload size, that will compose the transport block for
  /// SRB0 or for SRB1 in fallback mode.
  /// It includes the UE Contention Resolution Identity CE if it is pending.
  /// \return Returns the number of bytes reserved in the TB for subPDUs (other than padding).
  unsigned build_dl_fallback_transport_block_info(dl_msg_tb_info& tb_info, unsigned tb_size_bytes, bool is_srb0);

private:
  /// Expert config parameters used for UE scheduler.
  const scheduler_ue_expert_config& expert_cfg;

  /// Cell configuration. This is common to all UEs within the same cell.
  const cell_configuration& cell_cfg_common;

  /// Dedicated configuration for the UE.
  const ue_configuration* ue_ded_cfg = nullptr;

  /// Notifier used by HARQ processes to signal timeouts due to undetected HARQ ACKs/CRCs.
  ue_harq_timeout_notifier harq_timeout_notif;

  srslog::basic_logger& logger;

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

  /// UE Timing Advance Manager.
  ta_manager ta_mgr;
};

} // namespace srsran
