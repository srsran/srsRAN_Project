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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/sch/sch_mcs.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/scheduler/sched_consts.h"
#include "srsran/scheduler/scheduler_dci.h"
#include "srsran/support/format_utils.h"
#include <numeric>

namespace srsran {

struct pdsch_information;
struct pusch_information;

/// \brief Handler of HARQ process timeouts.
class harq_timeout_handler
{
public:
  virtual ~harq_timeout_handler() = default;

  /// \brief Handles a HARQ process timeout.
  virtual void handle_harq_timeout(du_ue_index_t ue_index, bool is_dl) = 0;
};

/// \brief UE-specific notifier of HARQ process timeouts.
class ue_harq_timeout_notifier
{
public:
  /// \brief Default constructor creates a no-op notifier.
  ue_harq_timeout_notifier();

  /// \brief Creates a notifier to a custom harq timeout handler.
  ///
  /// \param notifier_ The custom harq timeout handler.
  /// \param ue_index_ The UE index.
  ue_harq_timeout_notifier(harq_timeout_handler& handler_, du_ue_index_t ue_index_);

  /// \brief Notifies a HARQ timeout.
  void notify_harq_timeout(bool is_dl) { handler->handle_harq_timeout(ue_index, is_dl); }

private:
  harq_timeout_handler* handler;
  du_ue_index_t         ue_index;
};

/// \brief Helper class to log HARQ events.
class harq_logger
{
public:
  harq_logger(srslog::basic_logger& logger_, rnti_t rnti_, du_cell_index_t cell_index_, bool is_dl_) :
    logger(logger_), rnti(rnti_), cell_index(cell_index_), is_dl(is_dl_)
  {
  }

  void set_rnti(rnti_t rnti_) { rnti = rnti_; }

  template <typename... Args>
  void warning(harq_id_t h_id, const char* fmtstr, Args&&... args)
  {
    log(logger.warning, h_id, fmtstr, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void info(harq_id_t h_id, const char* fmtstr, Args&&... args)
  {
    log(logger.info, h_id, fmtstr, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void debug(harq_id_t h_id, const char* fmtstr, Args&&... args)
  {
    log(logger.debug, h_id, fmtstr, std::forward<Args>(args)...);
  }

private:
  template <typename... Args>
  void log(srslog::log_channel& ch, harq_id_t h_id, const char* fmtstr, Args&&... args)
  {
    if (not ch.enabled()) {
      return;
    }
    fmt::memory_buffer fmtbuf;
    fmt::format_to(fmtbuf, fmtstr, std::forward<Args>(args)...);
    ch("{} HARQ rnti={} cell={} h_id={}: {}", is_dl ? "DL" : "UL", rnti, cell_index, h_id, to_c_str(fmtbuf));
  }

  srslog::basic_logger& logger;
  rnti_t                rnti;
  du_cell_index_t       cell_index;
  bool                  is_dl;
};

namespace detail {

/// Basic class for HARQ processes: will be extended by DL and UL HARQ process classes
template <bool IsDownlink>
class harq_process
{
  /// Type used to store the DCI format and RNTI type of the TB stored in the HARQ.
  using dci_rnti_config_type = std::conditional_t<IsDownlink, dci_dl_rnti_config_type, dci_ul_rnti_config_type>;

public:
  /// \brief Default timeout in slots after which the HARQ process assumes that the CRC/ACK went missing
  /// (implementation-defined).
  constexpr static unsigned DEFAULT_ACK_TIMEOUT_SLOTS = 256U;

  /// \brief Timeout value to use when the HARQ has been ACKed/NACKed, but it is expecting another PUCCH before being
  /// cleared (implementation-defined).
  constexpr static unsigned SHORT_ACK_TIMEOUT_DTX = 8U;

  /// Maximum number of Transport Blocks as per TS38.321, 5.3.2.1 and 5.4.2.1.
  constexpr static size_t MAX_NOF_TBS = IsDownlink ? 2 : 1;

  struct transport_block {
    /// State of the Transport Block.
    enum class state_t { empty, pending_retx, waiting_ack } state = state_t::empty;

    /// New Data Indicator. Its value should flip for every new Tx.
    bool ndi = false;

    /// Number of retransmissions that took place for the current Transport Block.
    unsigned nof_retxs = 0;

    /// Maximum number of retransmission before Transport Block is reset.
    unsigned max_nof_harq_retxs = 0;

    /// HARQ-bit index corresponding to this HARQ process in the UCI PDU indication.
    uint8_t harq_bit_idx = 0;

    /// Whether to set the HARQ as ACKed or NACKed when the timeout expires.
    bool ack_on_timeout = false;

    bool empty() const { return state == state_t::empty; }
  };

  /// HARQ process id.
  const harq_id_t id;

  /// \brief HARQ process constructor.
  /// \param h_id HARQ process ID.
  /// \param max_ack_wait_in_slots_ number of slots above which the scheduler considers that the ACK/CRC went missing.
  explicit harq_process(harq_id_t                h_id,
                        harq_logger&             logger_,
                        ue_harq_timeout_notifier timeout_notif          = {},
                        unsigned                 max_ack_wait_in_slots_ = DEFAULT_ACK_TIMEOUT_SLOTS);

  /// \brief Indicate the beginning of a new slot.
  void slot_indication(slot_point slot_tx);

  /// \brief Last slot when a SCH grant was scheduled for this HARQ process.
  slot_point slot_tx() const { return last_slot_tx; }

  /// \brief Slot when the last SCH grant of this HARQ process is supposed to be ACKed.
  slot_point slot_ack() const { return last_slot_ack; }

  /// \brief Checks whether a specified TB is empty.
  bool empty(unsigned tb_idx) const
  {
    srsran_assert(tb_idx < tb_array.size(), "Invalid TB index={}", tb_idx);
    return tb_array[tb_idx].empty();
  }

  /// \brief Checks whether specified TB has pending new_retx.
  bool has_pending_retx(unsigned tb_idx) const
  {
    srsran_assert(tb_idx < tb_array.size(), "Invalid TB index ({} >= {})", tb_idx, tb_array.size());
    return tb_array[tb_idx].state == transport_block::state_t::pending_retx;
  }

  /// \brief Checks whether specified TB is expecting an ACK.
  bool is_waiting_ack(unsigned tb_idx) const
  {
    srsran_assert(tb_idx < tb_array.size(), "Invalid TB index={}", tb_idx);
    return tb_array[tb_idx].state == transport_block::state_t::waiting_ack;
  }

  unsigned max_nof_harq_retxs(unsigned tb_idx) const
  {
    srsran_assert(tb_idx < tb_array.size(), "Invalid TB index ({} >= {})", tb_idx, tb_array.size());
    return tb_array[tb_idx].max_nof_harq_retxs;
  }

  /// \brief Getter for TB parameters.
  const transport_block& tb(unsigned tb_idx) const { return tb_array[tb_idx]; }

  /// \brief Resets HARQ process state.
  void reset();

  /// \brief Cancels the HARQ process by stopping retransmissions of the currently held TB.
  void cancel_harq_retxs(unsigned tb_idx);

  /// \brief Getter of the slot when the HARQ process will assume that the ACK/CRC went missing.
  slot_point get_slot_ack_timeout() const { return slot_ack_timeout; }

protected:
  void tx_common(slot_point slot_tx, slot_point slot_ack);
  void new_tx_tb_common(unsigned tb_idx, unsigned max_nof_harq_retxs, uint8_t harq_bit_idx);
  void new_retx_tb_common(unsigned tb_idx, uint8_t harq_bit_idx);

  /// \brief Updates the ACK state of the TB of the HARQ process.
  bool ack_info_common(uint32_t tb_idx, bool ack);

  /// \brief Clear the contents of a specific TB of this HARQ process and resets it back to empty state.
  void reset_tb(unsigned tb_idx);

  /// HARQ entity logger used by this HARQ process.
  harq_logger& logger;

  /// Notifier used by this HARQ process when there is a timeout.
  ue_harq_timeout_notifier timeout_notifier;

  /// Maximum value of time interval, in slots, before the HARQ process assumes that the ACK/CRC went missing.
  const unsigned max_ack_wait_in_slots;

  /// Last slot indication.
  slot_point last_slot_ind;

  /// For DL, slot_tx corresponds to the slot when the TB in the HARQ process is going to be transmitted by the gNB.
  /// For UL, slot_tx corresponds to the slot when the TB in the HARQ process is going to be transmitted by the UE.
  slot_point last_slot_tx;

  /// For DL, slot_ack is the slot at which gNB is expected to receive the ACK via UCI.
  /// For UL, slot_ack is the slot when the PUSCH CRC will be received by the gNB. It coincides with slot_tx of UL.
  slot_point last_slot_ack;

  /// Slot when the HARQ process will assume that the ACK/CRC went missing.
  slot_point slot_ack_timeout;

  std::array<transport_block, MAX_NOF_TBS> tb_array;
};

} // namespace detail

class dl_harq_process : public detail::harq_process<true>
{
  using base_type = detail::harq_process<true>;

public:
  /// \brief Update to the HARQ process state after a HARQ-ACK is received.
  enum class status_update { acked, nacked, no_update, error };

  /// \brief Parameters relative to the last used PDSCH PDU that get stored in the HARQ process for future reuse.
  struct alloc_params {
    struct tb_params {
      pdsch_mcs_table mcs_table;
      sch_mcs_index   mcs;
      unsigned        tbs_bytes;
    };

    dci_dl_rnti_config_type                                 dci_cfg_type;
    vrb_alloc                                               rbs;
    unsigned                                                nof_symbols;
    std::array<optional<tb_params>, base_type::MAX_NOF_TBS> tb;
    cqi_value                                               cqi;
    unsigned                                                nof_layers;
    bool                                                    is_fallback{false};
  };

  struct dl_ack_info_result {
    /// \brief HARQ process ID.
    harq_id_t h_id;
    /// \brief mcs_table used for the last PDSCH transmission.
    pdsch_mcs_table mcs_table;
    /// \brief MCS used for the last PDSCH transmission.
    sch_mcs_index mcs;
    /// \brief Number of bytes of the last PDSCH transmission.
    unsigned tbs_bytes;
    /// \brief HARQ process status update.
    dl_harq_process::status_update update;
  };

  using base_type::transport_block;

  using base_type::empty;
  using base_type::harq_process;
  using base_type::has_pending_retx;
  using base_type::is_waiting_ack;

  /// \brief Checks whether the HARQ process has no TBs currently active.
  bool empty() const { return base_type::empty(0) and base_type::empty(1); }

  /// \brief Checks whether there is at least one TB with pending new_retx.
  bool has_pending_retx() const { return base_type::has_pending_retx(0) or base_type::has_pending_retx(1); }

  /// \brief Checks whether the UL HARQ process is busy but waiting for an ACK.
  bool is_waiting_ack() const { return base_type::is_waiting_ack(0) or base_type::is_waiting_ack(1); }

  /// \brief Getter of the PDCCH/PDSCH parameters used in the last Tx.
  const alloc_params& last_alloc_params() const { return prev_tx_params; }

  /// \brief Called on every new TB transmission, when only one TB is active. It marks this HARQ process as busy and
  /// stores respective TB information.
  void new_tx(slot_point pdsch_slot,
              unsigned   k1,
              unsigned   max_harq_nof_retxs,
              uint8_t    harq_bit_idx,
              cqi_value  cqi,
              unsigned   nof_layers,
              bool       is_fallback = false);

  /// \brief Called on every TB retransmission, when only one TB is active. This function assumes that the HARQ TB is
  /// in pending new_retx state.
  void new_retx(slot_point pdsch_slot, unsigned k1, uint8_t harq_bit_idx);

  /// \brief Called on every new TB transmission/retransmission, when 2 TBs are used.
  enum class tb_tx_request { newtx, retx, disabled };
  void tx_2_tb(slot_point                pdsch_slot,
               unsigned                  k1,
               span<const tb_tx_request> tb_tx_req,
               unsigned                  max_harq_nof_retxs,
               uint8_t                   harq_bit_idx);

  /// \brief Updates the ACK state of the HARQ process.
  /// \return The result of a HARQ receiving an HARQ-ACK bit.
  status_update ack_info(uint32_t tb_idx, mac_harq_ack_report_status ack, optional<float> pucch_snr);

  /// \brief Stores grant parameters that are associated with the HARQ allocation (e.g. DCI format, PRBs, MCS) so that
  /// they can be later fetched and optionally reused.
  void save_alloc_params(dci_dl_rnti_config_type dci_cfg_type, const pdsch_information& pdsch);

  void increment_pucch_counter();

private:
  /// Parameters used for the last Tx of this HARQ process.
  alloc_params prev_tx_params;
  /// Keeps the count of how many PUCCH grants are allocate for this harq_process.
  unsigned pucch_ack_to_receive{0};
  /// Chosen ACK status for this HARQ process transmission, given one or more HARQ-ACK bits received.
  mac_harq_ack_report_status chosen_ack = mac_harq_ack_report_status::dtx;
  /// Stores the highest recorded PUCCH SNR for this HARQ process.
  optional<float> last_pucch_snr;
};

class ul_harq_process : private detail::harq_process<false>
{
  using base_type = detail::harq_process<false>;

public:
  /// \brief Parameters relative to the last allocated PUSCH PDU for this HARQ process.
  struct alloc_params {
    dci_ul_rnti_config_type dci_cfg_type;
    vrb_alloc               rbs;
    pusch_mcs_table         mcs_table;
    sch_mcs_index           mcs;
    unsigned                tbs_bytes;
    unsigned                nof_symbols;
  };

  using base_type::transport_block;

  using base_type::get_slot_ack_timeout;
  using base_type::harq_process;
  using base_type::id;
  using base_type::reset;
  using base_type::slot_ack;
  using base_type::slot_indication;
  using base_type::slot_tx;

  /// \brief Checks whether the HARQ process is inactive.
  bool empty() const { return base_type::empty(0); }

  /// \brief Checks whether the UL HARQ process has a pending new_retx.
  bool has_pending_retx() const { return base_type::has_pending_retx(0); }

  /// \brief Checks whether the UL HARQ process is busy but waiting for an ACK.
  bool is_waiting_ack() const { return base_type::is_waiting_ack(0); }

  /// \brief Gets the maximum number of HARQ retxs.
  unsigned max_nof_harq_retxs() const { return base_type::max_nof_harq_retxs(0); }

  /// \brief Getter of TB params.
  const transport_block& tb() const { return base_type::tb(0); }

  /// \brief Getter of the PDCCH/PDSCH parameters used in the last Tx.
  const alloc_params& last_tx_params() const { return prev_tx_params; }

  /// \brief Processes CRC info for the UL HARQ process.
  /// \return The number of bytes of the TB in case of ack==true, zero in case ack==false, and -1 if HARQ is inactive.
  int crc_info(bool ack);

  /// \brief Called on every new transmission. It marks this HARQ process as busy and stores respective TB information.
  void new_tx(slot_point pusch_slot, unsigned max_harq_retxs);

  /// \brief Called on every TB retransmission. This function assumes that the HARQ TB is in pending new_retx state.
  void new_retx(slot_point pusch_slot);

  /// \brief Stores grant parameters that are associated with the HARQ allocation (e.g. DCI format, PRBs, MCS) so that
  /// they can be later fetched and optionally reused.
  void save_alloc_params(dci_ul_rnti_config_type dci_cfg_type, const pusch_information& pusch);

  /// Cancels the HARQ and stops retransmitting the specified TB until the next new transmission.
  void cancel_harq_retxs();

  /// \brief Getter of the number of bytes of the last transmitted TB.
  int get_tbs_bytes() const { return prev_tx_params.tbs_bytes; }

private:
  /// Parameters used for the last Tx of this HARQ process.
  alloc_params prev_tx_params;
};

/// \brief Helper function to fill HARQ allocation grant parameters.
void save_harq_alloc_params(ul_harq_process::alloc_params& ul_alloc,
                            bwp_id_t                       bwp_id,
                            dci_ul_rnti_config_type        dci_cfg_type,
                            unsigned                       time_resource,
                            const pusch_information&       pusch);

/// This object handles the DL and UL HARQ processes for each UE's Component Carrier; therefore, for each UE, we
/// instantiate as many harq_entity object as CCs
class harq_entity
{
public:
  /// \brief Creates a HARQ entity for a given UE cell.
  /// \param rnti RNTI of the UE
  /// \param nof_dl_harq_procs Number of DL HARQ processes that the UE can support. This value is derived based on
  /// the UE capabilities, and passed to the UE via RRC signalling. See TS38.331, "nrofHARQ-ProcessesForPDSCH".
  /// Values: {2, 4, 6, 10, 12, 16}.
  /// \param nof_ul_harq_procs Number of UL HARQ processes that gNB can support. This value is implementation-defined
  /// and can up to 16 (there are up to 4 bits for HARQ-Id signalling).
  /// \param max_ack_wait_in_slots Duration in slots before a HARQ process acknowledgement is considered to have gone
  /// missing and that the HARQ can be reset.
  explicit harq_entity(rnti_t                   rnti,
                       unsigned                 nof_dl_harq_procs,
                       unsigned                 nof_ul_harq_procs,
                       ue_harq_timeout_notifier timeout_notif,
                       unsigned                 ntn_cs_koffset = 0,
                       unsigned max_ack_wait_in_slots          = detail::harq_process<true>::DEFAULT_ACK_TIMEOUT_SLOTS);

  /// Update slot, and checks if there are HARQ processes that have reached maxReTx with no ACK
  void slot_indication(slot_point slot_tx_);

  /// \brief Update the state of the DL HARQ for the specified UCI slot.
  /// \return struct containing the HARQ process ID, MCS, TBS and status update.
  /// ACK/NACK was directed was found.
  dl_harq_process::dl_ack_info_result
  dl_ack_info(slot_point uci_slot, mac_harq_ack_report_status ack, uint8_t harq_bit_idx, optional<float> pucch_snr);

  /// Update UL HARQ state given the received CRC indication.
  /// \return Transport Block size of the HARQ whose state was updated.
  int ul_crc_info(harq_id_t h_id, bool ack, slot_point pusch_slot);

  /// \brief The UCI scheduling associated with a given slot was cancelled. The associated DL HARQs will be NACKEd.
  ///
  /// This function can be called for instance when there is an error indication coming from lower layers.
  void dl_ack_info_cancelled(slot_point uci_slot);

  uint32_t               nof_dl_harqs() const { return dl_harqs.size(); }
  uint32_t               nof_ul_harqs() const { return ul_harqs.size(); }
  const dl_harq_process& dl_harq(uint32_t pid) const { return dl_harqs[pid]; }
  dl_harq_process&       dl_harq(uint32_t pid) { return dl_harqs[pid]; }
  const ul_harq_process& ul_harq(uint32_t pid) const { return ul_harqs[pid]; }
  ul_harq_process&       ul_harq(uint32_t pid) { return ul_harqs[pid]; }

  dl_harq_process* find_pending_dl_retx()
  {
    harq_id_t h_id = find_oldest_harq_retx(dl_harqs);
    return h_id == INVALID_HARQ_ID ? nullptr : &dl_harqs[h_id];
  }
  const dl_harq_process* find_pending_dl_retx() const
  {
    harq_id_t h_id = find_oldest_harq_retx(dl_harqs);
    return h_id == INVALID_HARQ_ID ? nullptr : &dl_harqs[h_id];
  }
  ul_harq_process* find_pending_ul_retx()
  {
    harq_id_t h_id = find_oldest_harq_retx(ul_harqs);
    return h_id == INVALID_HARQ_ID ? nullptr : &ul_harqs[h_id];
  }
  const ul_harq_process* find_pending_ul_retx() const
  {
    harq_id_t h_id = find_oldest_harq_retx(ul_harqs);
    return h_id == INVALID_HARQ_ID ? nullptr : &ul_harqs[h_id];
  }
  dl_harq_process* find_empty_dl_harq()
  {
    harq_id_t h_id = find_empty_harq(dl_harqs);
    return h_id == INVALID_HARQ_ID ? nullptr : &dl_harqs[h_id];
  }
  const dl_harq_process* find_empty_dl_harq() const
  {
    harq_id_t h_id = find_empty_harq(dl_harqs);
    return h_id == INVALID_HARQ_ID ? nullptr : &dl_harqs[h_id];
  }
  ul_harq_process* find_empty_ul_harq()
  {
    harq_id_t h_id = find_empty_harq(ul_harqs);
    return h_id == INVALID_HARQ_ID ? nullptr : &ul_harqs[h_id];
  }
  const ul_harq_process* find_empty_ul_harq() const
  {
    harq_id_t h_id = find_empty_harq(ul_harqs);
    return h_id == INVALID_HARQ_ID ? nullptr : &ul_harqs[h_id];
  }
  const dl_harq_process* find_dl_harq_waiting_ack() const
  {
    harq_id_t h_id = find_harq_waiting_ack(dl_harqs);
    return h_id == INVALID_HARQ_ID ? nullptr : &dl_harqs[h_id];
  }
  const ul_harq_process* find_ul_harq_waiting_ack() const
  {
    harq_id_t h_id = find_harq_waiting_ack(ul_harqs);
    return h_id == INVALID_HARQ_ID ? nullptr : &ul_harqs[h_id];
  }

  dl_harq_process* find_dl_harq_waiting_ack_slot(slot_point sl_ack, unsigned harq_bit_idx)
  {
    // For the time being, we assume 1 TB only.
    static const size_t tb_index = 0;

    for (unsigned i = 0; i != dl_harqs.size(); ++i) {
      if (dl_harqs[i].tb(tb_index).harq_bit_idx == harq_bit_idx and dl_harqs[i].is_waiting_ack(tb_index) and
          dl_harqs[i].slot_ack() == sl_ack) {
        return &dl_harqs[to_harq_id(i)];
      }
    }
    return nullptr;
  }

  unsigned ntn_get_tbs_pending_crcs() const
  {
    if (ntn_harq.active()) {
      return ntn_harq.get_total_tbs();
    }
    return 0;
  }

private:
  template <typename HarqVector>
  harq_id_t find_oldest_harq_retx(const HarqVector& harqs) const
  {
    harq_id_t oldest_h = harq_id_t::INVALID_HARQ_ID;
    for (unsigned i = 0; i != harqs.size(); ++i) {
      if (harqs[i].has_pending_retx()) {
        if (oldest_h == harq_id_t::INVALID_HARQ_ID or harqs[i].slot_ack() < harqs[oldest_h].slot_ack()) {
          oldest_h = to_harq_id(i);
        }
      }
    }
    return oldest_h;
  }
  template <typename HarqVector>
  harq_id_t find_empty_harq(const HarqVector& harqs) const
  {
    for (unsigned i = 0; i != harqs.size(); ++i) {
      if (harqs[i].empty()) {
        return to_harq_id(i);
      }
    }
    return harq_id_t::INVALID_HARQ_ID;
  }
  template <typename HarqVector>
  harq_id_t find_harq_waiting_ack(const HarqVector& harqs) const
  {
    for (unsigned i = 0; i != harqs.size(); ++i) {
      if (harqs[i].is_waiting_ack()) {
        return to_harq_id(i);
      }
    }
    return harq_id_t::INVALID_HARQ_ID;
  }

  class ntn_tbs_history
  {
  public:
    /// \brief This class is used to store the TBS of the HARQ processes that have to be released before they have been
    /// completed by an ACK or a PUSCH.
    explicit ntn_tbs_history(unsigned ntn_cs_koffset_);

    bool active() const { return ntn_cs_koffset != 0; }

    /// \brief This function is used to save the TBS of the HARQ process.
    void save_dl_harq_info(const dl_harq_process& harq, const slot_point& _slot_tx)
    {
      if (harq.empty()) {
        return;
      }
      if (harq.get_slot_ack_timeout() <= _slot_tx) {
        int idx                         = (harq.slot_ack().to_uint() + ntn_cs_koffset) % slot_ack_info.size();
        slot_ack_info.at(idx).tbs_bytes = harq.last_alloc_params().tb[0]->tbs_bytes;
        slot_ack_info.at(idx).h_id      = harq.id;
        slot_ack_info.at(idx).mcs       = harq.last_alloc_params().tb[0]->mcs;
        slot_ack_info.at(idx).mcs_table = harq.last_alloc_params().tb[0]->mcs_table;
        slot_ack_info.at(idx).update    = dl_harq_process::status_update::acked;
      }
    }

    /// \brief This function is used to save the ack info of the HARQ process.
    void save_ul_harq_info(const ul_harq_process& harq, const slot_point& _slot_tx)
    {
      if (harq.empty()) {
        return;
      }
      if (harq.get_slot_ack_timeout() <= _slot_tx) {
        int idx          = (harq.slot_ack().to_uint() + ntn_cs_koffset) % slot_tbs.size();
        slot_tbs.at(idx) = harq.get_tbs_bytes();
      }
    }

    /// \brief This function is used to pop the ack info of the HARQ process saved at the slot _slot_tx.
    dl_harq_process::dl_ack_info_result pop_ack_info(const slot_point& _slot_tx, mac_harq_ack_report_status ack)
    {
      auto ret = slot_ack_info.at(_slot_tx.to_uint() % slot_ack_info.size());
      if (ret.tbs_bytes) {
        slot_ack_info.at(_slot_tx.to_uint() % slot_ack_info.size()) = {};
      } else {
        return {};
      }
      ret.update = ack == mac_harq_ack_report_status::ack ? dl_harq_process::status_update::acked
                                                          : dl_harq_process::status_update::nacked;
      return ret;
    }

    /// \brief This function is used to pop the TBS of the HARQ process saved at the slot _slot_tx.
    int pop_tbs(const slot_point& _slot_tx)
    {
      unsigned ret = slot_tbs.at(_slot_tx.to_uint() % slot_tbs.size());
      if (ret) {
        slot_tbs.at(_slot_tx.to_uint() % slot_tbs.size()) = 0;
      } else {
        return -1;
      }
      return ret;
    }

    /// \brief This function is used to clear the TBS of the HARQ process saved at the slot _slot_tx.
    void clear_tbs(slot_point _slot_tx) { slot_tbs.at(_slot_tx.to_uint() % slot_tbs.size()) = 0; }
    /// \brief This function is used to get the total TBS of the HARQ processes saved.
    int get_total_tbs() const { return std::accumulate(slot_tbs.begin(), slot_tbs.end(), 0); }
    // We timeout the HARQ since we need to reuse the process before the PUSCH arrives.
    const unsigned ntn_harq_timeout = 1;
    /// \brief The function is used to get the ntn koffset.
    unsigned get_ntn_koffset() const { return ntn_cs_koffset; }

  private:
    std::vector<unsigned>                            slot_tbs;
    std::vector<dl_harq_process::dl_ack_info_result> slot_ack_info;
    unsigned                                         ntn_cs_koffset;
  };
  rnti_t                rnti;
  srslog::basic_logger& logger;
  harq_logger           dl_h_logger;
  harq_logger           ul_h_logger;

  // slot_rx is the slot index at which the scheduler is currently working
  slot_point                   slot_tx;
  std::vector<dl_harq_process> dl_harqs;
  std::vector<ul_harq_process> ul_harqs;
  ue_harq_timeout_notifier     nop_timeout_notifier;
  ntn_tbs_history              ntn_harq;
};

} // namespace srsran
