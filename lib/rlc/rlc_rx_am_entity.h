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
#include "rlc_pdu_window.h"
#include "rlc_rx_entity.h"
#include "srsgnb/support/timers.h"
#include <set>

namespace srsgnb {

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
  /// Indicates whether a status report is required
  std::atomic<bool> do_status = {false};

  /// Rx counter modulus
  const uint32_t mod;
  /// AM window size
  const uint32_t am_window_size;

  //
  // TODO Refactor this struct
  //
  struct sdu_info {
    bool                                                 fully_received = false;
    bool                                                 has_gap        = false;
    std::set<rlc_am_sdu_segment, rlc_am_sdu_segment_cmp> segments; // Map of segments with SO as key
    byte_buffer                                          sdu              = {};
    uint32_t                                             next_expected_so = 0;
    uint32_t                                             total_sdu_length = 0;
  };
  /// Rx window
  std::unique_ptr<rlc_pdu_window_base<sdu_info>> rx_window;
  /// Indicates the rx_window has not been changed, i.e. no need to rebuild status report.
  static const bool rx_window_not_changed = false;
  /// Indicates the rx_window has been changed, i.e. need to rebuild status report.
  static const bool rx_window_changed = true;

  /// Cached status report
  rlc_am_status_pdu status_report = {cfg.sn_field_length};
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

public:
  rlc_rx_am_entity(du_ue_index_t                     du_index,
                   lcid_t                            lcid,
                   const rlc_rx_am_config&           config,
                   rlc_rx_upper_layer_data_notifier& upper_dn,
                   timer_manager&                    timers);

  // Rx/Tx interconnect
  void set_status_handler(rlc_tx_am_status_handler* status_handler_) { status_handler = status_handler_; }

  // Interfaces for lower layers
  void handle_pdu(byte_buffer_slice buf) override;

  // Status provider for RX entity
  rlc_am_status_pdu get_status_pdu() override;
  uint32_t          get_status_pdu_length() override;
  bool              status_report_required() override;

private:
  /// Handles a received control PDU. The PDU is unpacked and forwarded to the RX entity
  /// \param buf The control PDU to be handled (including header and payload)
  void handle_control_pdu(byte_buffer_slice buf);

  /// Handles a received data PDU which contains an SDU or SDU segment, puts it into the receive window if required,
  /// reassembles the SDU if possible and forwards it to upper layer.
  ///
  /// \param buf The data PDU to be handled (including header and payload)
  /// \return True if the rx_window changed and requires the cached status PDU to be rebuilt, false otherwise
  bool handle_data_pdu(byte_buffer_slice buf);

  /// Handles a received data PDU which contains an SDU, puts it into the receive window and forwards it to upper layer.
  ///
  /// \param header The header of the PDU, used for sanity check
  /// \param payload The PDU payload, i.e. the SDU
  /// \return True if the rx_window changed and requires the cached status PDU to be rebuilt, false on error
  bool handle_full_data_sdu(const rlc_am_pdu_header& header, byte_buffer_slice& payload);

  /// Handles a received data PDU which contains an SDU segment, puts it into the receive window if required,
  /// reassembles the SDU if possible and forwards it to upper layer.
  ///
  /// \param header The header of the PDU, used for sanity check and tracking of the segment offset
  /// \param payload The PDU payload, i.e. the SDU segment
  /// \return True if the rx_window changed and requires the cached status PDU to be rebuilt, false otherwise
  bool handle_segment_data_sdu(const rlc_am_pdu_header& header, byte_buffer_slice& payload);

  void update_segment_inventory(sdu_info& rx_sdu) const;

  /// Rebuilds the cached status_report according to missing SDUs and SDU segments in rx_window
  /// and resets the rx_window_changed flag
  void refresh_status_report();

  void on_expired_status_prohibit_timer(uint32_t timeout_id);
  void on_expired_reassembly_timer(uint32_t timeout_id);

  inline uint32_t rx_mod_base(uint32_t sn) const { return (sn - st.rx_next) % mod; }
  inline bool     inside_rx_window(uint32_t sn) const
  {
    // RX_Next <= SN < RX_Next + AM_Window_Size
    return rx_mod_base(sn) < am_window_size;
  }

  /// \brief This function is used to check if the Rx_Highest_Status is valid when t-Reasseambly expires.
  ///
  /// ACK_SN may be equal to RX_NEXT + AM_Window_Size if the PDU with SN=RX_NEXT+AM_Window_Size has been received by the
  /// RX. An ACK_SN == Rx_Next should not update Rx_Highest_Status,it should be updated when Rx_Next is updated.
  /// \param sn
  /// \return
  inline bool valid_ack_sn(uint32_t sn) const
  {
    // RX_Next < SN <= RX_Next + AM_Window_Size
    return (0 < rx_mod_base(sn)) && (rx_mod_base(sn) <= am_window_size);
  }

  /// Creates the rx_window according to sn_size
  /// \param sn_size Size of the sequence number (SN)
  /// \return unique pointer to rx_window instance
  static std::unique_ptr<rlc_pdu_window_base<sdu_info>> create_rx_window(rlc_am_sn_size sn_size);
};

} // namespace srsgnb

namespace fmt {
template <>
struct formatter<srsgnb::rlc_rx_am_state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::rlc_rx_am_state& st, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "rx_next={}, rx_next_status_trigger={}, rx_highest_status={}, rx_next_highest={}",
                     st.rx_next,
                     st.rx_next_status_trigger,
                     st.rx_highest_status,
                     st.rx_next_highest);
  }
};

} // namespace fmt
