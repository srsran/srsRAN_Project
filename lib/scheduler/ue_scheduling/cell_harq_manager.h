/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/intrusive_list.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/scheduler/scheduler_dci.h"
#include "srsran/srslog/srslog.h"
#include <queue>

namespace srsran {

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

const static unsigned INVALID_HARQ_REF_INDEX = std::numeric_limits<unsigned>::max();

/// Possible states of a HARQ process.
enum class harq_state_t { empty, pending_retx, waiting_ack };

/// Parameters that are common to DL and UL HARQ processes.
struct base_harq_process : public intrusive_double_linked_list_element<> {
  du_ue_index_t ue_idx;
  harq_id_t     h_id;
  harq_state_t  status = harq_state_t::empty;
  slot_point    slot_tx;
  slot_point    slot_ack;
  slot_point    slot_ack_timeout;
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
    dci_dl_rnti_config_type dci_cfg_type;
    vrb_alloc               rbs;
    unsigned                nof_symbols;
    unsigned                nof_layers{1};
    bool                    is_fallback{false};
    cqi_value               cqi;
    pdsch_mcs_table         mcs_table;
    sch_mcs_index           mcs;
    unsigned                tbs_bytes;
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
    dci_ul_rnti_config_type      dci_cfg_type;
    vrb_alloc                    rbs;
    pusch_mcs_table              mcs_table;
    sch_mcs_index                mcs;
    unsigned                     tbs_bytes;
    unsigned                     nof_symbols;
    std::optional<sch_mcs_index> olla_mcs;
  };

  /// Parameters used for the last Tx of this HARQ process.
  alloc_params prev_tx_params;
};

struct ue_harq_entity_impl {
  std::vector<unsigned>  harqs;
  std::vector<harq_id_t> free_harq_ids;
};

template <bool IsDl>
struct cell_harq_repository {
  using harq_type = std::conditional_t<IsDl, dl_harq_process_impl, ul_harq_process_impl>;

  cell_harq_repository(unsigned               max_ues,
                       unsigned               max_ack_wait_in_slots,
                       harq_timeout_notifier& timeout_notifier_,
                       srslog::basic_logger&  logger_);

  /// Maximum value of time interval, in slots, before the HARQ process assumes that the ACK/CRC went missing.
  const unsigned         max_ack_wait_in_slots;
  harq_timeout_notifier& timeout_notifier;
  srslog::basic_logger&  logger;

  std::vector<harq_type>                               harqs;
  std::vector<unsigned>                                free_harqs;
  std::vector<ue_harq_entity_impl>                     ues;
  intrusive_double_linked_list<harq_type>              harq_pending_retx_list;
  std::vector<intrusive_double_linked_list<harq_type>> harq_timeout_wheel;

  unsigned get_harq_ref_idx(const harq_type& h) const;

  void       slot_indication(slot_point sl_tx);
  void       handle_harq_ack_timeout(harq_type& h, slot_point sl_tx);
  harq_type* alloc_harq(du_ue_index_t ue_idx, slot_point sl_tx, slot_point sl_ack, unsigned max_nof_harq_retxs);
  void       dealloc_harq(harq_type& h);
  void       handle_ack(harq_type& h, bool ack);
  void       set_pending_retx(harq_type& h);
  void       handle_new_retx(harq_type& h, slot_point sl_tx, slot_point sl_ack);
  void       reserve_ue_harqs(du_ue_index_t ue_idx, unsigned nof_harqs);
  void       destroy_ue_harqs(du_ue_index_t ue_idx);
  void       cancel_retxs(harq_type& h);
  unsigned   find_ue_harq_in_state(du_ue_index_t ue_idx, harq_utils::harq_state_t state) const;
};

} // namespace harq_utils

class cell_harq_manager
{
public:
  /// \brief Default timeout in slots after which the HARQ process assumes that the CRC/ACK went missing
  /// (implementation-defined).
  constexpr static unsigned DEFAULT_ACK_TIMEOUT_SLOTS = 256U;

  cell_harq_manager(unsigned                               max_ues              = MAX_NOF_DU_UES,
                    std::unique_ptr<harq_timeout_notifier> notifier             = nullptr,
                    unsigned                               max_ack_wait_timeout = DEFAULT_ACK_TIMEOUT_SLOTS);

  /// Update slot, and checks if there are HARQ processes that have reached maxReTx with no ACK
  void slot_indication(slot_point sl_tx);

  /// Create new UE HARQ entity.
  /// \param rnti RNTI of the UE
  /// \param nof_dl_harq_procs Number of DL HARQ processes that the UE can support. This value is derived based on
  /// the UE capabilities, and passed to the UE via RRC signalling. See TS38.331, "nrofHARQ-ProcessesForPDSCH".
  /// Values: {2, 4, 6, 10, 12, 16}.
  /// \param nof_ul_harq_procs Number of UL HARQ processes that gNB can support. This value is implementation-defined
  /// and can up to 16 (there are up to 4 bits for HARQ-Id signalling).
  unique_ue_harq_entity add_ue(du_ue_index_t ue_idx,
                               rnti_t        crnti,
                               unsigned      nof_dl_harq_procs = MAX_NOF_HARQS,
                               unsigned      nof_ul_harq_procs = MAX_NOF_HARQS);

  /// Checks whether an UE with the provided ue index exists.
  bool contains(du_ue_index_t ue_idx) const;

private:
  friend class unique_ue_harq_entity;
  friend class dl_harq_process_view;
  friend class ul_harq_process_view;

  /// \brief Timeout value to use when the HARQ has been ACKed/NACKed, but it is expecting another PUCCH before being
  /// cleared (implementation-defined).
  constexpr static unsigned SHORT_ACK_TIMEOUT_DTX = 8U;

  const static unsigned INVALID_HARQ = std::numeric_limits<unsigned>::max();

  void destroy_ue(du_ue_index_t ue_idx);

  /// \brief Called on every DL new Tx to allocate an DL HARQ process.
  harq_utils::dl_harq_process_impl* new_dl_tx(du_ue_index_t ue_idx,
                                              slot_point    pdsch_slot,
                                              unsigned      k1,
                                              unsigned      max_harq_nof_retxs,
                                              uint8_t       harq_bit_idx);

  /// \brief Called on every UL new Tx to allocate an UL HARQ process.
  harq_utils::ul_harq_process_impl* new_ul_tx(du_ue_index_t ue_idx, slot_point pusch_slot, unsigned max_harq_nof_retxs);

  /// \brief Called on a new retx of a DL HARQ process.
  void new_dl_retx(harq_utils::dl_harq_process_impl& h, slot_point pdsch_slot, unsigned k1, uint8_t harq_bit_idx);

  /// \brief Called on a new retx of a UL HARQ process.
  void new_ul_retx(harq_utils::ul_harq_process_impl& h, slot_point pusch_slot);

  /// Updates a DL HARQ process given the received HARQ-ACK info.
  harq_utils::dl_harq_process_impl::status_update
  dl_ack_info(harq_utils::dl_harq_process_impl& h, mac_harq_ack_report_status ack, std::optional<float> pucch_snr);

  /// Updates a UL HARQ process given the received CRC indication.
  /// \return Transport Block size of the HARQ whose state was updated.
  int ul_crc_info(harq_utils::ul_harq_process_impl& h, bool ack);

  std::unique_ptr<harq_timeout_notifier> timeout_notifier;
  srslog::basic_logger&                  logger;

  slot_point last_sl_tx;

  harq_utils::cell_harq_repository<true>  dl;
  harq_utils::cell_harq_repository<false> ul;
};

class dl_harq_process_view
{
public:
  using status_update = harq_utils::dl_harq_process_impl::status_update;
  using grant_params  = harq_utils::dl_harq_process_impl::alloc_params;

  dl_harq_process_view(cell_harq_manager& cell_harq_mng_, unsigned h_ref_idx) :
    cell_harq_mng(&cell_harq_mng_), harq_ref_idx(h_ref_idx)
  {
    srsran_sanity_check(cell_harq_mng->dl.harqs[harq_ref_idx].status != harq_utils::harq_state_t::empty,
                        "Empty HARQ process created");
  }

  harq_id_t id() const { return cell_harq_mng->dl.harqs[harq_ref_idx].h_id; }

  bool is_waiting_ack() const
  {
    return cell_harq_mng->dl.harqs[harq_ref_idx].status == harq_utils::harq_state_t::waiting_ack;
  }
  bool has_pending_retx() const
  {
    return cell_harq_mng->dl.harqs[harq_ref_idx].status == harq_utils::harq_state_t::pending_retx;
  }

  void new_retx(slot_point pdsch_slot, unsigned k1, uint8_t harq_bit_idx);

  /// \brief Update the state of the DL HARQ process waiting for an HARQ-ACK.
  /// \param[in] ack HARQ-ACK status received.
  /// \param[in] pucch_snr SNR of the PUCCH that carried the HARQ-ACK.
  /// \return Status update after processing the ACK info.
  status_update dl_ack_info(mac_harq_ack_report_status ack, std::optional<float> pucch_snr);

  const grant_params& get_grant_params() const { return cell_harq_mng->dl.harqs[harq_ref_idx].prev_tx_params; }

  bool operator==(const dl_harq_process_view& other) const
  {
    return cell_harq_mng == other.cell_harq_mng and harq_ref_idx == other.harq_ref_idx;
  }
  bool operator!=(const dl_harq_process_view& other) const { return !(*this == other); }

private:
  cell_harq_manager* cell_harq_mng = nullptr;
  unsigned           harq_ref_idx  = cell_harq_manager::INVALID_HARQ;
};

class ul_harq_process_view
{
public:
  ul_harq_process_view(cell_harq_manager& cell_harq_mng_, unsigned h_ref_idx) :
    cell_harq_mng(&cell_harq_mng_), harq_ref_idx(h_ref_idx)
  {
    srsran_sanity_check(cell_harq_mng->ul.harqs[harq_ref_idx].status != harq_utils::harq_state_t::empty,
                        "Empty HARQ process created");
  }

  harq_id_t id() const { return cell_harq_mng->ul.harqs[harq_ref_idx].h_id; }

  bool is_waiting_ack() const
  {
    return cell_harq_mng->ul.harqs[harq_ref_idx].status == harq_utils::harq_state_t::waiting_ack;
  }
  bool has_pending_retx() const
  {
    return cell_harq_mng->ul.harqs[harq_ref_idx].status == harq_utils::harq_state_t::pending_retx;
  }

  void new_retx(slot_point pusch_slot);

  /// Update UL HARQ state given the received CRC indication.
  /// \return Transport Block size of the HARQ whose state was updated.
  int ul_crc_info(bool ack);

  bool operator==(const ul_harq_process_view& other) const
  {
    return cell_harq_mng == other.cell_harq_mng and harq_ref_idx == other.harq_ref_idx;
  }
  bool operator!=(const ul_harq_process_view& other) const { return !(*this == other); }

private:
  cell_harq_manager* cell_harq_mng = nullptr;
  unsigned           harq_ref_idx  = cell_harq_manager::INVALID_HARQ;
};

class unique_ue_harq_entity
{
public:
  unique_ue_harq_entity() = default;
  unique_ue_harq_entity(cell_harq_manager* mgr, du_ue_index_t ue_idx, rnti_t crnti_) :
    cell_harq_mgr(mgr), ue_index(ue_idx), crnti(crnti_)
  {
    (void)crnti;
  }
  ~unique_ue_harq_entity();
  unique_ue_harq_entity(const unique_ue_harq_entity&) = delete;
  unique_ue_harq_entity(unique_ue_harq_entity&& other) noexcept;
  unique_ue_harq_entity& operator=(const unique_ue_harq_entity&) = delete;
  unique_ue_harq_entity& operator=(unique_ue_harq_entity&& other) noexcept;

  /// Gets the maximum number of HARQ processes a UE can use, which depends on its configuration.
  unsigned nof_dl_harqs() const { return get_dl_ue().harqs.size(); }
  unsigned nof_ul_harqs() const { return get_ul_ue().harqs.size(); }

  /// Checks whether there are free HARQ processes.
  bool has_empty_dl_harqs() const { return not get_dl_ue().free_harq_ids.empty(); }
  bool has_empty_ul_harqs() const { return not get_ul_ue().free_harq_ids.empty(); }

  /// Deallocate UE HARQ entity.
  void reset();

  std::optional<dl_harq_process_view> dl_harq(harq_id_t h_id)
  {
    if (cell_harq_mgr->dl.ues[ue_index].harqs[h_id] != harq_utils::INVALID_HARQ_REF_INDEX) {
      return dl_harq_process_view{*cell_harq_mgr, cell_harq_mgr->dl.ues[ue_index].harqs[h_id]};
    }
    return std::nullopt;
  }
  std::optional<ul_harq_process_view> ul_harq(harq_id_t h_id)
  {
    if (cell_harq_mgr->ul.ues[ue_index].harqs[h_id] != harq_utils::INVALID_HARQ_REF_INDEX) {
      return ul_harq_process_view{*cell_harq_mgr, cell_harq_mgr->ul.ues[ue_index].harqs[h_id]};
    }
    return std::nullopt;
  }

  std::optional<dl_harq_process_view>
  alloc_dl_harq(slot_point sl_tx, unsigned k1, unsigned max_harq_nof_retxs, unsigned harq_bit_idx);
  std::optional<ul_harq_process_view> alloc_ul_harq(slot_point sl_tx, unsigned max_harq_nof_retxs);

  std::optional<dl_harq_process_view> find_pending_dl_retx();
  std::optional<ul_harq_process_view> find_pending_ul_retx();

  std::optional<dl_harq_process_view> find_dl_harq_waiting_ack();
  std::optional<ul_harq_process_view> find_ul_harq_waiting_ack();

  /// Fetch active DL HARQ process based on HARQ-ACK UCI slot and HARQ bit index.
  /// \param[in] uci_slot Slot when the UCI is to be received.
  /// \param[in] harq_bit_idx Bit index of the HARQ-ACK in the UCI indication.
  /// \return Active DL HARQ process with matching UCI slot and HARQ bit index, if found.
  std::optional<dl_harq_process_view> find_dl_harq(slot_point uci_slot, uint8_t harq_bit_idx);

  /// Fetch active UL HARQ process based on slot when its PUSCH was transmitted.
  /// \param[in] pusch_slot Slot when the PUSCH was transmitted.
  /// \return Active UL HARQ process with matching PUSCH slot, if found.
  std::optional<ul_harq_process_view> find_ul_harq(slot_point pusch_slot);

private:
  harq_utils::ue_harq_entity_impl&       get_dl_ue() { return cell_harq_mgr->dl.ues[ue_index]; }
  const harq_utils::ue_harq_entity_impl& get_dl_ue() const { return cell_harq_mgr->dl.ues[ue_index]; }
  harq_utils::ue_harq_entity_impl&       get_ul_ue() { return cell_harq_mgr->ul.ues[ue_index]; }
  const harq_utils::ue_harq_entity_impl& get_ul_ue() const { return cell_harq_mgr->ul.ues[ue_index]; }

  cell_harq_manager* cell_harq_mgr = nullptr;
  du_ue_index_t      ue_index      = INVALID_DU_UE_INDEX;
  rnti_t             crnti         = rnti_t::INVALID_RNTI;
};

} // namespace srsran