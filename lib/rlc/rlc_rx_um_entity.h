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

#include "rlc_rx_entity.h"
#include "rlc_sdu_window.h"
#include "rlc_um_pdu.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include "fmt/format.h"
#include <set>

namespace srsran {

/// UM SDU segment container
struct rlc_rx_um_sdu_segment {
  rlc_si_field      si;      ///< Segmentation info
  uint16_t          so;      ///< Segment offset (SO)
  byte_buffer_slice payload; ///< Payload (SDU segment)
};

/// UM SDU segment compare object
struct rlc_rx_um_sdu_segment_cmp {
  bool operator()(const rlc_rx_um_sdu_segment& a, const rlc_rx_um_sdu_segment& b) const { return a.so < b.so; }
};

/// Container to collect received SDU segments and to assemble the SDU upon completion
struct rlc_rx_um_sdu_info {
  // TODO: Refactor this struct.
  // Move the following rlc_rx_um methods here:
  // - add segments without duplicates
  // - assemble SDU
  bool                                                       fully_received = false;
  bool                                                       has_gap        = false;
  std::set<rlc_rx_um_sdu_segment, rlc_rx_um_sdu_segment_cmp> segments; // Set of segments with SO as key
  byte_buffer_chain                                          sdu = {};
};

/// \brief Rx state variables
/// Ref: 3GPP TS 38.322 version 16.2.0 Section 7.1
struct rlc_rx_um_state {
  /// \brief RX_Next_Reassembly – UM receive state variable
  /// The earliest SN that is still considered for reassembly
  uint32_t rx_next_reassembly = 0;

  /// \brief RX_Timer_Trigger - UM t-Reassembly state variable
  /// The SN following the SN which triggered t-Reassembly
  uint32_t rx_timer_trigger = 0;

  /// \brief RX_Next_Highest - UM receive state variable
  /// The SN following the SN of the UMD PDU with the highest SN among
  /// received UMD PDUs. It serves as the higher edge of the reassembly window.
  uint32_t rx_next_highest = 0;
};

class rlc_rx_um_entity : public rlc_rx_entity
{
private:
  /// Config storage
  const rlc_rx_um_config cfg;

  /// Rx state variables
  rlc_rx_um_state st;

  /// Rx counter modulus
  const uint32_t mod;
  /// UM window size
  const uint32_t um_window_size;

  /// Rx window
  std::unique_ptr<rlc_sdu_window_base<rlc_rx_um_sdu_info>> rx_window;

  /// \brief t-Reassembly
  /// This timer is used by [...] the receiving side of an UM RLC entity in order to detect loss of RLC PDUs at lower
  /// layer (see sub clauses 5.2.2.2 and 5.2.3.2). If t-Reassembly is running, t-Reassembly shall not be started
  /// additionally, i.e.only one t-Reassembly per RLC entity is running at a given time.
  /// Ref: TS 38.322 Sec. 7.3
  unique_timer reassembly_timer; // to detect loss of RLC PDUs at lower layers

  bool sn_in_reassembly_window(const uint32_t sn);
  bool sn_invalid_for_rx_buffer(const uint32_t sn);

  constexpr uint32_t rx_mod_base(uint32_t x) { return (x - st.rx_next_highest - um_window_size) % mod; }

  void log_state(srslog::basic_levels level)
  {
    logger.log(
        level, "RX entity state. {} t_reassembly={}", st, reassembly_timer.is_running() ? "running" : "notify_stop");
  }

public:
  rlc_rx_um_entity(du_ue_index_t                     du_index,
                   rb_id_t                           rb_id,
                   const rlc_rx_um_config&           config,
                   rlc_rx_upper_layer_data_notifier& upper_dn_,
                   timer_factory                     timers,
                   task_executor&                    ue_executor);

  void on_expired_reassembly_timer();

  void handle_pdu(byte_buffer_slice buf) override;

private:
  /// Handles a received data PDU which contains an SDU segment, puts it into the receive window if required,
  /// reassembles the SDU if possible and forwards it to upper layer.
  ///
  /// \param header The header of the PDU, used for sanity check and tracking of the segment offset
  /// \param payload The PDU payload, i.e. the SDU segment
  /// \return True if segment was added and repository was changed. False otherwise (i.e. new segment was dropped).
  bool handle_segment_data_sdu(const rlc_um_pdu_header& header, byte_buffer_slice payload);

  /// Stores a newly received SDU segment and avoids overlapping bytes.
  /// Overlaps are prevented by either trimming the new or previously received segment; or dropping the new segment
  /// entirely if all of its bytes are already present.
  ///
  /// \param sdu_info Container/Info object of the associated SDU
  /// \param new_segment The newly received SDU segment
  /// \return True if segment was added and repository was changed. False otherwise (i.e. new segment was dropped).
  bool store_segment(rlc_rx_um_sdu_info& sdu_info, rlc_rx_um_sdu_segment new_segment);

  /// Iterates the received SDU segments to check whether it is fully received and whether the received segments are
  /// contiguous or have gaps.
  ///
  /// \param rx_sdu Container/Info object to be inspected
  void update_segment_inventory(rlc_rx_um_sdu_info& rx_sdu) const;

  /// Creates the rx_window according to sn_size
  /// \param sn_size Size of the sequence number (SN)
  /// \return unique pointer to rx_window instance
  std::unique_ptr<rlc_sdu_window_base<rlc_rx_um_sdu_info>> create_rx_window(rlc_um_sn_size sn_size);
};

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::rlc_rx_um_sdu_info> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_rx_um_sdu_info& info, FormatContext& ctx)
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
struct formatter<srsran::rlc_rx_um_state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_rx_um_state& st, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "rx_next_reassembly={} rx_timer_trigger={} rx_next_highest={}",
                     st.rx_next_reassembly,
                     st.rx_timer_trigger,
                     st.rx_next_highest);
  }
};

} // namespace fmt
