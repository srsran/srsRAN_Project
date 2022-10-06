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

#include "rlc_am_interconnect.h"
#include "rlc_am_pdu.h"
#include "rlc_am_window.h"
#include "rlc_retx_queue.h"
#include "rlc_sdu_queue.h"
#include "rlc_tx_entity.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/timers.h"
#include <set>

namespace srsgnb {

/// Container to hold a SDU for transmission, the progress in case of segmentation, and associated meta data
struct rlc_tx_am_sdu_info {
  uint32_t    pdcp_count = INVALID_RLC_SN;
  byte_buffer sdu        = {};
  uint32_t    next_so    = 0;
  uint32_t    retx_count = RETX_COUNT_NOT_STARTED;
};

/// \brief Tx state variables
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

  // Tx state variables
  rlc_tx_am_state st;

  // Tx SDU buffers
  rlc_sdu_queue sdu_queue;
  uint32_t      sn_under_segmentation = INVALID_RLC_SN; // SN of the SDU currently being segmented

  // ReTx buffers
  rlc_retx_queue retx_queue;

  // Mutexes
  std::mutex mutex;

  /// Tx counter modulus
  const uint32_t     mod;
  constexpr uint32_t tx_mod_base(uint32_t x) const { return (x - st.tx_next_ack) % mod; }

  /// AM window size
  const uint32_t am_window_size;

  /// Tx window
  std::unique_ptr<rlc_am_window_base<rlc_tx_am_sdu_info>> tx_window;

  // Header sizes are computed upon construction based on SN length
  const uint32_t head_min_size;
  const uint32_t head_max_size;

  /// \brief t-PollRetransmit
  /// This timer is used by the transmitting side of an AM RLC entity in order to retransmit a poll (see sub
  /// clause 5.3.3).
  /// Ref: TS 38.322 Sec. 7.3
  unique_timer poll_retransmit_timer;

public:
  rlc_tx_am_entity(du_ue_index_t                        du_index,
                   lcid_t                               lcid,
                   const rlc_tx_am_config&              config,
                   rlc_tx_upper_layer_data_notifier&    upper_dn,
                   rlc_tx_upper_layer_control_notifier& upper_cn,
                   rlc_tx_lower_layer_notifier&         lower_dn,
                   timer_manager&                       timers,
                   task_executor&                       pcell_executor);

  // Tx/Rx interconnect
  void set_status_provider(rlc_rx_am_status_provider* status_provider_) { status_provider = status_provider_; }

  // Interfaces for higher layers
  void handle_sdu(rlc_sdu sdu) override;

  // Interfaces for lower layers
  byte_buffer_slice_chain pull_pdu(uint32_t nof_bytes) override;

  uint32_t get_buffer_state() override;

  // Status handler interface
  void handle_status_pdu(rlc_am_status_pdu status) override;
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
  void timer_expired(uint32_t timeout_id);

  // Window helpers

  /// \brief Checks if a sequence number is inside the Tx window
  /// \param sn Sequence Number to check
  /// \return true if sn is inside the Tx window, false otherwise
  bool inside_tx_window(uint32_t sn) const;

  /// \brief This function is used to check if a received status report as a valid ACK_SN.
  ///
  /// ACK_SN may be equal to TX_NEXT + AM_Window_Size if the PDU with SN=TX_NEXT+AM_Window_Size has been received by the
  /// RX An ACK_SN == Tx_Next_Ack doesn't ACK or NACKs any PDUs, as such, such a status report can be discarded.
  ///
  /// \param sn Sequence Number to check
  /// \return true if sn is valid, false otherwise
  bool valid_ack_sn(uint32_t sn) const;

private:
  /// \brief Builds a new RLC PDU.
  /// This will be called after checking whether control, retransmission,
  /// or segment PDUs needed to be transmitted first.
  /// It will read an SDU from the SDU queue, build a new PDU, and add it to the tx_window.
  /// SDU segmentation is applied if necessary.
  ///
  /// An empty PDU is returned if nof_bytes is insufficient or the TX buffer is empty.
  ///
  /// \param nof_bytes Limits the maximum size of the requested PDU.
  /// \return One PDU
  byte_buffer_slice_chain build_new_pdu(uint32_t nof_bytes);

  /// \brief Builds a RLC PDU containing the first segment of a new SDU.
  ///
  /// This function will set sn_under_segmentation to the sequence number of the SDU under segmentation.
  ///
  /// \param tx_pdu the tx_pdu info contained in the tx_window.
  /// \param nof_bytes Limits the maximum size of the requested PDU.
  /// \return One PDU
  byte_buffer_slice_chain build_first_sdu_segment(rlc_tx_am_sdu_info& sdu_info, uint32_t nof_bytes);

  /// \brief Builds a RLC PDU containing an SDU segment for an SDU that is undergoing segmentation.
  ///
  /// This function will reset sn_under_segmentation to RLC_INVALID_SN if the produced PDU contains
  /// the last segment of the SDU under segmentation.
  ///
  /// \param tx_pdu The tx_pdu info contained in the tx_window.
  /// \param nof_bytes Limits the maximum size of the requested PDU.
  /// \return One PDU
  byte_buffer_slice_chain build_continued_sdu_segment(rlc_tx_am_sdu_info& sdu_info, uint32_t nof_bytes);

  /// \brief Builds a retx RLC PDU.
  ///
  /// This will use the retx_queue to get information about the RLC PDU
  /// being retx'ed. The retx may have been previously transmitted as
  /// a full PDU or an PDU segment(s).
  ///
  /// \param nof_bytes Limits the maximum size of the requested PDU.
  /// \return One PDU or PDU segment segment
  byte_buffer_slice_chain build_retx_pdu(uint32_t nof_bytes);

  constexpr uint32_t get_retx_expected_hdr_len(const rlc_tx_amd_retx retx)
  {
    return retx.so == 0 ? head_min_size : head_max_size;
  }

  /// \brief Schedules ReTx for NACK'ed PDUs
  ///
  /// NACKs will be dropped if the SN falls out of the tx window or if the NACK'ed
  /// PDU or PDU segment is already queued for ReTx.
  /// Invalid or out of bounds segment offsets are adjusted to SDU boundaries
  ///
  /// Note: This function must not be called with NACKs that have a nack range.
  /// Instead, nacks with nack range must be decomposed into individual NACKs.
  ///
  /// \param nack The NACK to be processed. The NACK must not have a nack range.
  /// \return true if NACK was handled and queued successfully, false if NACK has been ignored
  bool handle_nack(rlc_am_status_nack nack);

  /// \brief Helper to check if a SN has reached the max reTx threshold
  ///
  /// Caller _must_ hold the mutex when calling the function.
  /// If the retx has been reached for a SN the upper layers (i.e. RRC/PDCP) will be informed.
  /// The SN is _not_ removed from the Tx window, so retransmissions of that SN can still occur.
  /// \param sn The SN of the PDU to check
  void check_sn_reached_max_retx(uint32_t sn);

  /// Called when buffer state needs to be updated and forwarded to lower layers.
  void handle_buffer_state_update();
  /// Called when buffer state needs to be updated and forwarded to lower layers while already holding a lock.
  void handle_buffer_state_update_nolock();

  uint32_t get_buffer_state_nolock();

  /// Creates the tx_window according to sn_size
  /// \param sn_size Size of the sequence number (SN)
  /// \return unique pointer to tx_window instance
  std::unique_ptr<rlc_am_window_base<rlc_tx_am_sdu_info>> create_tx_window(rlc_am_sn_size sn_size);

  void log_state(srslog::basic_levels level)
  {
    fmt::memory_buffer buffer;
    if (sn_under_segmentation == INVALID_RLC_SN) {
      fmt::format_to(buffer, "{}", "none");
    } else {
      fmt::format_to(buffer, "{}", sn_under_segmentation);
    }
    //
    // TODO: avoid string construction
    //
    logger.log(level, "TX entity state: st=[{}], sn_under_segmentation={}", st, fmt::to_string(buffer));
  }
};

} // namespace srsgnb

namespace fmt {
template <>
struct formatter<srsgnb::rlc_tx_am_state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::rlc_tx_am_state& st, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "tx_next_ack={}, tx_next={}, poll_sn={}, pdu_without_poll={}, byte_without_poll={}",
                     st.tx_next_ack,
                     st.tx_next,
                     st.poll_sn,
                     st.pdu_without_poll,
                     st.byte_without_poll);
  }
};

} // namespace fmt
