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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/sch_mcs.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/scheduler/sched_consts.h"
#include "srsran/scheduler/scheduler_dci.h"
#include "srsran/support/format_utils.h"

namespace srsran {

struct pdsch_information;
struct pusch_information;

/// \brief Handler of HARQ process timeouts.
class harq_timeout_handler
{
public:
  virtual ~harq_timeout_handler() = default;

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
    ch("{} HARQ rnti={:#x} cell={} h_id={}: {}", is_dl ? "DL" : "UL", rnti, cell_index, h_id, to_c_str(fmtbuf));
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

  constexpr static unsigned SHORT_ACK_TIMEOUT_DTX = 10U;

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

    /// Downlink Assignment Index used in case of PDSCH.
    uint8_t dai;

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
  const transport_block& tb(unsigned tb_idx) const
  {
    srsran_assert(not empty(tb_idx), "TB {} is not active", tb_idx);
    return tb_array[tb_idx];
  }

  /// \brief Resets HARQ process state.
  void reset();

  /// Forbids the HARQ from retransmitting the specified TB until the next new transmission.
  void stop_retransmissions(unsigned tb_idx);

protected:
  void tx_common(slot_point slot_tx, slot_point slot_ack);
  void new_tx_tb_common(unsigned tb_idx, unsigned max_nof_harq_retxs, uint8_t dai);
  void new_retx_tb_common(unsigned tb_idx, uint8_t dai);

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

  /// Actual time interval, in slots, before the HARQ process assumes that the ACK/CRC went missing.
  /// This value is shorten dynamically when the MAC returns a ACK with DTX (not correctly decoded) state; it gets
  /// re-set to its maximum value at the beginning of each transmission or retx, and after receiving a ACK or NACK.
  unsigned ack_wait_in_slots;

  /// For DL, slot_tx corresponds to the slot when the TB in the HARQ process is going to be transmitted by the gNB.
  /// For UL, slot_tx corresponds to the slot when the TB in the HARQ process is going to be transmitted by the UE.
  slot_point last_slot_tx;

  /// For DL, slot_ack is the slot at which gNB is expected to receive the ACK via UCI.
  /// For UL, slot_ack is the slot when the PUSCH CRC will be received by the gNB. It coincides with slot_tx of UL.
  slot_point last_slot_ack;

  std::array<transport_block, MAX_NOF_TBS> tb_array;
};

} // namespace detail

class dl_harq_process : public detail::harq_process<true>
{
  using base_type = detail::harq_process<true>;

public:
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
  };

  using base_type::transport_block;

  using base_type::empty;
  using base_type::harq_process;
  using base_type::has_pending_retx;

  /// \brief Checks whether the HARQ process has no TBs currently active.
  bool empty() const { return base_type::empty(0) and base_type::empty(1); }

  /// \brief Checks whether there is at least one TB with pending new_retx.
  bool has_pending_retx() const { return base_type::has_pending_retx(0) or base_type::has_pending_retx(1); }

  /// \brief Getter of the PDCCH/PDSCH parameters used in the last Tx.
  const alloc_params& last_alloc_params() const { return prev_tx_params; }

  /// \brief Called on every new TB transmission, when only one TB is active. It marks this HARQ process as busy and
  /// stores respective TB information.
  void new_tx(slot_point pdsch_slot, unsigned k1, unsigned max_harq_nof_retxs, uint8_t dai);

  /// \brief Called on every TB retransmission, when only one TB is active. This function assumes that the HARQ TB is
  /// in pending new_retx state.
  void new_retx(slot_point pdsch_slot, unsigned k1, uint8_t dai);

  /// \brief Called on every new TB transmission/retransmission, when 2 TBs are used.
  enum class tb_tx_request { newtx, retx, disabled };
  void tx_2_tb(slot_point                pdsch_slot,
               unsigned                  k1,
               span<const tb_tx_request> tb_tx_req,
               unsigned                  max_harq_nof_retxs,
               uint8_t                   dai);

  /// \brief Updates the ACK state of the HARQ process.
  /// \return True if harq was not empty and state was succesfully updated. False, otherwise.
  bool ack_info(uint32_t tb_idx, mac_harq_ack_report_status ack);

  /// \brief Stores grant parameters that are associated with the HARQ allocation (e.g. DCI format, PRBs, MCS) so that
  /// they can be later fetched and optionally reused.
  void save_alloc_params(dci_dl_rnti_config_type dci_cfg_type, const pdsch_information& pdsch);

private:
  /// Parameters used for the last Tx of this HARQ process.
  alloc_params prev_tx_params;
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

  /// Forbids the HARQ from retransmitting the specified TB until the next new transmission.
  void stop_retransmissions();

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
                       unsigned max_ack_wait_in_slots = detail::harq_process<true>::DEFAULT_ACK_TIMEOUT_SLOTS);

  /// Update slot, and checks if there are HARQ processes that have reached maxReTx with no ACK
  void slot_indication(slot_point slot_tx_);

  /// \brief Update the state of the DL HARQ for the specified UCI slot.
  /// \return HARQ process whose state was updated. Nullptr, if no HARQ for which the ACK/NACK was directed was found.
  const dl_harq_process* dl_ack_info(slot_point uci_slot, mac_harq_ack_report_status ack, uint8_t dai);

  /// Update UL HARQ state given the received CRC indication.
  int ul_crc_info(harq_id_t h_id, bool ack, slot_point pusch_slot);

  uint32_t               nof_dl_harqs() const { return dl_harqs.size(); }
  uint32_t               nof_ul_harqs() const { return ul_harqs.size(); }
  const dl_harq_process& dl_harq(uint32_t pid) const { return dl_harqs[pid]; }
  dl_harq_process&       dl_harq(uint32_t pid) { return dl_harqs[pid]; }
  const ul_harq_process& ul_harq(uint32_t pid) const { return ul_harqs[pid]; }
  ul_harq_process&       ul_harq(uint32_t pid) { return ul_harqs[pid]; }

  dl_harq_process* find_pending_dl_retx()
  {
    return find_dl([](const dl_harq_process& h) { return h.has_pending_retx(); });
  }
  const dl_harq_process* find_pending_dl_retx() const
  {
    return find_dl([](const dl_harq_process& h) { return h.has_pending_retx(); });
  }
  ul_harq_process* find_pending_ul_retx()
  {
    return find_ul([](const ul_harq_process& h) { return h.has_pending_retx(); });
  }
  const ul_harq_process* find_pending_ul_retx() const
  {
    return find_ul([](const ul_harq_process& h) { return h.has_pending_retx(); });
  }
  dl_harq_process* find_empty_dl_harq()
  {
    return find_dl([](const dl_harq_process& h) { return h.empty(); });
  }
  const dl_harq_process* find_empty_dl_harq() const
  {
    return find_dl([](const dl_harq_process& h) { return h.empty(); });
  }
  ul_harq_process* find_empty_ul_harq()
  {
    return find_ul([](const ul_harq_process& h) { return h.empty(); });
  }
  const ul_harq_process* find_empty_ul_harq() const
  {
    return find_ul([](const ul_harq_process& h) { return h.empty(); });
  }

private:
  template <typename Predicate>
  dl_harq_process* find_dl(Predicate p)
  {
    auto it = std::find_if(dl_harqs.begin(), dl_harqs.end(), p);
    return (it == dl_harqs.end()) ? nullptr : &(*it);
  }
  template <typename Predicate>
  const dl_harq_process* find_dl(Predicate p) const
  {
    auto it = std::find_if(dl_harqs.begin(), dl_harqs.end(), p);
    return (it == dl_harqs.end()) ? nullptr : &(*it);
  }
  template <typename Predicate>
  ul_harq_process* find_ul(Predicate p)
  {
    auto it = std::find_if(ul_harqs.begin(), ul_harqs.end(), p);
    return (it == ul_harqs.end()) ? nullptr : &(*it);
  }
  template <typename Predicate>
  const ul_harq_process* find_ul(Predicate p) const
  {
    auto it = std::find_if(ul_harqs.begin(), ul_harqs.end(), p);
    return (it == ul_harqs.end()) ? nullptr : &(*it);
  }

  rnti_t                rnti;
  srslog::basic_logger& logger;
  harq_logger           dl_h_logger;
  harq_logger           ul_h_logger;

  // slot_rx is the slot index at which the scheduler is currently working
  slot_point                   slot_tx;
  std::vector<dl_harq_process> dl_harqs;
  std::vector<ul_harq_process> ul_harqs;
};

} // namespace srsran
