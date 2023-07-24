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
#include "rlc_rx_entity.h"
#include "rlc_sdu_window.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include "fmt/format.h"
#include <set>

namespace srsran {

/// AM SDU segment container
struct rlc_rx_am_sdu_segment {
  rlc_si_field      si;      ///< Segmentation info
  uint16_t          so;      ///< Segment offset (SO)
  byte_buffer_slice payload; ///< Payload (SDU segment)
};

/// AM SDU segment compare object
struct rlc_rx_am_sdu_segment_cmp {
  bool operator()(const rlc_rx_am_sdu_segment& a, const rlc_rx_am_sdu_segment& b) const { return a.so < b.so; }
};

/// Container to collect received SDU segments and to assemble the SDU upon completion
struct rlc_rx_am_sdu_info {
  // TODO: Refactor this struct.
  // Move the following rlc_rx_am methods here:
  // - add segments without duplicates
  // - assemble SDU
  bool                                                       fully_received = false;
  bool                                                       has_gap        = false;
  std::set<rlc_rx_am_sdu_segment, rlc_rx_am_sdu_segment_cmp> segments; // Set of segments with SO as key
  byte_buffer_chain                                          sdu = {};
};

/// \brief Rx state variables
/// Ref: 3GPP TS 38.322 version 16.2.0 Section 7.1
struct rlc_rx_am_state {
  /// \brief RX_Next – Receive state variable
  /// This state variable holds the value of the SN following the last in-sequence completely received RLC SDU, and it
  /// serves as the lower edge of the receiving window. It is initially set to 0, and is updated whenever the AM RLC
  /// entity receives an RLC SDU with SN = RX_Next.
  uint32_t rx_next = 0;

  /// \brief RX_Next_Status_Trigger – t-Reassembly state variable
  /// This state variable holds the value of the SN following the SN of the RLC SDU which triggered t-Reassembly.
  uint32_t rx_next_status_trigger = 0;

  /// \brief RX_Highest_Status – Maximum STATUS transmit state variable
  /// This state variable holds the highest possible value of the SN which can be indicated by "ACK_SN" when a STATUS
  /// PDU needs to be constructed. It is initially set to 0.
  uint32_t rx_highest_status = 0;

  /// \brief RX_Next_Highest – Highest received state variable
  /// This state variable holds the value of the SN following the SN of the RLC SDU with the highest SN among received
  /// RLC SDUs. It is initially set to 0.
  uint32_t rx_next_highest = 0;
};

class rlc_rx_am_entity : public rlc_rx_entity, public rlc_rx_am_status_provider
{
private:
  /// Config storage
  const rlc_rx_am_config cfg;

  /// Rx state variables
  rlc_rx_am_state st;

  /// Status handler
  rlc_tx_am_status_handler* status_handler = nullptr;
  /// Status notifier
  rlc_tx_am_status_notifier* status_notifier = nullptr;
  /// Indicates whether a status report is required
  std::atomic<bool> do_status = {false};

  /// Rx counter modulus
  const uint32_t mod;
  /// AM window size
  const uint32_t am_window_size;

  /// Rx window
  std::unique_ptr<rlc_sdu_window_base<rlc_rx_am_sdu_info>> rx_window;
  /// Indicates the rx_window has not been changed, i.e. no need to rebuild status report.
  static const bool rx_window_not_changed = false;
  /// Indicates the rx_window has been changed, i.e. need to rebuild status report.
  static const bool rx_window_changed = true;

  /// Cached status report
  rlc_am_status_pdu status_report;
  /// Cached status report size
  std::atomic<uint32_t> status_report_size;
  std::atomic<bool>     status_prohibit_timer_is_running{false};

  /// Mutex for controlled access to the cached status report, e.g. read by the Tx entity in a different executor
  std::mutex status_report_mutex;

  /// \brief t-StatusProhibit
  /// This timer is used by the receiving side of an AM RLC entity in order to prohibit transmission of a STATUS PDU
  /// (see sub clause 5.3.4).
  /// Ref: TS 38.322 Sec. 7.3
  unique_timer status_prohibit_timer;

  /// \brief t-Reassembly
  /// This timer is used by the receiving side of an AM RLC entity [...] in order to detect loss of RLC PDUs at lower
  /// layer (see sub clauses 5.2.2.2 and 5.2.3.2). If t-Reassembly is running, t-Reassembly shall not be started
  /// additionally, i.e.only one t-Reassembly per RLC entity is running at a given time.
  /// Ref: TS 38.322 Sec. 7.3
  unique_timer reassembly_timer;

  task_executor& ue_executor;

public:
  rlc_rx_am_entity(du_ue_index_t                     du_index,
                   rb_id_t                           rb_id,
                   const rlc_rx_am_config&           config,
                   rlc_rx_upper_layer_data_notifier& upper_dn_,
                   timer_factory                     timers,
                   task_executor&                    ue_executor);

  // Rx/Tx interconnect
  void set_status_handler(rlc_tx_am_status_handler* status_handler_) { status_handler = status_handler_; }
  void set_status_notifier(rlc_tx_am_status_notifier* status_notifier_) { status_notifier = status_notifier_; }

  // Interfaces for lower layers
  void handle_pdu(byte_buffer_slice buf) override;

  // Status provider for Rx entity
  rlc_am_status_pdu get_status_pdu() override;
  uint32_t          get_status_pdu_length() override;
  bool              status_report_required() override;

  /// Inform the Tx entity that a status report is triggered (whenever do_status is set to true and t-statusProhibit is
  /// not running), or its size has changed (e.g. further PDUs have been received)
  void notify_status_report_changed();

  /*
   * Window helpers
   */

  /// \brief Helper function for arithmetic comparisons of state variables or SN values
  ///
  /// When performing arithmetic comparisons of state variables or SN values, a modulus base shall be used.
  ///
  /// (...) RX_Next shall be assumed as the modulus base at the (...) receiving side of an AM RLC entity (...). This
  /// modulus base is subtracted from all the values involved, and then an absolute comparison is performed (e.g.
  /// RX_Next <= SN < RX_Next + AM_Window_Size is evaluated as [RX_Next – RX_Next] modulo 2^[sn-FieldLength] <= [SN –
  /// RX_Next] modulo 2^[sn-FieldLength] < [RX_Next + AM_Window_Size – RX_Next] modulo 2^[sn-FieldLength]), where
  /// sn-FieldLength is 12 or 18 for 12 bit SN and 18 bit SN, respectively.
  ///
  /// Ref: TS 38.322 Sec. 7.1
  ///
  /// \param sn The sequence number to be rebased from RX_Next
  /// \return The rebased value of sn
  uint32_t rx_mod_base(uint32_t sn) const { return (sn - st.rx_next) % mod; }

  /// Checks whether a sequence number is inside the current Rx window
  /// \param sn The sequence number to be checked
  /// \return True if sn is inside the Rx window, false otherwise
  bool inside_rx_window(uint32_t sn) const
  {
    // RX_Next <= SN < RX_Next + AM_Window_Size
    return rx_mod_base(sn) < am_window_size;
  }

  /// This function is used to check if the Rx_Highest_Status is valid when t-Reasseambly expires.
  ///
  /// ACK_SN may be equal to RX_NEXT + AM_Window_Size if the PDU with SN=RX_NEXT+AM_Window_Size has been received by the
  /// RX. An ACK_SN == Rx_Next should not update Rx_Highest_Status, it should be updated when Rx_Next is updated.
  /// \param sn The sequence number to be checked
  /// \return True if sn is a valid ACK_SN, false otherwise
  bool valid_ack_sn(uint32_t sn) const
  {
    // RX_Next < SN <= RX_Next + AM_Window_Size
    return (0 < rx_mod_base(sn)) && (rx_mod_base(sn) <= am_window_size);
  }

  /*
   * Testing helpers
   */
  /// Set the internal state variables
  /// Note: This function should only be used for testing
  ///
  /// \param st_ The state variables to be set
  void set_state(const rlc_rx_am_state& st_) { this->st = st_; }

  /// Get the internal state variables
  /// Note: This function should only be used for testing
  ///
  /// \return A copy of the internal state variables
  rlc_rx_am_state get_state() { return st; }

  /// Get whether the reassembly timer is running
  /// Note: This function should only be used for testing
  ///
  /// \return true is t-Reassembly is running
  bool is_t_reassembly_running() { return reassembly_timer.is_running(); }

private:
  /// Handles a received control PDU. The PDU is unpacked and forwarded to the Rx entity
  /// \param buf The control PDU to be handled (including header and payload)
  void handle_control_pdu(byte_buffer_slice buf);

  /// Handles a received data PDU which contains an SDU or SDU segment, puts it into the receive window if required,
  /// reassembles the SDU if possible and forwards it to upper layer.
  ///
  /// \param buf The data PDU to be handled (including header and payload)
  void handle_data_pdu(byte_buffer_slice buf);

  /// Handles a received data PDU which contains an SDU, puts it into the receive window and forwards it to upper layer.
  ///
  /// \param header The header of the PDU, used for sanity check
  /// \param payload The PDU payload, i.e. the SDU
  /// \return True if SDU was stored. False otherwise (i.e. SDU was dropped).
  bool handle_full_data_sdu(const rlc_am_pdu_header& header, byte_buffer_slice payload);

  /// Handles a received data PDU which contains an SDU segment, puts it into the receive window if required,
  /// reassembles the SDU if possible and forwards it to upper layer.
  ///
  /// \param header The header of the PDU, used for sanity check and tracking of the segment offset
  /// \param payload The PDU payload, i.e. the SDU segment
  /// \return True if segment was added and repository was changed. False otherwise (i.e. new segment was dropped).
  bool handle_segment_data_sdu(const rlc_am_pdu_header& header, byte_buffer_slice payload);

  /// Stores a newly received SDU segment and avoids overlapping bytes.
  /// Overlaps are prevented by either trimming the new or previously received segment; or dropping the new segment
  /// entirely if all of its bytes are already present.
  ///
  /// \param sdu_info Container/Info object of the associated SDU
  /// \param new_segment The newly received SDU segment
  /// \return True if segment was added and repository was changed. False otherwise (i.e. new segment was dropped).
  bool store_segment(rlc_rx_am_sdu_info& sdu_info, rlc_rx_am_sdu_segment new_segment);

  /// Iterates the received SDU segments to check whether it is fully received and whether the received segments are
  /// contiguous or have gaps.
  ///
  /// \param rx_sdu Container/Info object to be inspected
  void update_segment_inventory(rlc_rx_am_sdu_info& rx_sdu) const;

  /// Rebuilds the cached status_report according to missing SDUs and SDU segments in rx_window
  /// and resets the rx_window_changed flag
  void refresh_status_report();

  /// Replaces the cached status_report with a new version
  void store_status_report(rlc_am_status_pdu&& status);

  void on_expired_status_prohibit_timer();

  /// \brief on_expired_reassembly_timer Handler for expired reassembly timer
  ///
  /// Note: This function shall be executed by the same executor that calls handle_sdu(), i.e. the ue_executor,
  /// in order to avoid incidential blocking of those critical paths.
  ///
  /// \param timeout_id The timer ID
  void on_expired_reassembly_timer();

  /// Creates the rx_window according to sn_size
  /// \param sn_size Size of the sequence number (SN)
  /// \return unique pointer to rx_window instance
  std::unique_ptr<rlc_sdu_window_base<rlc_rx_am_sdu_info>> create_rx_window(rlc_am_sn_size sn_size);

  void log_state(srslog::basic_levels level) { logger.log(level, "RX entity state. {}", st); }
};

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::rlc_rx_am_sdu_info> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_rx_am_sdu_info& info, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "nof_segments={} has_gap={} fully_received={} sdu_len={}",
                     info.segments.size(),
                     info.has_gap,
                     info.fully_received,
                     info.sdu.length());
  }
};

template <>
struct formatter<srsran::rlc_rx_am_state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_rx_am_state& st, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "rx_next={} rx_next_status_trigger={} rx_highest_status={} rx_next_highest={}",
                     st.rx_next,
                     st.rx_next_status_trigger,
                     st.rx_highest_status,
                     st.rx_next_highest);
  }
};

} // namespace fmt
