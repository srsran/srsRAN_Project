/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "dl_logical_channel_manager.h"
#include "ue_cell.h"
#include "ul_logical_channel_manager.h"
#include "srsgnb/adt/stable_id_map.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/scheduler/mac_scheduler.h"

namespace srsgnb {

class ue
{
public:
  ue(const scheduler_ue_expert_config&        expert_cfg_,
     const cell_configuration&                cell_cfg_common_,
     const sched_ue_creation_request_message& req);
  ue(const ue&)            = delete;
  ue(ue&&)                 = delete;
  ue& operator=(const ue&) = delete;
  ue& operator=(ue&&)      = delete;

  const du_ue_index_t ue_index;
  const rnti_t        crnti;

  void slot_indication(slot_point sl_tx);

  ue_cell* find_cell(du_cell_index_t cell_index)
  {
    srsgnb_assert(cell_index < MAX_NOF_DU_CELLS, "Invalid cell_index={}", cell_index);
    return du_cells[cell_index].get();
  }
  const ue_cell* find_cell(du_cell_index_t cell_index) const
  {
    srsgnb_assert(cell_index < MAX_NOF_DU_CELLS, "Invalid cell_index={}", cell_index);
    return du_cells[cell_index].get();
  }

  /// \brief Fetch UE cell based on UE-specific cell identifier. E.g. PCell corresponds to ue_cell_index==0.
  ue_cell& get_cell(ue_cell_index_t ue_cell_index)
  {
    srsgnb_assert(ue_cell_index < ue_cells.size(), "Invalid cell_index={}", ue_cell_index);
    return *ue_cells[ue_cell_index];
  }
  const ue_cell& get_cell(ue_cell_index_t ue_cell_index) const
  {
    srsgnb_assert(ue_cell_index < ue_cells.size(), "Invalid cell_index={}", ue_cell_index);
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
  void handle_sr_indication() { ul_lc_ch_mgr.handle_sr_indication(); }

  /// \brief Once an UL grant is given, the SR status of the UE must be reset.
  void reset_sr_indication() { ul_lc_ch_mgr.reset_sr_indication(); }

  /// \brief Handles received BSR indication by updating UE UL logical channel states.
  void handle_bsr_indication(const ul_bsr_indication_message& msg) { ul_lc_ch_mgr.handle_bsr_indication(msg); }

  /// \brief Handles MAC CE indication.
  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& msg)
  {
    dl_lc_ch_mgr.handle_mac_ce_indication(msg.ce_lcid);
  }

  /// \brief Handles DL Buffer State indication.
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& msg)
  {
    dl_lc_ch_mgr.handle_dl_buffer_status_indication(msg.lcid, msg.bs);
    if (msg.lcid == LCID_SRB0 and msg.bs > 0) {
      // Enqueue a UE Contention Resolution ID.
      dl_lc_ch_mgr.handle_mac_ce_indication(lcid_dl_sch_t::UE_CON_RES_ID);
    }
  }

  void handle_reconfiguration_request(const sched_ue_reconfiguration_message& msg);

  /// \brief Checks if there are DL pending bytes that are yet to be allocated in a DL HARQ.
  /// This method is faster than computing \c pending_dl_newtx_bytes() > 0.
  /// \remark Excludes SRB0 and UE Contention Resolution Identity CE.
  bool has_pending_dl_newtx_bytes() const { return dl_lc_ch_mgr.has_pending_bytes(); }

  /// \brief Checks if there are DL pending bytes for a specific LCID that are yet to be allocated in a DL HARQ.
  bool has_pending_dl_newtx_bytes(lcid_t lcid) const { return dl_lc_ch_mgr.has_pending_bytes(lcid); }

  /// \brief Computes the number of DL pending bytes that are not already allocated in a DL HARQ. The value is used
  /// to derive the required transport block size for an DL grant.
  /// \remark Excludes SRB0 and UE Contention Resolution Identity CE.
  unsigned pending_dl_newtx_bytes() const;

  /// \brief Computes the number of DL pending bytes that are not already allocated in a DL HARQ for SRB0. The value is
  /// used to derive the required transport block size for an DL grant.
  unsigned pending_dl_srb0_newtx_bytes() const;

  /// \brief Computes the number of UL pending bytes that are not already allocated in a UL HARQ. The value is used
  /// to derive the required transport block size for an UL grant.
  unsigned pending_ul_newtx_bytes() const;

  /// \brief Defines the list of subPDUs, including LCID and payload size, that will compose the transport block.
  /// \return Returns the number of bytes reserved in the TB for subPDUs (other than padding).
  /// \remark Excludes SRB0 and UE Contention Resolution Identity CE.
  unsigned build_dl_transport_block_info(dl_msg_tb_info& tb_info, unsigned tb_size_bytes);

  /// \brief Defines the list of subPDUs, including LCID and payload size, that will compose the transport block for
  /// SRB0.
  /// \return Returns the number of bytes reserved in the TB for subPDUs (other than padding).
  unsigned build_dl_srb0_transport_block_info(dl_msg_tb_info& tb_info, unsigned tb_size_bytes);

private:
  /// Expert config parameters used for UE scheduler.
  const scheduler_ue_expert_config& expert_cfg;

  /// Cell configuration. This is common to all UEs within the same cell.
  const cell_configuration& cell_cfg_common;

  /// List of \c mac-LogicalChannelConfig, TS 38.331; \ref sched_ue_creation_request_message.
  std::vector<logical_channel_config> log_channels_configs;
  /// \c schedulingRequestToAddModList, TS 38.331; \ref sched_ue_creation_request_message.
  std::vector<scheduling_request_to_addmod> sched_request_configs;

  /// List of UE cells indexed by \c du_cell_index_t. If an element is null, it means that the DU cell is not
  /// configured to be used by the UE.
  std::array<std::unique_ptr<ue_cell>, MAX_NOF_DU_CELLS> du_cells;

  /// List of UE cells indexed by \c ue_cell_index_t. The size of the list is equal to the number of cells aggregated
  /// and configured for the UE. PCell corresponds to ue_cell_index=0. the first SCell corresponds to ue_cell_index=1,
  /// etc.
  static_vector<ue_cell*, MAX_NOF_DU_CELLS> ue_cells;

  /// UE DL Logical Channel Manager.
  dl_logical_channel_manager dl_lc_ch_mgr;

  /// UE UL Logical Channel Manager.
  ul_logical_channel_manager ul_lc_ch_mgr;
};

/// Container that stores all scheduler UEs.
using ue_list = stable_id_map<du_ue_index_t, ue, MAX_NOF_DU_UES>;

} // namespace srsgnb
