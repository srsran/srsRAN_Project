/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/mac/lcid_dl_sch.h"
#include "srsran/mac/mac_pdu_format.h"
#include "srsran/scheduler/config/logical_channel_config.h"
#include "srsran/scheduler/scheduler_slot_handler.h"
#include <queue>

namespace srsran {

class dl_logical_channel_manager
{
public:
  dl_logical_channel_manager();

  /// \brief Activate/Deactivate Bearer.
  void set_status(lcid_t lcid, bool active) { channels[lcid].active = active; }

  /// \brief Update the configurations of the provided lists of bearers.
  void configure(span<const logical_channel_config> log_channels_configs);

  /// \brief Verifies if logical channel is activated for DL.
  bool is_active(lcid_t lcid) const { return channels[lcid].active; }

  /// \brief Checks whether the UE has pending data.
  /// \remark Excludes data for SRB0.
  bool has_pending_bytes() const
  {
    return has_pending_ces() or std::any_of(channels.begin() + 1, channels.end(), [](const auto& ch) {
             return ch.active and ch.buf_st > 0;
           });
  }

  /// \brief Checks whether a logical channel has pending data.
  bool has_pending_bytes(lcid_t lcid) const { return pending_bytes(lcid) > 0; }

  /// \brief Checks whether a ConRes CE is pending for transmission.
  /// \remark ConRes CE is only sent when there is pending data for SRB0 or SRB1.
  bool is_con_res_id_pending() const
  {
    return (pending_con_res_id and (has_pending_bytes(LCID_SRB0) or has_pending_bytes(LCID_SRB1)));
  }

  /// \brief Checks whether UE has pending CEs to be scheduled.
  bool has_pending_ces() const { return is_con_res_id_pending() or not pending_ces.empty(); }

  /// \brief Calculates total number of DL bytes, including MAC header overhead.
  /// \remark Excludes data for SRB0 and UE Contention Resolution Identity CE.
  unsigned pending_bytes() const
  {
    unsigned bytes = pending_ce_bytes();
    // Skip index 0 ==> SRB0.
    for (unsigned i = 1; i <= MAX_LCID; ++i) {
      bytes += pending_bytes((lcid_t)i);
    }
    return bytes;
  }

  /// \brief Checks whether UE has pending CEs to be scheduled.
  unsigned pending_ce_bytes() const
  {
    unsigned bytes = pending_ue_con_res_id_ce_bytes();
    for (const lcid_dl_sch_t& ce : pending_ces) {
      bytes += ce.is_var_len_ce() ? get_mac_sdu_required_bytes(ce.sizeof_ce())
                                  : FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + ce.sizeof_ce();
    }
    return bytes;
  }

  /// \brief Checks whether UE has pending UE Contention Resolution Identity CE to be scheduled.
  unsigned pending_ue_con_res_id_ce_bytes() const
  {
    static const auto ce_size = lcid_dl_sch_t{lcid_dl_sch_t::UE_CON_RES_ID}.sizeof_ce();
    return is_con_res_id_pending() ? FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + ce_size : 0;
  }

  /// \brief Last DL buffer status for given LCID (MAC subheader included).
  unsigned pending_bytes(lcid_t lcid) const
  {
    return is_active(lcid) ? get_mac_sdu_required_bytes(channels[lcid].buf_st) : 0;
  }

  /// \brief Update DL buffer status for a given LCID.
  void handle_dl_buffer_status_indication(lcid_t lcid, unsigned buffer_status)
  {
    channels[lcid].buf_st = buffer_status;
  }

  /// \brief Enqueue new MAC CE to be scheduled.
  void handle_mac_ce_indication(lcid_dl_sch_t ce)
  {
    if (ce == lcid_dl_sch_t::UE_CON_RES_ID) {
      pending_con_res_id = true;
    } else {
      pending_ces.push_back(ce);
    }
  }

  /// \brief Allocates highest priority MAC SDU within space of \c rem_bytes bytes. Updates \c lch_info with allocated
  /// bytes for the MAC SDU (no MAC subheader).
  /// \return Allocated bytes for MAC SDU (with subheader).
  unsigned allocate_mac_sdu(dl_msg_lc_info& lch_info, unsigned rem_bytes);

  /// \brief Allocates next MAC CE within space of \c rem_bytes bytes. Updates \c lch_info with allocated bytes for the
  /// MAC CE.
  /// \return Allocated bytes for MAC CE (with subheader).
  /// \remark Excludes UE Contention Resolution Identity CE.
  unsigned allocate_mac_ce(dl_msg_lc_info& lch_info, unsigned rem_bytes);

  /// \brief Allocates UE Contention Resolution Identity MAC CE within space of \c rem_bytes bytes. Updates \c lch_info
  /// with allocated bytes for the MAC CE.
  /// \return Allocated bytes for UE Contention Resolution Identity MAC CE (with subheader).
  unsigned allocate_ue_con_res_id_mac_ce(dl_msg_lc_info& lch_info, unsigned rem_bytes);

private:
  struct channel_context {
    bool active = false;
    /// DL Buffer status of this logical channel.
    unsigned buf_st = 0;
  };

  /// \brief Returns the next highest priority LCID. The prioritization policy is implementation-defined.
  lcid_t get_max_prio_lcid() const;

  /// \brief Updates DL Buffer State for a given LCID based on available space.
  unsigned allocate_mac_sdu(dl_msg_lc_info& subpdu, lcid_t lcid, unsigned rem_bytes);

  std::array<channel_context, MAX_NOF_RB_LCIDS> channels;

  bool pending_con_res_id{false};

  /// \brief List of pending CEs except UE Contention Resolution Identity.
  std::deque<lcid_dl_sch_t> pending_ces;
};

/// \brief Allocate MAC SDUs and corresponding MAC subPDU subheaders.
/// \param[in] tb_info TB on which MAC subPDUs will be stored.
/// \param[in] lch_mng UE DL logical channel manager.
/// \param[in] total_tbs available space in bytes for subPDUs.
/// \return Total number of bytes allocated (including MAC subheaders).
unsigned allocate_mac_sdus(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs);

/// \brief Allocate MAC subPDUs for pending MAC CEs.
/// \param[in] tb_info TB on which MAC subPDUs will be stored.
/// \param[in] lch_mng UE DL logical channel manager.
/// \param[in] total_tbs available space in bytes for subPDUs.
/// \return Total number of bytes allocated (including MAC subheaders).
/// \remark Excludes UE Contention Resolution Identity CE.
unsigned allocate_mac_ces(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs);

/// \brief Allocate MAC subPDUs for pending UE Contention Resolution Identity MAC CE.
/// \param[in] tb_info TB on which MAC subPDUs will be stored.
/// \param[in] lch_mng UE DL logical channel manager.
/// \param[in] total_tbs available space in bytes for subPDUs.
/// \return Total number of bytes allocated (including MAC subheaders).
unsigned
allocate_ue_con_res_id_mac_ce(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs);

} // namespace srsran
