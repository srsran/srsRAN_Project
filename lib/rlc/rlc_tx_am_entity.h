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

#include "rlc_am_interconnect.h"
#include "rlc_am_pdu.h"
#include "rlc_pdu_recycler.h"
#include "rlc_retx_queue.h"
#include "rlc_sdu_queue.h"
#include "rlc_sdu_window.h"
#include "rlc_tx_entity.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include "fmt/format.h"

namespace srsran {

/// Container to hold a SDU for transmission, the progress in case of segmentation, and associated meta data
struct rlc_tx_am_sdu_info {
  byte_buffer        sdu = {};
  optional<uint32_t> pdcp_sn;
  uint32_t           next_so    = 0;
  uint32_t           retx_count = RETX_COUNT_NOT_STARTED;
};

/// \brief TX state variables
/// Ref: 3GPP TS 38.322 version 16.2.0 Section 7.1
struct rlc_tx_am_state {
  /// \brief TX_Next_Ack – Acknowledgement state variable
  /// It holds the value of the SN of the next RLC SDU for which a positive acknowledgment is to be received
  /// in-sequence, and it serves as the lower edge of the transmitting window. It is initially set to 0, and is
  /// updated whenever the AM RLC entity receives a positive acknowledgment for an RLC SDU with SN = TX_Next_Ack.
  uint32_t tx_next_ack = 0;

  /// \brief  TX_Next – AM send state variable
  /// It holds the value of the SN to be assigned for the next newly generated AMD PDU. It is
  /// initially set to 0, and is updated whenever the AM RLC entity constructs an AMD PDU
  /// with SN = TX_Next and contains an RLC SDU or the last segment of a RLC SDU.
  uint32_t tx_next = 0;

  /// \brief POLL_SN – Poll send state variable
  /// It holds the value of the highest SN of the AMD PDU among the AMD PDUs submitted to lower layer when POLL_SN is
  /// set according to sub clause 5.3.3.2. It is initially set to 0.
  uint32_t poll_sn = 0;

  /// \brief PDU_WITHOUT_POLL – Counter
  /// This counter is initially set to 0. It counts the number of AMD PDUs sent since the most recent poll bit was
  /// transmitted.
  uint32_t pdu_without_poll = 0;

  /// \brief BYTE_WITHOUT_POLL – Counter
  /// This counter is initially set to 0. It counts the number of data bytes sent since the most recent poll bit was
  /// transmitted.
  uint32_t byte_without_poll = 0;
};

class rlc_tx_am_entity : public rlc_tx_entity, public rlc_tx_am_status_handler, public rlc_tx_am_status_notifier
{
private:
  // Config storage
  const rlc_tx_am_config cfg;

  // Status interface
  rlc_rx_am_status_provider* status_provider = nullptr;

  // TX state variables
  rlc_tx_am_state st;

  // TX SDU buffers
  rlc_sdu_queue sdu_queue;
  uint32_t      sn_under_segmentation = INVALID_RLC_SN; // SN of the SDU currently being segmented

  // RETX buffers
  rlc_retx_queue retx_queue;

  // Mutexes
  std::mutex mutex;

  /// TX counter modulus
  const uint32_t     mod;
  constexpr uint32_t tx_mod_base(uint32_t x) const { return (x - st.tx_next_ack) % mod; }

  /// AM window size
  const uint32_t am_window_size;

  /// TX window
  std::unique_ptr<rlc_sdu_window_base<rlc_tx_am_sdu_info>> tx_window;

  /// Recycler for discarded PDUs (from tx_window) that shall be deleted by a different executor off the critical path
  rlc_pdu_recycler pdu_recycler;

  // Header sizes are computed upon construction based on SN length
  const uint32_t head_min_size;
  const uint32_t head_max_size;

  /// \brief t-PollRetransmit
  /// This timer is used by the transmitting side of an AM RLC entity in order to retransmit a poll (see sub
  /// clause 5.3.3).
  /// Ref: TS 38.322 Sec. 7.3
  unique_timer      poll_retransmit_timer;
  std::atomic<bool> is_poll_retransmit_timer_expired;

  task_executor& pcell_executor;
  task_executor& ue_executor;

  // Storage for previous buffer state
  unsigned prev_buffer_state = 0;

  /// This atomic_flag indicates whether a buffer state update task has been queued but not yet run by pcell_executor.
  /// It helps to avoid queuing of redundant notification tasks in case of frequent changes of the buffer status.
  /// If the flag is set, no further notification needs to be scheduled, because the already queued task will pick the
  /// latest buffer state upon execution.
  std::atomic_flag pending_buffer_state = ATOMIC_FLAG_INIT;

public:
  rlc_tx_am_entity(du_ue_index_t                        du_index,
                   rb_id_t                              rb_id,
                   const rlc_tx_am_config&              config,
                   rlc_tx_upper_layer_data_notifier&    upper_dn_,
                   rlc_tx_upper_layer_control_notifier& upper_cn_,
                   rlc_tx_lower_layer_notifier&         lower_dn_,
                   timer_factory                        timers,
                   task_executor&                       pcell_executor_,
                   task_executor&                       ue_executor_);

  // TX/RX interconnect
  void set_status_provider(rlc_rx_am_status_provider* status_provider_) { status_provider = status_provider_; }

  // Interfaces for higher layers
  void handle_sdu(rlc_sdu sdu) override;
  void discard_sdu(uint32_t pdcp_sn) override;

  // Interfaces for lower layers
  byte_buffer_chain pull_pdu(uint32_t grant_len) override;

  uint32_t get_buffer_state() override;

  // Status handler interface
  void on_status_pdu(rlc_am_status_pdu status) override;
  // Status notifier interface
  void on_status_report_changed() override;

  /// \brief Determines whether the polling bit in a PDU header has to be set or not
  /// Ref: TS 38.322, Sec. 5.3.3.2
  /// \param sn Sequence number of PDU to be transmitted
  /// \param is_retx Flags whether this PDU is a retransmission or not
  /// \param payload_size Length of the data field mapped to the AMD PDU
  /// \return 1 if the polling bit has to be set, otherwise 0
  uint8_t get_polling_bit(uint32_t sn, bool is_retx, uint32_t payload_size);

  // Timers

  /// \brief on_expired_poll_retransmit_timer Handler for expired poll_retransmit_timer
  ///
  /// Note: This function shall be executed by the same executor that calls pull_pdu(), i.e. the pcell_executor,
  /// in order to avoid incidential blocking of those critical paths.
  void on_expired_poll_retransmit_timer();

  // Window helpers

  /// \brief Checks if a sequence number is inside the TX window
  /// \param sn Sequence Number to check
  /// \return true if sn is inside the TX window, false otherwise
  bool inside_tx_window(uint32_t sn) const;

  /// \brief This function is used to check if a received status report as a valid ACK_SN.
  ///
  /// ACK_SN may be equal to TX_NEXT + AM_Window_Size if the PDU with SN=TX_NEXT+AM_Window_Size has been received by the
  /// RX An ACK_SN == Tx_Next_Ack doesn't ACK or NACKs any PDUs, as such, such a status report can be discarded.
  ///
  /// \param sn Sequence Number to check
  /// \return true if sn is valid, false otherwise
  bool valid_ack_sn(uint32_t sn) const;

  /// \brief This function is used to check if a received NACK within a status report is valid.
  ///
  /// \param ack_sn ACK_SN of status report
  /// \param nack NACK to check validity of
  /// \return true if NACK is valid, false otherwise
  bool valid_nack(uint32_t ack_sn, const rlc_am_status_nack& nack_sn) const;
  /*
   * Testing helpers
   */

  /// Set the internal state variables
  /// Note: This function should only be used for testing
  ///
  /// \param st_ The state variables to be set
  void set_state(const rlc_tx_am_state& st_) { this->st = st_; }

  /// Get the internal state variables
  /// Note: This function should only be used for testing
  ///
  /// \return A copy of the internal state variables
  rlc_tx_am_state get_state() { return st; }

private:
  /// \brief Builds a new RLC PDU.
  /// This will be called after checking whether control, retransmission,
  /// or segment PDUs needed to be transmitted first.
  /// It will read an SDU from the SDU queue, build a new PDU, and add it to the tx_window.
  /// SDU segmentation is applied if necessary.
  ///
  /// An empty PDU is returned if grant_len is insufficient or the TX buffer is empty.
  ///
  /// \param grant_len Limits the maximum size of the requested PDU.
  /// \return One PDU
  byte_buffer_chain build_new_pdu(uint32_t grant_len);

  /// \brief Builds a RLC PDU containing the first segment of a new SDU.
  ///
  /// This function will set sn_under_segmentation to the sequence number of the SDU under segmentation.
  ///
  /// \param tx_pdu the tx_pdu info contained in the tx_window.
  /// \param grant_len Limits the maximum size of the requested PDU.
  /// \return One PDU
  byte_buffer_chain build_first_sdu_segment(rlc_tx_am_sdu_info& sdu_info, uint32_t grant_len);

  /// \brief Builds a RLC PDU containing an SDU segment for an SDU that is undergoing segmentation.
  ///
  /// This function will reset sn_under_segmentation to RLC_INVALID_SN if the produced PDU contains
  /// the last segment of the SDU under segmentation.
  ///
  /// \param tx_pdu The tx_pdu info contained in the tx_window.
  /// \param grant_len Limits the maximum size of the requested PDU.
  /// \return One PDU
  byte_buffer_chain build_continued_sdu_segment(rlc_tx_am_sdu_info& sdu_info, uint32_t grant_len);

  /// \brief Builds a RETX RLC PDU.
  ///
  /// This will use the retx_queue to get information about the RLC PDU
  /// being RETX'ed. The RETX may have been previously transmitted as
  /// a full PDU or an PDU segment(s).
  ///
  /// \param grant_len Limits the maximum size of the requested PDU.
  /// \return One PDU or PDU segment segment
  byte_buffer_chain build_retx_pdu(uint32_t grant_len);

  constexpr uint32_t get_retx_expected_hdr_len(const rlc_tx_amd_retx retx)
  {
    return retx.so == 0 ? head_min_size : head_max_size;
  }

  /// \brief Evaluates a status PDU, schedules RETX and removes ACK'ed SDUs from TX window
  /// \param status The status PDU
  void handle_status_pdu(rlc_am_status_pdu status);

  /// \brief Schedules RETX for NACK'ed PDUs
  ///
  /// NACKs will be dropped if the SN falls out of the tx window or if the NACK'ed
  /// PDU or PDU segment is already queued for RETX or the RETX queue is full.
  /// Invalid or out of bounds segment offsets are adjusted to SDU boundaries
  ///
  /// Note: This function must not be called with NACKs that have a nack range.
  /// Instead, nacks with nack range must be decomposed into individual NACKs.
  ///
  /// \param nack The NACK to be processed. The NACK must not have a nack range.
  /// \return true if NACK was handled and queued successfully, false if NACK has been ignored
  bool handle_nack(rlc_am_status_nack nack);

  /// \brief Increments the retx_count for a given SN and checks if max_retx is reached.
  ///
  /// Caller _must_ hold the mutex when calling the function.
  /// \param sn The SN of the SDU for which the retx_counter shall be incremented.
  void increment_retx_count(uint32_t sn);

  /// \brief Helper to check if a SN has reached the max RETX threshold
  ///
  /// Caller _must_ hold the mutex when calling the function.
  /// If the RETX has been reached for a SN the upper layers (i.e. RRC/PDCP) will be informed.
  /// The SN is _not_ removed from the TX window, so retransmissions of that SN can still occur.
  /// \param sn The SN of the PDU to check
  void check_sn_reached_max_retx(uint32_t sn);

  /// Called whenever the buffer state has been changed by upper layers (i.e new SDU or SDU discard) or the size of the
  /// own status PDU has changed so that lower layers need to be informed about the new buffer state. This function
  /// defers the actual notification \c handle_changed_buffer_state to pcell_executor. The notification is discarded if
  /// another notification is already queued for execution. This function should not be called from \c pull_pdu, since
  /// the lower layer accounts for the amount of extracted data itself.
  ///
  /// Safe execution from: Any executor
  void handle_changed_buffer_state();

  /// Immediately informs the lower layer of the current buffer state. This function must be called from pcell_executor.
  /// It is used for buffer update notifications by non-excessive internal RLC events (i.e. expired poll retransmit
  /// timer or handled received status report), and for defered handling of \c handle_changed_buffer_state. This
  /// function should not be called from \c pull_pdu, since the lower layer accounts for the amount of extracted data
  /// itself.
  ///
  /// Safe execution from: pcell_executor
  /// \param is_locked provides info whether the \c mutex is already locked or not.
  void update_mac_buffer_state(bool is_locked);

  /// Lock-free version of \c get_buffer_state()
  /// \return Provides the current buffer state
  uint32_t get_buffer_state_nolock();

  /// Creates the tx_window according to sn_size
  /// \param sn_size Size of the sequence number (SN)
  /// \return unique pointer to tx_window instance
  std::unique_ptr<rlc_sdu_window_base<rlc_tx_am_sdu_info>> create_tx_window(rlc_am_sn_size sn_size);

  void log_state(srslog::basic_levels level)
  {
    if (sn_under_segmentation == INVALID_RLC_SN) {
      logger.log(level, "TX entity state. {} sn_under_segmentation=none", st, sn_under_segmentation);
    } else {
      logger.log(level, "TX entity state. {} sn_under_segmentation={}", st, sn_under_segmentation);
    }
  }
};

} // namespace srsran

namespace fmt {
template <>
struct formatter<srsran::rlc_tx_am_state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_tx_am_state& st, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "tx_next_ack={} tx_next={} poll_sn={} pdu_without_poll={} byte_without_poll={}",
                     st.tx_next_ack,
                     st.tx_next,
                     st.poll_sn,
                     st.pdu_without_poll,
                     st.byte_without_poll);
  }
};

} // namespace fmt
