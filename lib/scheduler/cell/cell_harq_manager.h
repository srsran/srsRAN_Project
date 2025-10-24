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

#include "../slicing/ran_slice_id.h"
#include "srsran/adt/intrusive_list.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/logical_channel/lcid_dl_sch.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/scheduler/result/dci_info.h"
#include "srsran/scheduler/result/vrb_alloc.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

struct dl_msg_alloc;
struct pusch_information;

class unique_ue_harq_entity;

/// \brief Notifier of HARQ process timeouts.
class harq_timeout_notifier
{
public:
  virtual ~harq_timeout_notifier() = default;

  /// \brief Notifies a HARQ timeout.
  virtual void on_harq_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) = 0;
};

namespace harq_utils {

/// Possible states of a HARQ process.
enum class harq_state_t { empty, pending_retx, waiting_ack };

struct pending_retx_list_tag;

/// Parameters that are common to DL and UL HARQ processes.
struct base_harq_process : public intrusive_double_linked_list_element<>,
                           public intrusive_double_linked_list_element<pending_retx_list_tag> {
  du_ue_index_t ue_idx;
  rnti_t        rnti;
  harq_id_t     h_id;
  harq_state_t  status = harq_state_t::empty;
  /// Slot at which the PxSCH was transmitted.
  slot_point slot_tx;
  /// Slot at which the respective ACK/CRC is expected to be received in the PHY.
  slot_point slot_ack;
  /// \brief Slot at which the currently set timeout expires. In case of status == waiting_ack, the timeout expires
  /// if no ACK/CRC arrives. In case of status == pending_retx, the timeout expires if no new reTx is scheduled.
  slot_point slot_timeout;
  /// New Data Indicator. Its value should flip for every new Tx.
  bool ndi = false;
  /// Number of retransmissions that took place for the current Transport Block.
  unsigned nof_retxs = 0;
  /// Maximum number of retransmission before Transport Block is reset.
  unsigned max_nof_harq_retxs = 0;
  /// Whether to set the HARQ as ACKed or NACKed when the timeout expires.
  bool ack_on_timeout = false;
  /// Whether retransmissions for this HARQ process have been cancelled.
  bool retxs_cancelled = false;
};

/// Parameters of a DL HARQ process.
struct dl_harq_process_impl : public base_harq_process {
  /// \brief Update to the HARQ process state after a HARQ-ACK is received.
  enum class status_update { acked, nacked, no_update, error };

  /// \brief Parameters relative to the last used PDSCH PDU that get stored in the HARQ process for future reuse.
  struct alloc_params {
    struct lc_alloc_info {
      lcid_dl_sch_t lcid;
      units::bytes  sched_bytes;
    };

    dci_dl_rnti_config_type                     dci_cfg_type;
    vrb_alloc                                   rbs;
    unsigned                                    nof_symbols;
    unsigned                                    nof_layers{1};
    bool                                        is_fallback{false};
    cqi_value                                   cqi;
    pdsch_mcs_table                             mcs_table;
    sch_mcs_index                               mcs;
    unsigned                                    tbs_bytes;
    static_vector<lc_alloc_info, MAX_LC_PER_TB> lc_sched_info;
    /// RAN slice identifier.
    std::optional<ran_slice_id_t> slice_id;
    /// \brief MCS originally suggested by the OLLA. It might differ from the actual MCS used.
    std::optional<sch_mcs_index> olla_mcs;
  };

  /// Parameters used for the last Tx of this HARQ process.
  alloc_params prev_tx_params;
  /// HARQ-bit index corresponding to this HARQ process in the UCI PDU indication.
  uint8_t harq_bit_idx = 0;
  /// Keeps the count of how many PUCCH grants are allocate for this harq_process.
  unsigned pucch_ack_to_receive{0};
  /// Chosen ACK status for this HARQ process transmission, given one or more HARQ-ACK bits received.
  mac_harq_ack_report_status chosen_ack = mac_harq_ack_report_status::dtx;
  /// Stores the highest recorded PUCCH SNR for this HARQ process.
  std::optional<float> last_pucch_snr;
};

/// Parameters of a UL HARQ process.
struct ul_harq_process_impl : public base_harq_process {
  /// \brief Parameters relative to the last allocated PUSCH PDU for this HARQ process.
  struct alloc_params {
    dci_ul_rnti_config_type       dci_cfg_type;
    vrb_alloc                     rbs;
    pusch_mcs_table               mcs_table;
    sch_mcs_index                 mcs;
    unsigned                      tbs_bytes;
    unsigned                      nof_symbols;
    unsigned                      nof_layers;
    std::optional<ran_slice_id_t> slice_id;
    std::optional<sch_mcs_index>  olla_mcs;
  };

  /// Parameters used for the last Tx of this HARQ process.
  alloc_params prev_tx_params;
};

class ntn_dl_harq_alloc_history;
class ntn_ul_harq_alloc_history;

template <bool IsDl>
struct cell_harq_repository {
  using harq_type          = std::conditional_t<IsDl, dl_harq_process_impl, ul_harq_process_impl>;
  using harq_alloc_history = std::conditional_t<IsDl, ntn_dl_harq_alloc_history, ntn_ul_harq_alloc_history>;

  struct ue_harq_entity_impl {
    std::vector<harq_type> harqs;
    std::vector<harq_id_t> free_harq_ids;
    slot_point             last_slot_tx;
    slot_point             last_slot_ack;
  };

  cell_harq_repository(unsigned               max_ues,
                       unsigned               max_ack_wait_in_slots,
                       unsigned               harq_retx_timeout,
                       unsigned               max_harqs_per_ue,
                       unsigned               ntn_cs_koffset,
                       bool                   harq_mode_b,
                       harq_timeout_notifier& timeout_notifier_,
                       srslog::basic_logger&  logger_);
  ~cell_harq_repository();

  /// Time interval, in slots, before the HARQ process assumes that the ACK/CRC went missing.
  const unsigned max_ack_wait_in_slots;
  /// Maximum number of slots before a HARQ with pending retransmission is discarded.
  const unsigned harq_retx_timeout;
  /// Maximum number of HARQs allowed per UE.
  const unsigned         max_harqs_per_ue;
  harq_timeout_notifier& timeout_notifier;
  srslog::basic_logger&  logger;

  slot_point last_sl_ind;

  std::vector<ue_harq_entity_impl>                               ues;
  intrusive_double_linked_list<harq_type, pending_retx_list_tag> harq_pending_retx_list;
  std::vector<intrusive_double_linked_list<harq_type>>           harq_timeout_wheel;
  unsigned                                                       ntn_cs_koffset;
  std::unique_ptr<harq_alloc_history>                            alloc_hist;

  void               slot_indication(slot_point sl_tx);
  void               stop();
  void               handle_harq_ack_timeout(harq_type& h, slot_point sl_tx);
  harq_type*         alloc_harq(du_ue_index_t ue_idx, slot_point sl_tx, slot_point sl_ack, unsigned max_nof_harq_retxs);
  void               dealloc_harq(harq_type& h);
  void               handle_ack(harq_type& h, bool ack);
  void               set_pending_retx(harq_type& h);
  [[nodiscard]] bool handle_new_retx(harq_type& h, slot_point sl_tx, slot_point sl_ack);
  void               reserve_ue_harqs(du_ue_index_t ue_idx, rnti_t rnti, unsigned nof_harqs);
  void               destroy_ue(du_ue_index_t ue_idx);
  void               cancel_retxs(harq_type& h);
  harq_type*         find_ue_harq_in_state(du_ue_index_t ue_idx, harq_utils::harq_state_t state);
  const harq_type*   find_ue_harq_in_state(du_ue_index_t ue_idx, harq_utils::harq_state_t state) const;
  bool               is_ntn_harq_mode_b_enabled() const;
};

template <bool IsDl>
class base_harq_process_handle
{
protected:
  using harq_pool      = cell_harq_repository<IsDl>;
  using harq_impl_type = std::conditional_t<IsDl, dl_harq_process_impl, ul_harq_process_impl>;

public:
  base_harq_process_handle(harq_pool& pool_, harq_impl_type& h_) : harq_repo(&pool_), impl(&h_) {}

  du_ue_index_t ue_index() const { return impl->ue_idx; }
  rnti_t        rnti() const { return impl->rnti; }
  harq_id_t     id() const { return impl->h_id; }
  bool          is_waiting_ack() const { return impl->status == harq_utils::harq_state_t::waiting_ack; }
  bool          has_pending_retx() const { return impl->status == harq_utils::harq_state_t::pending_retx; }
  bool          empty() const { return impl->status == harq_utils::harq_state_t::empty; }
  unsigned      max_nof_retxs() const { return impl->max_nof_harq_retxs; }
  unsigned      nof_retxs() const { return impl->nof_retxs; }
  bool          ndi() const { return impl->ndi; }

  /// \brief Cancels any retransmissions for this HARQ process.
  /// If the HARQ process has a pending retransmission, it is reset. If the ACK/CRC info has not been received yet, the
  /// HARQ process waits for it to arrive before being reset.
  void cancel_retxs();

  /// Empty the HARQ process.
  void reset();

  bool operator==(const base_harq_process_handle& other) const
  {
    return harq_repo == other.harq_repo and impl == other.impl;
  }
  bool operator!=(const base_harq_process_handle& other) const { return !(*this == other); }

protected:
  harq_pool*      harq_repo = nullptr;
  harq_impl_type* impl      = nullptr;
};

} // namespace harq_utils

/// \brief Context of the scheduler during the current PDSCH allocation.
struct dl_harq_alloc_context {
  /// DCI format used to signal the PDSCH allocation.
  dci_dl_rnti_config_type dci_cfg_type;
  /// MCS suggested by the OLLA.
  std::optional<sch_mcs_index> olla_mcs;
  /// RAN slice identifier of the slice to which PDSCH belongs to.
  std::optional<ran_slice_id_t> slice_id;
  /// CQI value at the moment of newTx.
  std::optional<cqi_value> cqi;
  /// Whether the HARQ allocation was done in fallback mode.
  bool is_fallback = false;
};

/// \brief Context of the scheduler during the current PUSCH allocation.
struct ul_harq_alloc_context {
  /// DCI format used to signal the PUSCH allocation.
  dci_ul_rnti_config_type dci_cfg_type;
  /// MCS suggested by the OLLA.
  std::optional<sch_mcs_index> olla_mcs;
  /// RAN slice identifier of the slice to which PUSCH belongs to.
  std::optional<ran_slice_id_t> slice_id;
};

/// \brief Interface used to fetch and update the status of a DL HARQ process.
/// \remark This handle acts like a view to an internal HARQ process. It is not a "unique" type that controls the
/// lifetime of a HARQ. Avoid storing and using the same handle across different slots.
class dl_harq_process_handle : public harq_utils::base_harq_process_handle<true>
{
  using base_type = harq_utils::base_harq_process_handle<true>;

public:
  /// \brief Timeout value to use when the HARQ has been ACKed/NACKed, but it is expecting another PUCCH before being
  /// cleared (implementation-defined).
  static constexpr unsigned SHORT_ACK_TIMEOUT_DTX = 8U;

  using status_update = harq_utils::dl_harq_process_impl::status_update;
  using grant_params  = harq_utils::dl_harq_process_impl::alloc_params;

  using base_type::base_type;

  using base_type::empty;
  using base_type::has_pending_retx;
  using base_type::id;
  using base_type::is_waiting_ack;

  using base_type::max_nof_retxs;
  using base_type::ndi;
  using base_type::nof_retxs;

  using base_type::cancel_retxs;

  [[nodiscard]] bool new_retx(slot_point pdsch_slot, unsigned ack_delay, uint8_t harq_bit_idx);

  /// \brief Update the state of the DL HARQ process waiting for an HARQ-ACK.
  /// \param[in] ack HARQ-ACK status received.
  /// \param[in] pucch_snr SNR of the PUCCH that carried the HARQ-ACK.
  /// \return Status update after processing the ACK info.
  status_update dl_ack_info(mac_harq_ack_report_status ack, std::optional<float> pucch_snr);

  void increment_pucch_counter();

  /// \brief Stores grant parameters that are associated with the HARQ process (e.g. DCI format, PRBs, MCS) so that
  /// they can be later fetched and optionally reused.
  void save_grant_params(const dl_harq_alloc_context& ctx, const dl_msg_alloc& ue_pdsch);

  slot_point pdsch_slot() const { return impl->slot_tx; }
  slot_point uci_slot() const { return impl->slot_ack; }

  const grant_params& get_grant_params() const { return impl->prev_tx_params; }
};

/// Interface used to fetch and update the status of a UL HARQ process.
/// \remark This handle acts like a view to an internal HARQ process. It is not a "unique" type that controls the
/// lifetime of a HARQ. Avoid storing and using the same handle across different slots.
class ul_harq_process_handle : public harq_utils::base_harq_process_handle<false>
{
  using base_type = harq_utils::base_harq_process_handle<false>;

public:
  using grant_params = harq_utils::ul_harq_process_impl::alloc_params;

  using base_type::base_type;

  using base_type::empty;
  using base_type::has_pending_retx;
  using base_type::id;
  using base_type::is_waiting_ack;

  using base_type::max_nof_retxs;
  using base_type::ndi;
  using base_type::nof_retxs;

  using base_type::cancel_retxs;

  [[nodiscard]] bool new_retx(slot_point pusch_slot);

  /// Update UL HARQ state given the received CRC indication.
  /// \return Transport Block size of the HARQ whose state was updated.
  int ul_crc_info(bool ack);

  /// \brief Stores grant parameters that are associated with the HARQ process (e.g. DCI format, PRBs, MCS) so that
  /// they can be later fetched and optionally reused.
  void save_grant_params(const ul_harq_alloc_context& ctx, const pusch_information& pusch);

  slot_point pusch_slot() const { return impl->slot_tx; }

  const grant_params& get_grant_params() const { return impl->prev_tx_params; }
};

namespace harq_utils {

template <bool IsDl>
class harq_pending_retx_list_impl
{
  using harq_pool      = harq_utils::cell_harq_repository<IsDl>;
  using harq_impl_type = std::conditional_t<IsDl, harq_utils::dl_harq_process_impl, harq_utils::ul_harq_process_impl>;
  using handle_type    = std::conditional_t<IsDl, dl_harq_process_handle, ul_harq_process_handle>;
  using harq_impl_it_t = typename intrusive_double_linked_list<harq_impl_type, pending_retx_list_tag>::iterator;

public:
  struct iterator {
    using value_type        = handle_type;
    using reference         = handle_type;
    using pointer           = std::optional<value_type>;
    using iterator_category = std::forward_iterator_tag;

    iterator(harq_pool& pool_, harq_impl_it_t it_) : pool(&pool_), it(it_) {}

    iterator& operator++()
    {
      ++it;
      return *this;
    }
    iterator operator++(int)
    {
      iterator ret{*this};
      ++it;
      return ret;
    }
    reference operator*()
    {
      srsran_assert(it != pool->harq_pending_retx_list.end(), "Dereferencing list end()");
      return handle_type{*pool, *it};
    }
    pointer operator->()
    {
      if (it != pool->harq_pending_retx_list.end()) {
        return handle_type{*pool, *it};
      }
      return std::nullopt;
    }

    bool operator==(const iterator& other) const { return pool == other.pool and it == other.it; }
    bool operator!=(const iterator& other) const { return !(*this == other); }

  private:
    harq_pool*     pool;
    harq_impl_it_t it;
  };

  using value_type      = handle_type;
  using difference_type = std::ptrdiff_t;

  explicit harq_pending_retx_list_impl(harq_pool& pool_) : pool(&pool_) {}

  bool empty() const { return pool->harq_pending_retx_list.empty(); }

  iterator begin() { return iterator{*pool, pool->harq_pending_retx_list.begin()}; }
  iterator end() { return iterator{*pool, pool->harq_pending_retx_list.end()}; }

private:
  harq_pool* pool;
};

} // namespace harq_utils

/// List of HARQ processes with pending retransmissions
using dl_harq_pending_retx_list = harq_utils::harq_pending_retx_list_impl<true>;
using ul_harq_pending_retx_list = harq_utils::harq_pending_retx_list_impl<false>;

/// Manager of all HARQ processes in a given cell.
class cell_harq_manager
{
public:
  /// \brief Default timeout in slots after which the HARQ process assumes that the CRC/ACK went missing
  /// (implementation-defined).
  static constexpr unsigned DEFAULT_ACK_TIMEOUT_SLOTS = 256U;

  /// \brief Default timeout in slots for HARQ to be scheduled for retransmission after a negative CRC/ACK.
  static constexpr unsigned DEFAULT_HARQ_RETX_TIMEOUT_SLOTS = 200U;

  cell_harq_manager(unsigned                               max_ues,
                    unsigned                               max_harqs_per_ue,
                    std::unique_ptr<harq_timeout_notifier> dl_notifier          = nullptr,
                    std::unique_ptr<harq_timeout_notifier> ul_notifier          = nullptr,
                    unsigned                               dl_harq_retx_timeout = DEFAULT_HARQ_RETX_TIMEOUT_SLOTS,
                    unsigned                               ul_harq_retx_timeout = DEFAULT_HARQ_RETX_TIMEOUT_SLOTS,
                    unsigned                               max_ack_wait_timeout = DEFAULT_ACK_TIMEOUT_SLOTS,
                    unsigned                               ntn_cs_koffset       = 0,
                    bool                                   dl_harq_mode_b       = false,
                    bool                                   ul_harq_mode_b       = false);

  /// Update slot, and checks if there are HARQ processes that have reached maxReTx with no ACK
  void slot_indication(slot_point sl_tx);

  /// Called on cell deactivation.
  void stop();

  /// Create new UE HARQ entity.
  /// \param rnti RNTI of the UE
  /// \param nof_dl_harq_procs Number of DL HARQ processes that the UE can support. This value is derived based on
  /// the UE capabilities, and passed to the UE via RRC signalling. See TS38.331, "nrofHARQ-ProcessesForPDSCH".
  /// Values: {2, 4, 6, 10, 12, 16, 32}.
  /// \param nof_ul_harq_procs Number of UL HARQ processes that gNB can support. This value is implementation-defined
  /// and can up to 32 (there are up to 4 bits for HARQ-Id signalling, up to 5 bits in an NTN cell).
  unique_ue_harq_entity add_ue(du_ue_index_t ue_idx,
                               rnti_t        crnti,
                               unsigned      nof_dl_harq_procs = MAX_NOF_HARQS,
                               unsigned      nof_ul_harq_procs = MAX_NOF_HARQS);

  /// Checks whether a UE with the provided ue index exists.
  bool contains(du_ue_index_t ue_idx) const;

  /// Retrieve list of HARQ processes with pending retxs.
  dl_harq_pending_retx_list pending_dl_retxs();
  ul_harq_pending_retx_list pending_ul_retxs();

private:
  friend class unique_ue_harq_entity;

  void destroy_ue(du_ue_index_t ue_idx);

  /// \brief Called on every DL new Tx to allocate an DL HARQ process.
  harq_utils::dl_harq_process_impl* new_dl_tx(du_ue_index_t ue_idx,
                                              rnti_t        rnti,
                                              slot_point    pdsch_slot,
                                              unsigned      ack_delay,
                                              unsigned      max_harq_nof_retxs,
                                              uint8_t       harq_bit_idx);

  /// \brief Called on every UL new Tx to allocate an UL HARQ process.
  harq_utils::ul_harq_process_impl*
  new_ul_tx(du_ue_index_t ue_idx, rnti_t rnti, slot_point pusch_slot, unsigned max_harq_nof_retxs);

  const unsigned                         max_harqs_per_ue;
  std::unique_ptr<harq_timeout_notifier> dl_timeout_notifier;
  std::unique_ptr<harq_timeout_notifier> ul_timeout_notifier;
  srslog::basic_logger&                  logger;

  harq_utils::cell_harq_repository<true>  dl;
  harq_utils::cell_harq_repository<false> ul;
};

/// HARQ entity that manages a set of HARQ processes of a single UE.
class unique_ue_harq_entity
{
  using dl_harq_ent_impl = harq_utils::cell_harq_repository<true>::ue_harq_entity_impl;
  using ul_harq_ent_impl = harq_utils::cell_harq_repository<false>::ue_harq_entity_impl;

public:
  unique_ue_harq_entity() = default;
  unique_ue_harq_entity(cell_harq_manager* mgr, du_ue_index_t ue_idx, rnti_t crnti_);
  ~unique_ue_harq_entity();
  unique_ue_harq_entity(const unique_ue_harq_entity&) = delete;
  unique_ue_harq_entity(unique_ue_harq_entity&& other) noexcept;
  unique_ue_harq_entity& operator=(const unique_ue_harq_entity&) = delete;
  unique_ue_harq_entity& operator=(unique_ue_harq_entity&& other) noexcept;

  bool empty() const { return cell_harq_mgr == nullptr; }

  /// Gets the maximum number of HARQ processes a UE can use, which depends on its configuration.
  unsigned nof_dl_harqs() const { return get_dl_ue().harqs.size(); }
  unsigned nof_ul_harqs() const { return get_ul_ue().harqs.size(); }

  /// Checks whether there are free HARQ processes.
  bool   has_empty_dl_harqs() const { return not get_dl_ue().free_harq_ids.empty(); }
  bool   has_empty_ul_harqs() const { return not get_ul_ue().free_harq_ids.empty(); }
  size_t nof_empty_dl_harqs() const { return get_dl_ue().free_harq_ids.size(); }
  size_t nof_empty_ul_harqs() const { return get_ul_ue().free_harq_ids.size(); }

  /// Check the last HARQ allocations for the given UE.
  slot_point last_pdsch_slot() const { return get_dl_ue().last_slot_tx; }
  slot_point last_ack_slot() const { return get_dl_ue().last_slot_ack; }
  slot_point last_pusch_slot() const { return get_ul_ue().last_slot_tx; }

  /// Deallocate UE HARQ entity.
  void reset();

  /// Block new retxs of pending HARQs.
  void cancel_retxs();

  std::optional<dl_harq_process_handle> dl_harq(harq_id_t h_id)
  {
    if (h_id < get_dl_ue().harqs.size() and get_dl_ue().harqs[h_id].status != harq_utils::harq_state_t::empty) {
      return dl_harq_process_handle{cell_harq_mgr->dl, get_dl_ue().harqs[h_id]};
    }
    return std::nullopt;
  }
  std::optional<const dl_harq_process_handle> dl_harq(harq_id_t h_id) const
  {
    if (h_id < get_dl_ue().harqs.size() and get_dl_ue().harqs[h_id].status != harq_utils::harq_state_t::empty) {
      return dl_harq_process_handle{cell_harq_mgr->dl, cell_harq_mgr->dl.ues[ue_index].harqs[h_id]};
    }
    return std::nullopt;
  }
  std::optional<ul_harq_process_handle>       ul_harq(harq_id_t h_id, slot_point slot);
  std::optional<const ul_harq_process_handle> ul_harq(harq_id_t h_id, slot_point slot) const;
  std::optional<ul_harq_process_handle>       ul_harq(harq_id_t h_id)
  {
    if (h_id < get_ul_ue().harqs.size() and get_ul_ue().harqs[h_id].status != harq_utils::harq_state_t::empty) {
      return ul_harq_process_handle{cell_harq_mgr->ul, get_ul_ue().harqs[h_id]};
    }
    return std::nullopt;
  }
  std::optional<const ul_harq_process_handle> ul_harq(harq_id_t h_id) const
  {
    if (h_id < get_ul_ue().harqs.size() and get_ul_ue().harqs[h_id].status != harq_utils::harq_state_t::empty) {
      return ul_harq_process_handle{cell_harq_mgr->ul, cell_harq_mgr->ul.ues[ue_index].harqs[h_id]};
    }
    return std::nullopt;
  }

  std::optional<dl_harq_process_handle>
  alloc_dl_harq(slot_point sl_tx, unsigned ack_delay, unsigned max_harq_nof_retxs, unsigned harq_bit_idx);
  std::optional<ul_harq_process_handle> alloc_ul_harq(slot_point sl_tx, unsigned max_harq_nof_retxs);

  std::optional<dl_harq_process_handle>       find_pending_dl_retx();
  std::optional<const dl_harq_process_handle> find_pending_dl_retx() const;
  std::optional<ul_harq_process_handle>       find_pending_ul_retx();
  std::optional<const ul_harq_process_handle> find_pending_ul_retx() const;

  std::optional<dl_harq_process_handle>       find_dl_harq_waiting_ack();
  std::optional<const dl_harq_process_handle> find_dl_harq_waiting_ack() const;
  std::optional<ul_harq_process_handle>       find_ul_harq_waiting_ack();

  /// Fetch a DL HARQ process expecting ACK info based on HARQ-ACK UCI slot and HARQ bit index.
  /// \param[in] uci_slot Slot when the UCI is to be received.
  /// \param[in] harq_bit_idx Bit index of the HARQ-ACK in the UCI indication.
  /// \return Active DL HARQ process with matching UCI slot and HARQ bit index, if found.
  std::optional<dl_harq_process_handle> find_dl_harq_waiting_ack(slot_point uci_slot, uint8_t harq_bit_idx);

  /// Fetch active UL HARQ process based on slot when its PUSCH was transmitted.
  /// \param[in] pusch_slot Slot when the PUSCH was transmitted.
  /// \return Active UL HARQ process with matching PUSCH slot, if found.
  std::optional<ul_harq_process_handle> find_ul_harq_waiting_ack(slot_point pusch_slot);

  /// \brief The UCI scheduling associated with a given slot was cancelled. The associated DL HARQs will be NACKed, and
  /// won't expect further UCIs.
  ///
  /// This function can be called for instance when there is an error indication coming from lower layers.
  void uci_sched_failed(slot_point uci_slot);

  /// Determines the sum of the number of bytes that are in active UL HARQ processes.
  unsigned total_ul_bytes_waiting_ack() const;

private:
  dl_harq_ent_impl&       get_dl_ue() { return cell_harq_mgr->dl.ues[ue_index]; }
  const dl_harq_ent_impl& get_dl_ue() const { return cell_harq_mgr->dl.ues[ue_index]; }
  ul_harq_ent_impl&       get_ul_ue() { return cell_harq_mgr->ul.ues[ue_index]; }
  const ul_harq_ent_impl& get_ul_ue() const { return cell_harq_mgr->ul.ues[ue_index]; }

  cell_harq_manager* cell_harq_mgr = nullptr;
  du_ue_index_t      ue_index      = INVALID_DU_UE_INDEX;
  rnti_t             crnti         = rnti_t::INVALID_RNTI;
};

} // namespace srsran
