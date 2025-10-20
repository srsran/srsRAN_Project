/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../config/logical_channel_list_config.h"
#include "../slicing/ran_slice_id.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/adt/slotted_vector.h"
#include "srsran/adt/soa_table.h"
#include "srsran/mac/mac_pdu_format.h"
#include "srsran/ran/logical_channel/lcid_dl_sch.h"
#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/support/math/moving_averager.h"

namespace srsran {

class ue_dl_logical_channel_repository;

/// System responsible for handling the logical channel configuration and buffer occupancy status of UEs in DL.
class dl_logical_channel_system
{
public:
  /// Holds the MAC CE information.
  struct mac_ce_info {
    /// LCID of the MAC CE.
    lcid_dl_sch_t ce_lcid;
    /// Holds payload of CE except UE Contention Resolution Identity.
    std::variant<ta_cmd_ce_payload, dummy_ce_payload> ce_payload = dummy_ce_payload{0};
  };

  dl_logical_channel_system(subcarrier_spacing max_scs);

  /// Signal the start of a new slot.
  void slot_indication();

  /// Creates a new UE logical channel repository.
  ue_dl_logical_channel_repository create_ue(bool                            starts_in_fallback,
                                             logical_channel_config_list_ptr log_channels_configs);

  /// Number of UEs managed by the system.
  size_t nof_ues() const { return ue_contexts.size(); }

  /// Number of logical channels managed by the system.
  size_t nof_logical_channels() const { return configured_channels.size(); }

private:
  friend class ue_dl_logical_channel_repository;

  struct mac_ce_context {
    mac_ce_info                info;
    std::optional<soa::row_id> next_ue_ce;
  };
  struct qos_context {
    /// Bytes-per-slot average for this logical channel.
    moving_averager<unsigned> avg_bytes_per_slot;
    /// Current slot sched bytes.
    unsigned last_sched_bytes = 0;
  };
  struct channel_context {
    /// Whether the configured logical channel is currently active.
    bool active = false;
    /// DL Buffer status of this logical channel.
    unsigned buf_st = 0;
    /// Head-of-line (HOL) time-of-arrival
    slot_point hol_toa;
    /// In case statistics are being tracked, holds the row in the channel_stats_list table.
    std::optional<soa::row_id> stats_row;
    /// Slice associated with this channel.
    std::optional<ran_slice_id_t> slice_id;
  };
  struct ue_context {
    // List of UE-dedicated logical channel configurations.
    logical_channel_config_list_ptr channel_configs;
    // Whether the UE is in fallback (no DRB tx).
    bool fallback_state = false;
    // Whether a CON RES CE needs to be sent.
    bool pending_con_res_id{false};
    /// Sum of pending CE bytes for this UE.
    uint32_t pending_ce_bytes{0};
    /// Context of channels currently configured. The index of the array matches the LCID.
    slotted_id_table<lcid_t, soa::row_id, MAX_NOF_RB_LCIDS> channels;
    /// List of active logical channel IDs sorted in decreasing order of priority. i.e. first element has the highest
    /// priority.
    std::vector<lcid_t> sorted_channels;
    /// Mapping of RAN slice ID to the list of associated LCIDs.
    slotted_id_vector<ran_slice_id_t, std::pair<unsigned, std::vector<soa::row_id>>> slice_lcid_list_lookup;
    /// Currently enqueued CEs for this UE.
    std::optional<soa::row_id> pending_ces;
  };

  ue_context&       get_ue_ctx(soa::row_id ue_rid) { return ue_contexts.row(ue_rid).at<0>(); }
  const ue_context& get_ue_ctx(soa::row_id ue_rid) const { return ue_contexts.row(ue_rid).at<0>(); }

  void remove_ue(soa::row_id ue_rid);
  void configure(soa::row_id ue_rid, logical_channel_config_list_ptr log_channels_configs);
  void deactivate(soa::row_id ue_rid);

  void set_lcid_ran_slice(soa::row_id ue_rid, lcid_t lcid, ran_slice_id_t slice_id);
  void register_ran_slice(soa::row_id ue_rid, ran_slice_id_t slice_id);
  void deregister_lc_ran_slice(soa::row_id ue_rid, lcid_t lcid);
  void deregister_ran_slice(soa::row_id ue_rid, ran_slice_id_t slice_id);

  void
  handle_dl_buffer_status_indication(soa::row_id ue_row_id, lcid_t lcid, unsigned buffer_status, slot_point hol_toa);
  bool handle_mac_ce_indication(soa::row_id ue_row_id, const mac_ce_info& ce);

  unsigned allocate_mac_sdu(soa::row_id ue_rid, dl_msg_lc_info& subpdu, lcid_t lcid, unsigned rem_bytes);
  unsigned allocate_mac_sdu(soa::row_id ue_rid, dl_msg_lc_info& lch_info, unsigned rem_bytes, lcid_t lcid);
  unsigned allocate_mac_ce(soa::row_id ue_rid, dl_msg_lc_info& lch_info, unsigned rem_bytes);
  unsigned allocate_ue_con_res_id_mac_ce(soa::row_id ue_rid, dl_msg_lc_info& lch_info, unsigned rem_bytes);

  // Helper inlined methods
  const channel_context* find_ch_ctx(const ue_context& ue_ctx, lcid_t lcid) const
  {
    return ue_ctx.channels.contains(lcid) ? &configured_channels.row(ue_ctx.channels[lcid]).at<0>() : nullptr;
  }
  const channel_context* find_active_ch_ctx(const ue_context& ue_ctx, lcid_t lcid) const
  {
    auto* ch_ctx = find_ch_ctx(ue_ctx, lcid);
    return ch_ctx != nullptr and ch_ctx->active ? ch_ctx : nullptr;
  }
  bool has_pending_bytes(const ue_context& ue_ctx, lcid_t lcid) const
  {
    auto* ch_ctx = find_active_ch_ctx(ue_ctx, lcid);
    return ch_ctx != nullptr and ch_ctx->active and ch_ctx->buf_st > 0;
  }
  unsigned pending_bytes(const ue_context& ue_ctx, lcid_t lcid) const
  {
    const auto* ch_ctx = find_active_ch_ctx(ue_ctx, lcid);
    return ch_ctx != nullptr ? get_mac_sdu_required_bytes(ch_ctx->buf_st) : 0;
  }
  unsigned        total_pending_bytes(const ue_context& ue_ctx) const;
  static unsigned pending_con_res_ce_bytes(const ue_context& ue_ctx)
  {
    static const auto ce_size = lcid_dl_sch_t{lcid_dl_sch_t::UE_CON_RES_ID}.sizeof_ce();
    return ue_ctx.pending_con_res_id ? FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + ce_size : 0;
  }
  unsigned pending_ce_bytes(const ue_context& ue_ctx) const;

  /// \brief Returns the next highest priority LCID. The prioritization policy is implementation-defined.
  lcid_t get_max_prio_lcid(soa::row_id ue_rid) const;

  /// Number of slots per second given the used SCS. Parameter used to compute bit rates.
  const unsigned slots_per_sec;

  /// List of MAC CEs pending transmission.
  soa::table<mac_ce_context> pending_ces;

  /// Logical channels tracking bit-rates for QoS
  soa::table<qos_context> qos_channels;

  /// Active DL logical channels.
  soa::table<channel_context> configured_channels;

  /// List of UE contexts.
  soa::table<ue_context> ue_contexts;
};

class ue_dl_logical_channel_repository
{
  using mac_ce_info     = dl_logical_channel_system::mac_ce_info;
  using ue_context      = dl_logical_channel_system::ue_context;
  using channel_context = dl_logical_channel_system::channel_context;

  ue_dl_logical_channel_repository(dl_logical_channel_system& parent_, soa::row_id ue_row_) :
    parent(&parent_), ue_row_id(ue_row_)
  {
  }

public:
  ue_dl_logical_channel_repository()                                        = default;
  ue_dl_logical_channel_repository(const ue_dl_logical_channel_repository&) = delete;
  ue_dl_logical_channel_repository(ue_dl_logical_channel_repository&& other) noexcept :
    parent(std::exchange(other.parent, nullptr)), ue_row_id(other.ue_row_id)
  {
  }
  ue_dl_logical_channel_repository& operator=(const ue_dl_logical_channel_repository&) = delete;
  ue_dl_logical_channel_repository& operator=(ue_dl_logical_channel_repository&& other) noexcept
  {
    parent    = std::exchange(other.parent, nullptr);
    ue_row_id = other.ue_row_id;
    return *this;
  }
  ~ue_dl_logical_channel_repository() { reset(); }

  void reset()
  {
    if (parent != nullptr) {
      parent->remove_ue(ue_row_id);
      parent = nullptr;
    }
  }

  bool valid() const { return parent != nullptr; }

  /// Current list of of logical channel configurations for the given UE.
  const logical_channel_config_list& cfg() const { return *get_ue_ctx().channel_configs; }

  /// \brief Update the configurations of the provided lists of bearers.
  void configure(logical_channel_config_list_ptr log_channels_configs);

  /// \brief Deactivate all bearers and drop all pending CEs.
  void deactivate();

  /// Set UE fallback state.
  void set_fallback_state(bool fallback_active);

  /// Setups up an observer for DL pending data for a given RAN slice.
  void register_ran_slice(ran_slice_id_t slice_id);

  /// Assign a RAN slice to a logical channel.
  void set_lcid_ran_slice(lcid_t lcid, ran_slice_id_t slice_id);

  /// Detach logical channel from previously set RAN slice.
  void reset_lcid_ran_slice(lcid_t lcid);

  /// Remove RAN slice and detach all associated logical channels.
  void deregister_ran_slice(ran_slice_id_t slice_id);

  /// Determines whether a RAN slice has at least one bearer associated with it.
  bool has_slice(ran_slice_id_t slice_id) const { return get_ue_ctx().slice_lcid_list_lookup.contains(slice_id); }

  /// Get the RAN slice ID associated with a logical channel.
  std::optional<ran_slice_id_t> get_slice_id(lcid_t lcid) const
  {
    const auto* ch = find_channel_ctx(lcid);
    return ch != nullptr ? find_channel_ctx(lcid)->slice_id : std::nullopt;
  }

  /// \brief Verifies if logical channel is activated for DL.
  bool is_active(lcid_t lcid) const
  {
    const auto* ch_ctx = find_channel_ctx(lcid);
    return ch_ctx != nullptr and ch_ctx->active;
  }

  /// \brief Check whether the UE has pending data, given its current state.
  bool has_pending_bytes() const
  {
    const auto& ue_ctx = get_ue_ctx();
    if (ue_ctx.fallback_state) {
      return ue_ctx.pending_con_res_id or has_pending_bytes(LCID_SRB0) or has_pending_bytes(LCID_SRB1);
    }
    return has_pending_ces() or
           std::any_of(ue_ctx.sorted_channels.begin(), ue_ctx.sorted_channels.end(), [this, &ue_ctx](lcid_t lcid) {
             auto& ch_ctx = parent->configured_channels.row(ue_ctx.channels[lcid]).at<0>();
             return lcid != LCID_SRB0 and ch_ctx.active and ch_ctx.buf_st > 0;
           });
  }

  /// \brief Check whether the UE has pending data in the provided RAN slice.
  bool has_pending_bytes(ran_slice_id_t slice_id) const;

  /// \brief Checks whether a logical channel has pending data.
  bool has_pending_bytes(lcid_t lcid) const { return parent->has_pending_bytes(get_ue_ctx(), lcid); }

  /// \brief Checks whether a ConRes CE is pending for transmission.
  bool is_con_res_id_pending() const { return get_ue_ctx().pending_con_res_id; }

  /// \brief Checks whether UE has pending CEs to be scheduled (ConRes excluded).
  bool has_pending_ces() const
  {
    const auto& ue_ctx = get_ue_ctx();
    return ue_ctx.pending_con_res_id or ue_ctx.pending_ces.has_value();
  }

  /// \brief Calculates total number of DL bytes, including MAC header overhead, and without taking into account
  /// the UE state.
  unsigned total_pending_bytes() const { return parent->total_pending_bytes(get_ue_ctx()); }

  /// \brief Calculates number of DL pending bytes, including MAC header overhead, and taking UE state into account.
  unsigned pending_bytes() const;

  /// Calculates the number of DL pending bytes, including MAC header overhead, for a RAN slice.
  unsigned pending_bytes(ran_slice_id_t slice_id) const;

  /// \brief Returns the UE pending CEs' bytes to be scheduled, if any.
  unsigned pending_ce_bytes() const { return parent->pending_ce_bytes(get_ue_ctx()); }

  /// \brief Checks whether UE has pending UE Contention Resolution Identity CE to be scheduled.
  unsigned pending_con_res_ce_bytes() const
  {
    return dl_logical_channel_system::pending_con_res_ce_bytes(get_ue_ctx());
  }

  /// \brief Last DL buffer status for given LCID (MAC subheader included).
  unsigned pending_bytes(lcid_t lcid) const { return parent->pending_bytes(get_ue_ctx(), lcid); }

  /// \brief Average bit rate, in bps, for a given LCID.
  double average_bit_rate(lcid_t lcid) const
  {
    auto* ch_ctx = find_channel_ctx(lcid);
    auto* qos_ch = (ch_ctx != nullptr and ch_ctx->active and ch_ctx->stats_row.has_value())
                       ? &parent->qos_channels.row(ch_ctx->stats_row.value()).at<0>()
                       : nullptr;
    return qos_ch != nullptr ? qos_ch->avg_bytes_per_slot.average() * 8 * parent->slots_per_sec : 0.0;
  }

  slot_point hol_toa(lcid_t lcid) const
  {
    const channel_context* ch_ctx = find_channel_ctx(lcid);
    return ch_ctx != nullptr and ch_ctx->active ? ch_ctx->hol_toa : slot_point{};
  }

  /// \brief Update DL buffer status for a given LCID.
  void handle_dl_buffer_status_indication(lcid_t lcid, unsigned buffer_status, slot_point hol_toa = {})
  {
    parent->handle_dl_buffer_status_indication(ue_row_id, lcid, buffer_status, hol_toa);
  }

  /// \brief Enqueue new MAC CE to be scheduled.
  /// \return True if the MAC CE was enqueued successfully, false if the queue was full.
  [[nodiscard]] bool handle_mac_ce_indication(const mac_ce_info& ce)
  {
    return parent->handle_mac_ce_indication(ue_row_id, ce);
  }

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
  span<const lcid_t> get_prioritized_logical_channels() const { return get_ue_ctx().sorted_channels; }

private:
  friend class dl_logical_channel_system;

  ue_context&            get_ue_ctx() { return parent->ue_contexts.row(ue_row_id).at<0>(); }
  const ue_context&      get_ue_ctx() const { return parent->ue_contexts.row(ue_row_id).at<0>(); }
  const channel_context* find_channel_ctx(const ue_context& ue_ctx, lcid_t lcid) const
  {
    return ue_ctx.channels.contains(lcid) ? &parent->configured_channels.row(ue_ctx.channels[lcid]).at<0>() : nullptr;
  }
  channel_context* find_channel_ctx(const ue_context& ue_ctx, lcid_t lcid)
  {
    return ue_ctx.channels.contains(lcid) ? &parent->configured_channels.row(ue_ctx.channels[lcid]).at<0>() : nullptr;
  }
  const channel_context* find_channel_ctx(lcid_t lcid) const { return find_channel_ctx(get_ue_ctx(), lcid); }
  channel_context*       find_channel_ctx(lcid_t lcid) { return find_channel_ctx(get_ue_ctx(), lcid); }

  dl_logical_channel_system* parent = nullptr;
  soa::row_id                ue_row_id{std::numeric_limits<uint32_t>::max()};
};

/// \brief Allocate MAC SDUs and corresponding MAC subPDU subheaders.
/// \param[in] tb_info TB on which MAC subPDUs will be stored.
/// \param[in] lch_mng UE DL logical channel manager.
/// \param[in] total_tbs available space in bytes for subPDUs.
/// \param[in] lcid if provided, LCID of the logical channel to be allocated. Otherwise, the LCID with higher priority
/// is chosen.
/// \return Total number of bytes allocated (including MAC subheaders).
unsigned allocate_mac_sdus(dl_msg_tb_info&                   tb_info,
                           ue_dl_logical_channel_repository& lch_mng,
                           unsigned                          total_tbs,
                           lcid_t                            lcid = INVALID_LCID);

/// \brief Allocate MAC subPDUs for pending MAC CEs.
/// \param[in] tb_info TB on which MAC subPDUs will be stored.
/// \param[in] lch_mng UE DL logical channel manager.
/// \param[in] total_tbs available space in bytes for subPDUs.
/// \return Total number of bytes allocated (including MAC subheaders).
/// \remark Excludes UE Contention Resolution Identity CE.
unsigned allocate_mac_ces(dl_msg_tb_info& tb_info, ue_dl_logical_channel_repository& lch_mng, unsigned total_tbs);

/// \brief Allocate MAC subPDUs for pending UE Contention Resolution Identity MAC CE.
/// \param[in] tb_info TB on which MAC subPDUs will be stored.
/// \param[in] lch_mng UE DL logical channel manager.
/// \param[in] total_tbs available space in bytes for subPDUs.
/// \return Total number of bytes allocated (including MAC subheaders).
unsigned
allocate_ue_con_res_id_mac_ce(dl_msg_tb_info& tb_info, ue_dl_logical_channel_repository& lch_mng, unsigned total_tbs);

/// \brief Defines the list of subPDUs, including LCID and payload size, that will compose the transport block for
/// SRB0 or for SRB1 in fallback mode.
/// It includes the UE Contention Resolution Identity CE if it is pending.
/// \return Returns the number of bytes reserved in the TB for subPDUs (other than padding).
unsigned build_dl_fallback_transport_block_info(dl_msg_tb_info&                   tb_info,
                                                ue_dl_logical_channel_repository& lch_mng,
                                                unsigned                          tb_size_bytes);

/// \brief Defines the list of subPDUs, including LCID and payload size, that will compose the transport block for a
/// given RAN slice.
/// \return Returns the number of bytes reserved in the TB for subPDUs (other than padding).
/// \remark Excludes SRB0, as this operation is specific to a given RAN slice.
unsigned build_dl_transport_block_info(dl_msg_tb_info&                   tb_info,
                                       ue_dl_logical_channel_repository& lch_mng,
                                       unsigned                          tb_size_bytes,
                                       ran_slice_id_t                    slice_id);

} // namespace srsran
