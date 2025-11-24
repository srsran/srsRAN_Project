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
#include "srsran/adt/soa_table.h"
#include "srsran/adt/static_flat_map.h"
#include "srsran/mac/mac_pdu_format.h"
#include "srsran/ran/logical_channel/lcid_dl_sch.h"
#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/support/math/moving_averager.h"

namespace srsran {

class ue_logical_channel_repository;
struct ul_bsr_indication_message;

/// System responsible for handling the logical channel configuration and buffer occupancy status of UEs.
class logical_channel_system
{
public:
  /// Holds the information relative to a MAC CE that needs to be scheduled.
  struct mac_ce_info {
    /// LCID of the MAC CE.
    lcid_dl_sch_t ce_lcid;
    /// Holds payload of CE except UE Contention Resolution Identity.
    std::variant<ta_cmd_ce_payload, dummy_ce_payload> ce_payload = dummy_ce_payload{0};
  };

  logical_channel_system();

  /// Signal the start of a new slot.
  void slot_indication();

  /// Creates a new UE logical channel repository.
  /// \param[in] ue_index DU UE index.
  /// \param[in] slot_ind_scs Subcarrier spacing associated with slot indications.
  /// \param[in] starts_in_fallback Whether the UE starts in fallback mode (no DRB tx).
  /// \param[in] log_channels_configs List of logical channel configurations for this UE.
  /// \return Handle to the created UE logical channel repository.
  ue_logical_channel_repository create_ue(du_ue_index_t                   ue_index,
                                          subcarrier_spacing              slot_ind_scs,
                                          bool                            starts_in_fallback,
                                          logical_channel_config_list_ptr log_channels_configs);

  /// Number of UEs managed by the system.
  size_t nof_ues() const { return ues.size(); }

  /// Number of logical channels managed by the system.
  size_t nof_logical_channels() const;

  /// Fills list with UEs that have DL pending newTx data for the given RAN slice.
  /// \param[in] slice_id RAN slice identifier.
  /// \return Bitset of UEs with pending data for the provided RANslice.
  bounded_bitset<MAX_NOF_DU_UES> get_ues_with_dl_pending_data(ran_slice_id_t slice_id) const;

  /// Fills list with UEs that have UL pending newTx data for the given RAN slice (SR not considered).
  /// \param[in] slice_id RAN slice identifier.
  /// \return Bitset of UEs with pending data for the provided RANslice.
  bounded_bitset<MAX_NOF_DU_UES> get_ues_with_ul_pending_data(ran_slice_id_t slice_id) const;

private:
  friend class ue_logical_channel_repository;
  static constexpr size_t MAX_RAN_SLICES_PER_UE = 8;

  /// Information relative to a slice of the scheduler.
  struct ran_slice_context {
    /// Bitset of UE indexes for UEs with pending newTx DL data for this RAN slice.
    bounded_bitset<MAX_NOF_DU_UES> pending_dl_ues;
    /// Bitset of UE indexes for UEs with pending newTx UL data for this RAN slice.
    bounded_bitset<MAX_NOF_DU_UES> pending_ul_ues;
  };

  /// Context of a single pending CE.
  struct mac_ce_context {
    mac_ce_info                info;
    std::optional<soa::row_id> next_ue_ce;
  };
  /// QoS context relative to a single logical channel of a UE, which has QoS enabled.
  struct qos_context {
    /// DL Bytes-per-slot average for this logical channel.
    moving_averager<unsigned> dl_avg_bytes_per_slot;
    /// Current slot DL sched bytes.
    unsigned dl_last_sched_bytes = 0;
  };
  /// QoS context relative to a single logical channel group of a UE, which has QoS enabled.
  struct lcg_qos_context {
    /// Last slot UL sched bytes.
    unsigned ul_last_sched_bytes = 0;
    /// Sched bytes since last BSR.
    unsigned sched_bytes_accum = 0;
    /// UL Bytes-per-slot average for this logical channel.
    moving_averager<unsigned> ul_avg_bytes_per_slot;
  };
  /// Context of a single logical channel of a UE.
  struct dl_logical_channel_context {
    /// Whether the configured logical channel is currently active.
    bool active = false;
    /// DL Buffer status of this logical channel.
    unsigned buf_st = 0;
    /// Head-of-line (HOL) time-of-arrival
    slot_point hol_toa;
    /// In case QoS statistics are being tracked, holds the row in the \c qos_channels table.
    std::optional<soa::row_id> qos_row;
    /// Slice associated with this channel.
    std::optional<ran_slice_id_t> slice_id;

    // Note: clang 18.1.3 std::is_constructible_v<channel_context> return false without this.
    dl_logical_channel_context() noexcept = default;
  };
  /// Context of a single UL logical channel group of a UE.
  struct ul_logical_channel_group_context {
    /// Whether the configured logical channel is currently active.
    bool active = false;
    /// Number of LCIDs associated with this LCG.
    uint8_t lc_count = 0;
    /// UL Buffer status of this logical channel.
    unsigned buf_st = 0;
    /// In case QoS statistics are being tracked, holds the row in the \c qos_channels table.
    std::optional<soa::row_id> qos_row;
    /// Slice associated with this channel.
    std::optional<ran_slice_id_t> slice_id;
  };
  /// UE context relative to its configuration.
  struct ue_config_context {
    /// DU UE index of this UE.
    du_ue_index_t ue_index{INVALID_DU_UE_INDEX};
    /// Number of slots per millisecond for this UE, based on the subcarrier spacing.
    unsigned slots_per_msec{0};
    /// List of UE-dedicated logical channel configurations.
    logical_channel_config_list_ptr channel_configs;
  };
  /// UE context relative to its channel management.
  struct ue_channel_context {
    /// Currently enqueued CEs for this UE.
    std::optional<soa::row_id> pending_ces;
    /// List of active logical channel IDs sorted in decreasing order of priority. i.e. first element has the highest
    /// priority.
    static_vector<lcid_t, MAX_NOF_RB_LCIDS> sorted_channels;
    /// Context of channels currently configured. The index of the array matches the LCID.
    static_flat_map<lcid_t, dl_logical_channel_context, MAX_NOF_RB_LCIDS> channels;
  };
  /// UE context relative to its UL channel management.
  struct ue_ul_channel_context {
    /// Context of logical channel groups (LCGs) currently configured.
    static_flat_map<lcg_id_t, ul_logical_channel_group_context, MAX_NOF_LCGS> lcgs;
  };
  /// UE context relative to its DL state.
  struct ue_context {
    /// Whether the UE is in fallback (no DRB tx).
    bool fallback_state{false};
    /// Whether a CON RES CE needs to be sent.
    bool pending_con_res_id{false};
    /// Cached sum of pending CE bytes for this UE, excluding CON RES CE.
    uint32_t pending_ce_bytes{0};
    /// Mapping of RAN slice ID to the pending bytes of that slice, excluding any CE.
    static_flat_map<ran_slice_id_t, unsigned, MAX_RAN_SLICES_PER_UE> pending_bytes_per_slice;
  };
  // UE context relative to its UL state.
  struct ue_ul_context {
    /// Mapping of RAN slice ID to the pending bytes of that slice, excluding SRs.
    static_flat_map<ran_slice_id_t, unsigned, MAX_RAN_SLICES_PER_UE> pending_bytes_per_slice;
  };
  enum class ue_column_id { config, context, ul_context, channel };
  using ue_table =
      soa::table<ue_column_id, ue_config_context, ue_context, ue_ul_context, ue_channel_context, ue_ul_channel_context>;
  using ue_row       = soa::row_view<ue_table>;
  using const_ue_row = soa::row_view<const ue_table>;

  const_ue_row get_ue(soa::row_id ue_rid) const { return ues.row(ue_rid); }
  ue_row       get_ue(soa::row_id ue_rid) { return ues.row(ue_rid); }

  void remove_ue(soa::row_id ue_rid);
  void configure(soa::row_id ue_rid, logical_channel_config_list_ptr log_channels_configs);
  void deactivate(soa::row_id ue_rid);

  void set_lcid_ran_slice(soa::row_id ue_rid, lcid_t lcid, ran_slice_id_t slice_id);
  void deregister_lc_ran_slice(soa::row_id ue_rid, lcid_t lcid);
  void set_lcg_ran_slice(soa::row_id, lcg_id_t lcgid, ran_slice_id_t slice_id);
  void deregister_lcg_ran_slice(soa::row_id ue_rid, lcg_id_t lcgid);

  void
  handle_dl_buffer_status_indication(soa::row_id ue_row_id, lcid_t lcid, unsigned buffer_status, slot_point hol_toa);
  bool handle_mac_ce_indication(soa::row_id ue_row_id, const mac_ce_info& ce);
  void handle_bsr_indication(soa::row_id ue_row_id, const ul_bsr_indication_message& bsr);

  unsigned allocate_mac_sdu(soa::row_id ue_rid, dl_msg_lc_info& subpdu, lcid_t lcid, unsigned rem_bytes);
  unsigned allocate_mac_sdu(soa::row_id ue_rid, dl_msg_lc_info& lch_info, unsigned rem_bytes, lcid_t lcid);
  unsigned allocate_mac_ce(soa::row_id ue_rid, dl_msg_lc_info& lch_info, unsigned rem_bytes);
  unsigned allocate_ue_con_res_id_mac_ce(soa::row_id ue_rid, dl_msg_lc_info& lch_info, unsigned rem_bytes);

  /// Adds an estimate of the upper layer required header bytes.
  static unsigned add_upper_layer_header_bytes(lcg_id_t lcgid, unsigned payload_bytes)
  {
    // Estimate of the number of bytes required for the upper layer header.
    static constexpr unsigned RLC_HEADER_SIZE_ESTIMATE = 3U;
    if (payload_bytes == 0 or lcgid == 0) {
      // In case of no payload or LCG-ID == 0, there is no need to account for upper layer header.
      // TODO: Is this a fair assumption for LCG-ID == 0?
      return payload_bytes;
    }
    return payload_bytes + RLC_HEADER_SIZE_ESTIMATE;
  }

  // Helper inlined methods
  static const dl_logical_channel_context* find_ch_ctx(const const_ue_row& ue_row, lcid_t lcid)
  {
    auto& ue_ch = ue_row.at<ue_channel_context>();
    return ue_ch.channels.contains(lcid) ? &ue_ch.channels.at(lcid) : nullptr;
  }
  static const dl_logical_channel_context* find_active_ch_ctx(const const_ue_row& ue_row, lcid_t lcid)
  {
    auto* ch_ctx = find_ch_ctx(ue_row, lcid);
    return ch_ctx != nullptr and ch_ctx->active ? ch_ctx : nullptr;
  }
  static const ul_logical_channel_group_context* find_active_lcg_ctx(const const_ue_row& ue_row, lcg_id_t lcgid)
  {
    auto* ch_ctx = find_ch_ctx(ue_row, lcgid);
    return ch_ctx != nullptr and ch_ctx->active ? ch_ctx : nullptr;
  }
  static const ul_logical_channel_group_context* find_ch_ctx(const const_ue_row& ue_row, lcg_id_t lcgid)
  {
    auto& ue_ch = ue_row.at<ue_ul_channel_context>();
    return ue_ch.lcgs.contains(lcgid) ? &ue_ch.lcgs.at(lcgid) : nullptr;
  }
  static bool has_pending_bytes(const const_ue_row& ue_row, lcid_t lcid)
  {
    auto* ch_ctx = find_active_ch_ctx(ue_row, lcid);
    return ch_ctx != nullptr and ch_ctx->buf_st > 0;
  }
  static bool has_pending_bytes(const const_ue_row& ue_row, lcg_id_t lcgid)
  {
    auto* ch_ctx = find_active_lcg_ctx(ue_row, lcgid);
    return ch_ctx != nullptr and ch_ctx->buf_st > 0;
  }
  static unsigned pending_bytes(const const_ue_row& ue_row, lcid_t lcid)
  {
    // Note: DL buffer occupancy report already accounts for at least one RLC header.
    const auto* ch_ctx = find_active_ch_ctx(ue_row, lcid);
    return ch_ctx != nullptr ? get_mac_sdu_required_bytes(ch_ctx->buf_st) : 0;
  }
  static unsigned pending_bytes(const const_ue_row& ue_row, lcg_id_t lcgid)
  {
    // Note: TS38.321, 6.1.3.1 - The size of the RLC and MAC headers are not considered in the buffer size computation.
    const auto* ch_ctx = find_active_lcg_ctx(ue_row, lcgid);
    return ch_ctx != nullptr ? get_mac_sdu_required_bytes(add_upper_layer_header_bytes(lcgid, ch_ctx->buf_st)) : 0;
  }
  unsigned        total_dl_pending_bytes(const const_ue_row& ue_row) const;
  static unsigned pending_con_res_ce_bytes(const const_ue_row& ue_row)
  {
    static const auto ce_size = lcid_dl_sch_t{lcid_dl_sch_t::UE_CON_RES_ID}.sizeof_ce();
    return ue_row.at<ue_context>().pending_con_res_id ? FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + ce_size : 0;
  }
  static unsigned pending_ce_bytes(const const_ue_row& ue_row)
  {
    return pending_con_res_ce_bytes(ue_row) + ue_row.at<ue_context>().pending_ce_bytes;
  }

  /// Helper to remove an LCG from a UE.
  void remove_lcg(soa::row_id                       ue_rid,
                  ue_ul_channel_context&            ue_ch,
                  lcg_id_t                          lcgid,
                  ul_logical_channel_group_context& ue_lcg);

  /// Helper method to update the pending bytes for a given RAN slice when a lc associated with a slice is updated.
  void on_single_channel_buf_st_update(ue_row&                              u,
                                       bool                                 channel_active,
                                       const std::optional<ran_slice_id_t>& slice_id,
                                       unsigned                             new_buf_st,
                                       unsigned                             prev_buf_st);
  void on_single_lcg_buf_st_update(ue_row&                              u,
                                   bool                                 channel_active,
                                   const std::optional<ran_slice_id_t>& slice_id,
                                   unsigned                             new_buf_st,
                                   unsigned                             prev_buf_st);

  /// \brief Returns the next highest priority LCID. The prioritization policy is implementation-defined.
  lcid_t get_max_prio_lcid(const const_ue_row& ue_row) const;

  /// Bitset of configured UEs.
  bounded_bitset<MAX_NOF_DU_UES> configured_ues;

  /// Bitset of UEs with pending CEs.
  bounded_bitset<MAX_NOF_DU_UES> ues_with_pending_ces;

  /// Bitset of UEs with pending SRs.
  bounded_bitset<MAX_NOF_DU_UES> ues_with_pending_sr;

  /// List of bitsets of UEs with pending newTx DL data per RAN slice.
  flat_map<ran_slice_id_t, ran_slice_context> slices;

  /// List of MAC CEs pending transmission.
  enum class ce_column_id { ce };
  soa::table<ce_column_id, mac_ce_context> pending_ces;

  /// Logical channels tracking bit-rates for QoS
  enum class qos_column_id { context };
  soa::table<qos_column_id, qos_context>     qos_channels;
  soa::table<qos_column_id, lcg_qos_context> qos_lcgs;

  /// List of UE contexts.
  ue_table ues;
};

/// Handle of UE logical channel repository.
class ue_logical_channel_repository
{
  using mac_ce_info           = logical_channel_system::mac_ce_info;
  using ue_config_context     = logical_channel_system::ue_config_context;
  using ue_context            = logical_channel_system::ue_context;
  using ue_ul_context         = logical_channel_system::ue_ul_context;
  using ue_channel_context    = logical_channel_system::ue_channel_context;
  using ue_ul_channel_context = logical_channel_system::ue_ul_channel_context;
  using channel_context       = logical_channel_system::dl_logical_channel_context;
  using ue_row                = logical_channel_system::ue_row;
  using const_ue_row          = logical_channel_system::const_ue_row;

  ue_logical_channel_repository(logical_channel_system& parent_, du_ue_index_t ue_index_, soa::row_id ue_row_) :
    parent(&parent_), ue_index(ue_index_), ue_row_id(ue_row_)
  {
  }

public:
  ue_logical_channel_repository()                                     = default;
  ue_logical_channel_repository(const ue_logical_channel_repository&) = delete;
  ue_logical_channel_repository(ue_logical_channel_repository&& other) noexcept :
    parent(std::exchange(other.parent, nullptr)),
    ue_index(std::exchange(other.ue_index, INVALID_DU_UE_INDEX)),
    ue_row_id(other.ue_row_id)
  {
  }
  ue_logical_channel_repository& operator=(const ue_logical_channel_repository&) = delete;
  ue_logical_channel_repository& operator=(ue_logical_channel_repository&& other) noexcept
  {
    parent    = std::exchange(other.parent, nullptr);
    ue_index  = other.ue_index;
    ue_row_id = other.ue_row_id;
    return *this;
  }
  ~ue_logical_channel_repository() { reset(); }

  void reset()
  {
    if (parent != nullptr) {
      parent->remove_ue(ue_row_id);
      parent = nullptr;
    }
  }

  bool valid() const { return parent != nullptr; }

  /// Current list of of logical channel configurations for the given UE.
  const logical_channel_config_list& cfg() const { return *get_ue_row().at<ue_config_context>().channel_configs; }

  /// \brief Update the configurations of the provided lists of bearers.
  void configure(logical_channel_config_list_ptr log_channels_configs);

  /// \brief Deactivate all bearers and drop all pending CEs.
  void deactivate();

  /// Set UE fallback state.
  void set_fallback_state(bool fallback_active);

  /// Assign a RAN slice to a logical channel.
  void set_lcid_ran_slice(lcid_t lcid, ran_slice_id_t slice_id);

  /// Assign a RAN slice to a logical channel group.
  void set_lcg_ran_slice(lcg_id_t lcgid, ran_slice_id_t slice_id);

  /// Detach logical channel from previously set RAN slice.
  void reset_lcid_ran_slice(lcid_t lcid);

  /// Detach logical channel group from previously set RAN slice.
  void reset_lcg_ran_slice(lcg_id_t lcgid);

  /// Determines whether a RAN slice has at least one bearer associated with it.
  [[nodiscard]] bool has_slice(ran_slice_id_t slice_id) const
  {
    return get_ue_row().at<ue_context>().pending_bytes_per_slice.contains(slice_id);
  }

  /// Get the RAN slice ID associated with a logical channel.
  std::optional<ran_slice_id_t> get_slice_id(lcid_t lcid) const
  {
    const auto* ch = logical_channel_system::find_ch_ctx(get_ue_row(), lcid);
    return ch != nullptr ? ch->slice_id : std::nullopt;
  }

  /// Get the RAN slice ID associated with a logical channel group.
  std::optional<ran_slice_id_t> get_slice_id(lcg_id_t lcgid) const
  {
    const auto* ch = logical_channel_system::find_ch_ctx(get_ue_row(), lcgid);
    return ch != nullptr ? ch->slice_id : std::nullopt;
  }

  /// \brief Verifies if logical channel is activated for DL.
  [[nodiscard]] bool is_active(lcid_t lcid) const
  {
    return logical_channel_system::find_active_ch_ctx(get_ue_row(), lcid) != nullptr;
  }

  /// \brief Verifies if logical channel group is activated for UL.
  [[nodiscard]] bool is_active(lcg_id_t lcgid) const
  {
    return logical_channel_system::find_active_lcg_ctx(get_ue_row(), lcgid) != nullptr;
  }

  /// Check whether the UE is in fallback state.
  [[nodiscard]] bool is_in_fallback_state() const { return get_ue_row().at<ue_context>().fallback_state; }

  /// \brief Check whether the UE has pending DL data, given its current state.
  [[nodiscard]] bool has_dl_pending_bytes() const
  {
    auto        u      = get_ue_row();
    const auto& ue_ctx = u.at<ue_context>();
    if (ue_ctx.fallback_state) {
      return ue_ctx.pending_con_res_id or has_pending_bytes(LCID_SRB0) or has_pending_bytes(LCID_SRB1);
    }
    if (has_pending_ces()) {
      return true;
    }
    const auto& chs_ctx = u.at<ue_channel_context>();
    return std::any_of(chs_ctx.sorted_channels.begin(), chs_ctx.sorted_channels.end(), [&chs_ctx](lcid_t lcid) {
      return lcid != LCID_SRB0 and chs_ctx.channels.at(lcid).active and chs_ctx.channels.at(lcid).buf_st > 0;
    });
  }

  /// \brief Check whether the UE has pending UL data (excluding SR), given its current state.
  [[nodiscard]] bool has_ul_pending_bytes() const
  {
    auto        u      = get_ue_row();
    const auto& ue_ctx = u.at<ue_context>();
    if (ue_ctx.fallback_state) {
      return has_pending_bytes(uint_to_lcg_id(0U));
    }
    const auto& chs_ctx = u.at<ue_ul_channel_context>();
    return std::any_of(chs_ctx.lcgs.begin(), chs_ctx.lcgs.end(), [](const auto& p) {
      return p.second.active and p.second.buf_st > 0;
    });
  }

  /// \brief Check whether the UE has pending data in the provided RAN slice.
  /// \return Returns true if the UE is active, in non-fallback mode, it has pending bytes for the provided RAN slice
  /// ID and the slice is configured. Returns 0, otherwise.
  [[nodiscard]] bool has_pending_dl_bytes(ran_slice_id_t slice_id) const
  {
    auto slice_bitset_it = parent->slices.find(slice_id);
    if (slice_bitset_it != parent->slices.end() and slice_bitset_it->second.pending_dl_ues.test(ue_index)) {
      return true;
    }

    // In case SRB slice was selected (but with no data) and there are pending CE bytes (excluding ConRes, which is
    // only scheduled in fallback mode).
    if (slice_id == SRB_RAN_SLICE_ID and parent->ues_with_pending_ces.test(ue_index)) {
      // Check if any other slices have pending data. If they do, CE is not considered.
      // Note: This extra check is to avoid multiple slices report pending data when CEs are pending.
      return std::all_of(parent->slices.begin(), parent->slices.end(), [this, slice_id](const auto& p) {
        return p.first == slice_id or not p.second.pending_dl_ues.test(ue_index);
      });
    }
    return false;
  }

  /// \brief Check whether the UE has pending UL data in the provided RAN slice.
  /// \return Returns true if the UE is active, in non-fallback mode, it has pending bytes for the provided RAN slice
  /// ID and the slice is configured. Returns 0, otherwise.
  [[nodiscard]] bool has_pending_ul_bytes(ran_slice_id_t slice_id) const
  {
    auto slice_bitset_it = parent->slices.find(slice_id);
    return slice_bitset_it != parent->slices.end() and slice_bitset_it->second.pending_ul_ues.test(ue_index);
  }

  /// \brief Checks whether a logical channel has pending data.
  bool has_pending_bytes(lcid_t lcid) const { return logical_channel_system::has_pending_bytes(get_ue_row(), lcid); }

  /// \brief Checks whether a logical channel has pending data.
  bool has_pending_bytes(lcg_id_t lcgid) const
  {
    return logical_channel_system::has_pending_bytes(get_ue_row(), lcgid);
  }

  /// \brief Checks whether a ConRes CE is pending for transmission.
  bool is_con_res_id_pending() const { return get_ue_row().at<ue_context>().pending_con_res_id; }

  /// \brief Checks whether UE has pending CEs to be scheduled (ConRes included).
  bool has_pending_ces() const
  {
    return parent->ues_with_pending_ces.test(ue_index) or get_ue_row().at<ue_context>().pending_con_res_id;
  }

  /// \brief Checks whether a SR indication handling is pending.
  [[nodiscard]] bool has_pending_sr() const { return parent->ues_with_pending_sr.test(ue_index); }

  /// \brief Calculates total number of DL bytes, including MAC header overhead, and without taking into account
  /// the UE state.
  unsigned total_dl_pending_bytes() const { return parent->total_dl_pending_bytes(get_ue_row()); }

  /// \brief Calculates number of DL pending bytes, including MAC header overhead, and taking UE state into account.
  unsigned dl_pending_bytes() const;

  /// \brief Calculates number of UL pending bytes, including MAC header overhead, and taking UE state into account.
  unsigned ul_pending_bytes() const;

  /// Calculates the number of DL pending bytes, including MAC header overhead, for a RAN slice.
  unsigned dl_pending_bytes(ran_slice_id_t slice_id) const
  {
    const auto& ue_ctx = get_ue_row().at<ue_context>();
    if (ue_ctx.fallback_state) {
      return 0;
    }
    auto slice_it = ue_ctx.pending_bytes_per_slice.find(slice_id);
    if (slice_it == ue_ctx.pending_bytes_per_slice.end()) {
      return 0;
    }

    unsigned       total_bytes = slice_it->second;
    const unsigned ce_bytes    = ue_ctx.pending_ce_bytes;
    if (ce_bytes > 0) {
      // There are also pending CE bytes.
      if (total_bytes > 0) {
        // In case the UE has pending bearer bytes, we also include the CE bytes.
        total_bytes += ce_bytes;
      } else if (slice_id == SRB_RAN_SLICE_ID) {
        // In case SRB1 was selected, and there are no pending bytes in the selected bearers, we return the pending CE
        // bytes iff the UE has no pending data on the remaining, non-selected bearers.
        // This is to avoid the situation where a UE, for instance, has DRB data to transmit, but the CE is allocated in
        // the SRB slice instead.
        if (std::all_of(ue_ctx.pending_bytes_per_slice.begin(),
                        ue_ctx.pending_bytes_per_slice.end(),
                        [](const auto& elem) { return elem.second == 0; })) {
          return ce_bytes;
        }
        return 0;
      }
    }

    return total_bytes;
  }

  /// Calculates the number of UL pending bytes for a RAN slice.
  unsigned ul_pending_bytes(ran_slice_id_t slice_id) const
  {
    if (get_ue_row().at<ue_context>().fallback_state) {
      return 0;
    }
    const auto& ue_ul_ctx = get_ue_row().at<ue_ul_context>();
    auto        slice_it  = ue_ul_ctx.pending_bytes_per_slice.find(slice_id);
    if (slice_it == ue_ul_ctx.pending_bytes_per_slice.end()) {
      return 0;
    }
    return slice_it->second;
  }

  /// \brief Returns the UE pending CEs' bytes to be scheduled, if any.
  unsigned pending_ce_bytes() const { return logical_channel_system::pending_ce_bytes(get_ue_row()); }

  /// \brief Checks whether UE has pending UE Contention Resolution Identity CE to be scheduled.
  unsigned pending_con_res_ce_bytes() const { return logical_channel_system::pending_con_res_ce_bytes(get_ue_row()); }

  /// \brief Last DL buffer status for given LCID (MAC subheader included).
  unsigned pending_bytes(lcid_t lcid) const { return logical_channel_system::pending_bytes(get_ue_row(), lcid); }

  /// \brief Last UL buffer status for given LCG-ID (MAC subheader included).
  unsigned pending_bytes(lcg_id_t lcgid) const { return logical_channel_system::pending_bytes(get_ue_row(), lcgid); }

  /// \brief Average DL bit rate, in bps, for a given LCID.
  double average_dl_bit_rate(lcid_t lcid) const;

  /// \brief Average UL bit rate, in bps, for a given LCG-ID.
  double average_ul_bit_rate(lcg_id_t lcgid) const;

  slot_point hol_toa(lcid_t lcid) const
  {
    const channel_context* ch_ctx = logical_channel_system::find_ch_ctx(get_ue_row(), lcid);
    return ch_ctx != nullptr and ch_ctx->active ? ch_ctx->hol_toa : slot_point{};
  }

  /// \brief Update DL buffer status for a given LCID.
  void handle_dl_buffer_status_indication(lcid_t lcid, unsigned buffer_status, slot_point hol_toa = {});

  /// \brief Enqueue new MAC CE to be scheduled.
  /// \return True if the MAC CE was enqueued successfully, false if the queue was full.
  [[nodiscard]] bool handle_mac_ce_indication(const mac_ce_info& ce);

  /// \brief Handle a new Buffer Status Report.
  void handle_bsr_indication(const ul_bsr_indication_message& msg);

  /// \brief Indicate that the UE requested an UL grant.
  void handle_sr_indication();

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

  /// \brief Register the scheduling of an UL grant for this UE.
  ///
  /// This event will be used to update estimated bit rates.
  void handle_ul_grant(unsigned grant_size);

  /// Clear SR indication pending flag.
  void reset_sr_indication();

  /// \brief Returns a list of LCIDs sorted based on decreasing order of priority.
  span<const lcid_t> get_prioritized_logical_channels() const
  {
    return get_ue_row().at<ue_channel_context>().sorted_channels;
  }

private:
  friend class logical_channel_system;

  const_ue_row get_ue_row() const { return parent->get_ue(ue_row_id); }
  ue_row       get_ue_row() { return parent->ues.row(ue_row_id); }

  logical_channel_system* parent   = nullptr;
  du_ue_index_t           ue_index = INVALID_DU_UE_INDEX;
  soa::row_id             ue_row_id{std::numeric_limits<uint32_t>::max()};
};

/// \brief Allocate MAC SDUs and corresponding MAC subPDU subheaders.
/// \param[in] tb_info TB on which MAC subPDUs will be stored.
/// \param[in] lch_mng UE DL logical channel manager.
/// \param[in] total_tbs available space in bytes for subPDUs.
/// \param[in] lcid if provided, LCID of the logical channel to be allocated. Otherwise, the LCID with higher priority
/// is chosen.
/// \return Total number of bytes allocated (including MAC subheaders).
unsigned allocate_mac_sdus(dl_msg_tb_info&                tb_info,
                           ue_logical_channel_repository& lch_mng,
                           unsigned                       total_tbs,
                           lcid_t                         lcid = INVALID_LCID);

/// \brief Allocate MAC subPDUs for pending MAC CEs.
/// \param[in] tb_info TB on which MAC subPDUs will be stored.
/// \param[in] lch_mng UE DL logical channel manager.
/// \param[in] total_tbs available space in bytes for subPDUs.
/// \return Total number of bytes allocated (including MAC subheaders).
/// \remark Excludes UE Contention Resolution Identity CE.
unsigned allocate_mac_ces(dl_msg_tb_info& tb_info, ue_logical_channel_repository& lch_mng, unsigned total_tbs);

/// \brief Allocate MAC subPDUs for pending UE Contention Resolution Identity MAC CE.
/// \param[in] tb_info TB on which MAC subPDUs will be stored.
/// \param[in] lch_mng UE DL logical channel manager.
/// \param[in] total_tbs available space in bytes for subPDUs.
/// \return Total number of bytes allocated (including MAC subheaders).
unsigned
allocate_ue_con_res_id_mac_ce(dl_msg_tb_info& tb_info, ue_logical_channel_repository& lch_mng, unsigned total_tbs);

/// \brief Defines the list of subPDUs, including LCID and payload size, that will compose the transport block for
/// SRB0 or for SRB1 in fallback mode.
/// It includes the UE Contention Resolution Identity CE if it is pending.
/// \return Returns the number of bytes reserved in the TB for subPDUs (other than padding).
unsigned build_dl_fallback_transport_block_info(dl_msg_tb_info&                tb_info,
                                                ue_logical_channel_repository& lch_mng,
                                                unsigned                       tb_size_bytes);

/// \brief Defines the list of subPDUs, including LCID and payload size, that will compose the transport block for a
/// given RAN slice.
/// \return Returns the number of bytes reserved in the TB for subPDUs (other than padding).
/// \remark Excludes SRB0, as this operation is specific to a given RAN slice.
unsigned build_dl_transport_block_info(dl_msg_tb_info&                tb_info,
                                       ue_logical_channel_repository& lch_mng,
                                       unsigned                       tb_size_bytes,
                                       ran_slice_id_t                 slice_id);

} // namespace srsran
