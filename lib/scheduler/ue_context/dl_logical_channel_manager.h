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

#include "../config/logical_channel_list_config.h"
#include "../slicing/ran_slice_id.h"
#include "srsran/adt/intrusive_list.h"
#include "srsran/adt/ring_buffer.h"
#include "srsran/mac/mac_pdu_format.h"
#include "srsran/ran/logical_channel/lcid_dl_sch.h"
#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/support/math/moving_averager.h"
#include <queue>
#include <variant>

namespace srsran {

class dl_logical_channel_manager
{
public:
  /// Holds the MAC CE information.
  struct mac_ce_info {
    /// LCID of the MAC CE.
    lcid_dl_sch_t ce_lcid;
    /// Holds payload of CE except UE Contention Resolution Identity.
    std::variant<ta_cmd_ce_payload, dummy_ce_payload> ce_payload;
  };

  dl_logical_channel_manager(subcarrier_spacing              scs_common,
                             bool                            starts_in_fallback,
                             logical_channel_config_list_ptr log_channels_configs);

  /// Signal the start of a new slot.
  void slot_indication();

  /// \brief Deactivate all bearers.
  void deactivate();

  /// Set UE fallback state.
  void set_fallback_state(bool enter_fallback);

  /// Setups up an observer for DL pending data for a given RAN slice.
  void register_ran_slice(ran_slice_id_t slice_id);

  /// Assign a RAN slice to a logical channel.
  void set_lcid_ran_slice(lcid_t lcid, ran_slice_id_t slice_id);

  /// Detach logical channel from previously set RAN slice.
  void reset_lcid_ran_slice(lcid_t lcid);

  /// Remove RAN slice and detach all associated logical channels.
  void deregister_ran_slice(ran_slice_id_t slice_id);

  /// Determines whether a RAN slice has at least one bearer associated with it.
  bool has_slice(ran_slice_id_t slice_id) const
  {
    unsigned idx = slice_id.value();
    return idx < slice_lcid_list_lookup.size() and not slice_lcid_list_lookup[idx].empty();
  }

  /// Get the RAN slice ID associated with a logical channel.
  std::optional<ran_slice_id_t> get_slice_id(lcid_t lcid) const { return channels[lcid].slice_id; }

  /// \brief Update the configurations of the provided lists of bearers.
  void configure(logical_channel_config_list_ptr log_channels_configs);

  /// \brief Verifies if logical channel is activated for DL.
  bool is_active(lcid_t lcid) const { return lcid <= LCID_MAX_DRB and channels[lcid].active; }

  /// \brief Check whether the UE has pending data, given its current state.
  bool has_pending_bytes() const
  {
    if (fallback_state) {
      return is_con_res_id_pending() or has_pending_bytes(LCID_SRB0) or has_pending_bytes(LCID_SRB1);
    }
    return has_pending_ces() or std::any_of(sorted_channels.begin(), sorted_channels.end(), [this](lcid_t lcid) {
             return lcid != LCID_SRB0 and channels[lcid].active and channels[lcid].buf_st > 0;
           });
  }

  /// \brief Check whether the UE has pending data in the provided RAN slice.
  bool has_pending_bytes(ran_slice_id_t slice_id) const;

  /// \brief Checks whether a logical channel has pending data.
  bool has_pending_bytes(lcid_t lcid) const { return channels[lcid].active and channels[lcid].buf_st > 0; }

  /// \brief Checks whether a ConRes CE is pending for transmission.
  bool is_con_res_id_pending() const { return pending_con_res_id; }

  /// \brief Checks whether UE has pending CEs to be scheduled (ConRes excluded).
  bool has_pending_ces() const { return pending_con_res_id or not pending_ces.empty(); }

  /// \brief Calculates total number of DL bytes, including MAC header overhead, and without taking into account
  /// the UE state.
  unsigned total_pending_bytes() const;

  /// \brief Calculates number of DL pending bytes, including MAC header overhead, and taking UE state into account.
  unsigned pending_bytes() const;

  /// Calculates the number of DL pending bytes, including MAC header overhead, for a RAN slice.
  unsigned pending_bytes(ran_slice_id_t slice_id) const;

  /// \brief Returns the UE pending CEs' bytes to be scheduled, if any.
  unsigned pending_ce_bytes() const
  {
    unsigned bytes = pending_con_res_ce_bytes();
    for (const auto& ce : pending_ces) {
      bytes += ce.ce_lcid.is_var_len_ce() ? get_mac_sdu_required_bytes(ce.ce_lcid.sizeof_ce())
                                          : FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + ce.ce_lcid.sizeof_ce();
    }
    return bytes;
  }

  /// \brief Checks whether UE has pending UE Contention Resolution Identity CE to be scheduled.
  unsigned pending_con_res_ce_bytes() const
  {
    static const auto ce_size = lcid_dl_sch_t{lcid_dl_sch_t::UE_CON_RES_ID}.sizeof_ce();
    return is_con_res_id_pending() ? FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + ce_size : 0;
  }

  /// \brief Last DL buffer status for given LCID (MAC subheader included).
  unsigned pending_bytes(lcid_t lcid) const
  {
    return is_active(lcid) ? get_mac_sdu_required_bytes(channels[lcid].buf_st) : 0;
  }

  /// \brief Average bit rate, in bps, for a given LCID.
  double average_bit_rate(lcid_t lcid) const
  {
    return not is_srb(lcid) and is_active(lcid) and channels[lcid].avg_bytes_per_slot.size() > 0
               ? channels[lcid].avg_bytes_per_slot.average() * 8 * slots_per_sec
               : 0.0;
  }

  slot_point hol_toa(lcid_t lcid) const { return is_active(lcid) ? channels[lcid].hol_toa : slot_point{}; }

  /// \brief Update DL buffer status for a given LCID.
  void handle_dl_buffer_status_indication(lcid_t lcid, unsigned buffer_status, slot_point hol_toa = {})
  {
    // We apply this limit to avoid potential overflows.
    static constexpr unsigned max_buffer_status = 1U << 24U;
    srsran_sanity_check(lcid < MAX_NOF_RB_LCIDS, "Max LCID value 32 exceeded");
    channels[lcid].buf_st  = std::min(buffer_status, max_buffer_status);
    channels[lcid].hol_toa = hol_toa;
  }

  /// \brief Enqueue new MAC CE to be scheduled.
  /// \return True if the MAC CE was enqueued successfully, false if the queue was full.
  [[nodiscard]] bool handle_mac_ce_indication(const mac_ce_info& ce);

  /// \brief Allocates highest priority MAC SDU within space of \c rem_bytes bytes. Updates \c lch_info with allocated
  /// bytes for the MAC SDU (no MAC subheader).
  /// \return Allocated bytes for MAC SDU (with subheader).
  unsigned allocate_mac_sdu(dl_msg_lc_info& lch_info, unsigned rem_bytes, lcid_t lcid = INVALID_LCID);

  /// \brief Allocates next MAC CE within space of \c rem_bytes bytes. Updates \c lch_info with allocated bytes for the
  /// MAC CE.
  /// \return Allocated bytes for MAC CE (with subheader).
  /// \remark Excludes UE Contention Resolution Identity CE.
  unsigned allocate_mac_ce(dl_msg_lc_info& lch_info, unsigned rem_bytes);

  /// \brief Allocates UE Contention Resolution Identity MAC CE within space of \c rem_bytes bytes. Updates \c lch_info
  /// with allocated bytes for the MAC CE.
  /// \return Allocated bytes for UE Contention Resolution Identity MAC CE (with subheader).
  unsigned allocate_ue_con_res_id_mac_ce(dl_msg_lc_info& lch_info, unsigned rem_bytes);

  /// \brief Returns a list of LCIDs sorted based on decreasing order of priority.
  span<const lcid_t> get_prioritized_logical_channels() const { return sorted_channels; }

private:
  struct channel_context : public intrusive_double_linked_list_element<> {
    /// Whether the configured logical channel is currently active.
    bool active = false;
    /// DL Buffer status of this logical channel.
    unsigned buf_st = 0;
    /// Bytes-per-slot average for this logical channel.
    moving_averager<unsigned> avg_bytes_per_slot;
    /// Current slot sched bytes.
    unsigned last_sched_bytes = 0;
    /// Head-of-line (HOL) time-of-arrival
    slot_point hol_toa;
    /// Slice associated with this channel.
    std::optional<ran_slice_id_t> slice_id;

    void reset();
  };

  /// \brief Returns the next highest priority LCID. The prioritization policy is implementation-defined.
  lcid_t get_max_prio_lcid() const;

  /// \brief Updates DL Buffer State for a given LCID based on available space.
  unsigned allocate_mac_sdu(dl_msg_lc_info& subpdu, lcid_t lcid, unsigned rem_bytes);

  // Number of slots per second given the used SCS. Parameter used to compute bit rates.
  const unsigned slots_per_sec;

  // List of UE-dedicated logical channel configurations.
  logical_channel_config_list_ptr channel_configs;

  // State of configured channels.
  std::array<channel_context, MAX_NOF_RB_LCIDS> channels;

  // List of active logical channel IDs sorted in decreasing order of priority. i.e. first element has the highest
  // priority.
  std::vector<lcid_t> sorted_channels;

  // Mapping of RAN slice ID to the list of associated LCIDs.
  std::vector<intrusive_double_linked_list<channel_context>> slice_lcid_list_lookup;

  // Whether the UE is in fallback (no DRB tx).
  bool fallback_state = false;

  // Whether a CON RES CE needs to be sent.
  bool pending_con_res_id{false};

  // List of pending CEs except UE Contention Resolution Identity.
  ring_buffer<mac_ce_info> pending_ces;
};

/// \brief Allocate MAC SDUs and corresponding MAC subPDU subheaders.
/// \param[in] tb_info TB on which MAC subPDUs will be stored.
/// \param[in] lch_mng UE DL logical channel manager.
/// \param[in] total_tbs available space in bytes for subPDUs.
/// \param[in] lcid if provided, LCID of the logical channel to be allocated. Otherwise, the LCID with higher priority
/// is chosen.
/// \return Total number of bytes allocated (including MAC subheaders).
unsigned allocate_mac_sdus(dl_msg_tb_info&             tb_info,
                           dl_logical_channel_manager& lch_mng,
                           unsigned                    total_tbs,
                           lcid_t                      lcid = INVALID_LCID);

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
