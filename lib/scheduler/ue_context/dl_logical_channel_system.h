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

  dl_logical_channel_system();

  /// Signal the start of a new slot.
  void slot_indication();

  /// Creates a new UE logical channel repository.
  ue_dl_logical_channel_repository create_ue(subcarrier_spacing              scs_common,
                                             bool                            starts_in_fallback,
                                             logical_channel_config_list_ptr log_channels_configs);

  /// Number of UEs managed by the system.
  size_t nof_ues() const { return ues.size(); }

  /// Number of logical channels managed by the system.
  size_t nof_logical_channels() const;

private:
  friend class ue_dl_logical_channel_repository;
  static constexpr size_t MAX_RAN_SLICES_PER_UE = 8;

  /// Context of a single pending CE.
  struct mac_ce_context {
    mac_ce_info                info;
    std::optional<soa::row_id> next_ue_ce;
  };
  /// QoS context relative to a single logical channel of a UE, which has QoS enabled.
  struct qos_context {
    /// Bytes-per-slot average for this logical channel.
    moving_averager<unsigned> avg_bytes_per_slot;
    /// Current slot sched bytes.
    unsigned last_sched_bytes = 0;
  };
  /// Context of a single logical channel of a UE.
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

    // Note: clang 18.1.3 std::is_constructible_v<channel_context> return false without this.
    channel_context() noexcept = default;
  };
  /// UE context relative to its configuration.
  struct ue_config_context {
    /// List of UE-dedicated logical channel configurations.
    logical_channel_config_list_ptr channel_configs;
    /// Number of slots per millisecond for this UE, based on the subcarrier spacing.
    unsigned slots_per_msec{0};
  };
  /// UE context relative to its channel management.
  struct ue_channel_context {
    /// Context of channels currently configured. The index of the array matches the LCID.
    slotted_id_table<lcid_t, channel_context, MAX_NOF_RB_LCIDS> channels;
    /// List of active logical channel IDs sorted in decreasing order of priority. i.e. first element has the highest
    /// priority.
    static_vector<lcid_t, MAX_NOF_RB_LCIDS> sorted_channels;
    /// Currently enqueued CEs for this UE.
    std::optional<soa::row_id> pending_ces;
  };
  struct ue_context {
    /// Whether the UE is in fallback (no DRB tx).
    bool fallback_state{false};
    /// Whether a CON RES CE needs to be sent.
    bool pending_con_res_id{false};
    /// Cached sum of pending CE bytes for this UE, excluding CON RES CE.
    uint32_t pending_ce_bytes{0};
    /// Mapping of RAN slice ID to the pending bytes of that slice, excluding any CE.
    static_vector<std::pair<ran_slice_id_t, unsigned>, MAX_RAN_SLICES_PER_UE> pending_bytes_per_slice;

    bool contains(ran_slice_id_t slice_id) const
    {
      return std::any_of(pending_bytes_per_slice.begin(), pending_bytes_per_slice.end(), [slice_id](const auto& item) {
        return item.first == slice_id;
      });
    }
    auto find_slice(ran_slice_id_t slice_id)
    {
      return std::find_if(pending_bytes_per_slice.begin(), pending_bytes_per_slice.end(), [slice_id](auto& item) {
        return item.first == slice_id;
      });
    }
    auto find_slice(ran_slice_id_t slice_id) const
    {
      return std::find_if(pending_bytes_per_slice.begin(), pending_bytes_per_slice.end(), [slice_id](auto& item) {
        return item.first == slice_id;
      });
    }
  };
  enum class ue_column_id { config, context, channel };
  using ue_table     = soa::table<ue_column_id, ue_config_context, ue_context, ue_channel_context>;
  using ue_row       = soa::row_view<ue_table>;
  using const_ue_row = soa::row_view<const ue_table>;

  const_ue_row get_ue(soa::row_id ue_rid) const { return ues.row(ue_rid); }
  ue_row       get_ue(soa::row_id ue_rid) { return ues.row(ue_rid); }

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
  static const channel_context* find_ch_ctx(const const_ue_row& ue_row, lcid_t lcid)
  {
    auto& ue_ch = ue_row.at<ue_channel_context>();
    return ue_ch.channels.contains(lcid) ? &ue_ch.channels[lcid] : nullptr;
  }
  static const channel_context* find_active_ch_ctx(const const_ue_row& ue_row, lcid_t lcid)
  {
    auto* ch_ctx = find_ch_ctx(ue_row, lcid);
    return ch_ctx != nullptr and ch_ctx->active ? ch_ctx : nullptr;
  }
  static bool has_pending_bytes(const const_ue_row& ue_row, lcid_t lcid)
  {
    auto* ch_ctx = find_active_ch_ctx(ue_row, lcid);
    return ch_ctx != nullptr and ch_ctx->buf_st > 0;
  }
  static unsigned pending_bytes(const const_ue_row& ue_row, lcid_t lcid)
  {
    const auto* ch_ctx = find_active_ch_ctx(ue_row, lcid);
    return ch_ctx != nullptr ? get_mac_sdu_required_bytes(ch_ctx->buf_st) : 0;
  }
  unsigned        total_pending_bytes(const const_ue_row& ue_row) const;
  static unsigned pending_con_res_ce_bytes(const const_ue_row& ue_row)
  {
    static const auto ce_size = lcid_dl_sch_t{lcid_dl_sch_t::UE_CON_RES_ID}.sizeof_ce();
    return ue_row.at<ue_context>().pending_con_res_id ? FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + ce_size : 0;
  }
  static unsigned pending_ce_bytes(const const_ue_row& ue_row)
  {
    return pending_con_res_ce_bytes(ue_row) + ue_row.at<ue_context>().pending_ce_bytes;
  }

  /// \brief Returns the next highest priority LCID. The prioritization policy is implementation-defined.
  lcid_t get_max_prio_lcid(const const_ue_row& ue_row) const;

  /// List of MAC CEs pending transmission.
  enum class ce_column_id { ce };
  soa::table<ce_column_id, mac_ce_context> pending_ces;

  /// Logical channels tracking bit-rates for QoS
  enum class qos_column_id { context };
  soa::table<qos_column_id, qos_context> qos_channels;

  /// List of UE contexts.
  ue_table ues;
};

/// Handle of UE DL logical channel repository.
class ue_dl_logical_channel_repository
{
  using mac_ce_info        = dl_logical_channel_system::mac_ce_info;
  using ue_config_context  = dl_logical_channel_system::ue_config_context;
  using ue_context         = dl_logical_channel_system::ue_context;
  using ue_channel_context = dl_logical_channel_system::ue_channel_context;
  using channel_context    = dl_logical_channel_system::channel_context;
  using ue_row             = dl_logical_channel_system::ue_row;
  using const_ue_row       = dl_logical_channel_system::const_ue_row;

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
  const logical_channel_config_list& cfg() const { return *get_ue_row().at<ue_config_context>().channel_configs; }

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
  [[nodiscard]] bool has_slice(ran_slice_id_t slice_id) const
  {
    return get_ue_row().at<ue_context>().contains(slice_id);
  }

  /// Get the RAN slice ID associated with a logical channel.
  std::optional<ran_slice_id_t> get_slice_id(lcid_t lcid) const
  {
    const auto* ch = dl_logical_channel_system::find_ch_ctx(get_ue_row(), lcid);
    return ch != nullptr ? ch->slice_id : std::nullopt;
  }

  /// \brief Verifies if logical channel is activated for DL.
  [[nodiscard]] bool is_active(lcid_t lcid) const
  {
    return dl_logical_channel_system::find_active_ch_ctx(get_ue_row(), lcid) != nullptr;
  }

  /// \brief Check whether the UE has pending data, given its current state.
  [[nodiscard]] bool has_pending_bytes() const
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
    return has_pending_ces() or
           std::any_of(chs_ctx.sorted_channels.begin(), chs_ctx.sorted_channels.end(), [&chs_ctx](lcid_t lcid) {
             return lcid != LCID_SRB0 and chs_ctx.channels[lcid].active and chs_ctx.channels[lcid].buf_st > 0;
           });
  }

  /// \brief Check whether the UE has pending data in the provided RAN slice.
  bool has_pending_bytes(ran_slice_id_t slice_id) const
  {
    const auto& ue_ctx = get_ue_row().at<ue_context>();
    if (ue_ctx.fallback_state) {
      // In fallback mode, slices are disabled.
      return false;
    }
    auto slice_it = ue_ctx.find_slice(slice_id);
    if (slice_it == ue_ctx.pending_bytes_per_slice.end()) {
      return false;
    }
    if (slice_it->second > 0) {
      return true;
    }

    // In case SRB slice was selected (but with no data) and there are pending CE bytes.
    if (slice_id == SRB_RAN_SLICE_ID and has_pending_ces()) {
      // Check if any other slices have pending data. If they do, CE is not considered.
      // Note: This extra check is to avoid multiple slices report pending data.
      return std::all_of(ue_ctx.pending_bytes_per_slice.begin(),
                         ue_ctx.pending_bytes_per_slice.end(),
                         [](const auto& elem) { return elem.second == 0; });
    }

    return false;
  }

  /// \brief Checks whether a logical channel has pending data.
  bool has_pending_bytes(lcid_t lcid) const { return dl_logical_channel_system::has_pending_bytes(get_ue_row(), lcid); }

  /// \brief Checks whether a ConRes CE is pending for transmission.
  bool is_con_res_id_pending() const { return get_ue_row().at<ue_context>().pending_con_res_id; }

  /// \brief Checks whether UE has pending CEs to be scheduled (ConRes excluded).
  bool has_pending_ces() const
  {
    const auto& ue_ctx = get_ue_row().at<ue_context>();
    return ue_ctx.pending_con_res_id or ue_ctx.pending_ce_bytes > 0;
  }

  /// \brief Calculates total number of DL bytes, including MAC header overhead, and without taking into account
  /// the UE state.
  unsigned total_pending_bytes() const { return parent->total_pending_bytes(get_ue_row()); }

  /// \brief Calculates number of DL pending bytes, including MAC header overhead, and taking UE state into account.
  unsigned pending_bytes() const;

  /// Calculates the number of DL pending bytes, including MAC header overhead, for a RAN slice.
  unsigned pending_bytes(ran_slice_id_t slice_id) const
  {
    const auto& ue_ctx = get_ue_row().at<ue_context>();
    if (ue_ctx.fallback_state) {
      return 0;
    }
    auto slice_it = ue_ctx.find_slice(slice_id);
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

  /// \brief Returns the UE pending CEs' bytes to be scheduled, if any.
  unsigned pending_ce_bytes() const { return dl_logical_channel_system::pending_ce_bytes(get_ue_row()); }

  /// \brief Checks whether UE has pending UE Contention Resolution Identity CE to be scheduled.
  unsigned pending_con_res_ce_bytes() const
  {
    return dl_logical_channel_system::pending_con_res_ce_bytes(get_ue_row());
  }

  /// \brief Last DL buffer status for given LCID (MAC subheader included).
  unsigned pending_bytes(lcid_t lcid) const { return dl_logical_channel_system::pending_bytes(get_ue_row(), lcid); }

  /// \brief Average bit rate, in bps, for a given LCID.
  double average_bit_rate(lcid_t lcid) const;

  slot_point hol_toa(lcid_t lcid) const
  {
    const channel_context* ch_ctx = dl_logical_channel_system::find_ch_ctx(get_ue_row(), lcid);
    return ch_ctx != nullptr and ch_ctx->active ? ch_ctx->hol_toa : slot_point{};
  }

  /// \brief Update DL buffer status for a given LCID.
  void handle_dl_buffer_status_indication(lcid_t lcid, unsigned buffer_status, slot_point hol_toa = {});

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
  span<const lcid_t> get_prioritized_logical_channels() const
  {
    return get_ue_row().at<ue_channel_context>().sorted_channels;
  }

private:
  friend class dl_logical_channel_system;

  const_ue_row get_ue_row() const { return parent->get_ue(ue_row_id); }
  ue_row       get_ue_row() { return parent->ues.row(ue_row_id); }

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
