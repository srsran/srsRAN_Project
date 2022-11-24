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

#include "srsgnb/mac/lcid_dl_sch.h"
#include "srsgnb/mac/mac_pdu_format.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"
#include <queue>

namespace srsgnb {

class dl_logical_channel_manager
{
public:
  dl_logical_channel_manager()
  {
    // SRB0 is always activated.
    set_status(LCID_SRB0, true);
  }

  void set_status(lcid_t lcid, bool active) { channels[lcid].active = active; }

  /// \brief Verifies if logical channel is activated for DL.
  bool is_active(lcid_t lcid) const { return channels[lcid].active; }

  /// \brief Checks whether the UE has pending data.
  /// \remark Excludes data for SRB0 and UE Contention Resolution Identity CE.
  bool has_pending_bytes() const
  {
    return has_pending_ces() or std::any_of(channels.begin() + 1, channels.end(), [](const auto& ch) {
             return ch.active and ch.buf_st > 0;
           });
  }

  /// \brief Checks whether a logical channel has pending data.
  bool has_pending_bytes(lcid_t lcid) const { return pending_bytes(lcid) > 0; }

  /// \brief Checks whether UE has pending CEs to be scheduled.
  /// \remark Excludes UE Contention Resolution Identity CE.
  bool has_pending_ces() const
  {
    return std::any_of(
        pending_ces.begin(), pending_ces.end(), [](const auto& ce) { return ce != lcid_dl_sch_t::UE_CON_RES_ID; });
  }

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
  /// \remark Excludes UE Contention Resolution Identity CE.
  unsigned pending_ce_bytes() const
  {
    unsigned bytes = 0;
    for (const lcid_dl_sch_t& ce : pending_ces) {
      if (ce != lcid_dl_sch_t::UE_CON_RES_ID) {
        bytes += ce.is_var_len_ce() ? get_mac_sdu_required_bytes(ce.sizeof_ce())
                                    : FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + ce.sizeof_ce();
      }
    }
    return bytes;
  }

  /// \brief Checks whether UE has pending UE Contention Resolution Identity CE to be scheduled.
  unsigned pending_ue_con_res_id_ce_bytes() const
  {
    unsigned bytes = 0;
    // UE Contention Resolution Identity is always first in the queue.
    const auto& ce = pending_ces.front();
    if (ce == lcid_dl_sch_t::UE_CON_RES_ID) {
      bytes += FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + ce.sizeof_ce();
    }
    return bytes;
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
    // Give priority to CON RES ID over other CEs.
    if (ce == lcid_dl_sch_t::UE_CON_RES_ID) {
      pending_ces.push_front(ce);
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

} // namespace srsgnb
