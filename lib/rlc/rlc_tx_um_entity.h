/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rlc_sdu_queue.h"
#include "rlc_tx_entity.h"
#include "srsgnb/support/executors/task_executor.h"
#include "fmt/format.h"

namespace srsgnb {

///
/// \brief TX state variables
/// Ref: 3GPP TS 38.322 version 16.2.0 Section 7.1
///
struct rlc_tx_um_state {
  ///
  /// \brief  TX_Next – UM send state variable
  /// It holds the value of the SN to be assigned for the next newly generated UMD PDU with
  /// segment. It is initially set to 0, and is updated after the UM RLC entity submits a UMD PDU
  /// including the last segment of an RLC SDU to lower layers.
  ///
  uint32_t tx_next = 0;
};

class rlc_tx_um_entity : public rlc_tx_entity
{
private:
  // Config storage
  const rlc_tx_um_config cfg;

  // TX state variables
  rlc_tx_um_state st;

  // TX SDU buffers
  rlc_sdu_queue sdu_queue;
  rlc_sdu       sdu;
  uint32_t      next_so = 0; // The segment offset for the next generated PDU

  // Mutexes
  std::mutex mutex;

  /// TX counter modulus
  const uint32_t mod;

  // Header sizes are computed upon construction based on SN length
  const uint32_t head_len_full;
  const uint32_t head_len_first;
  const uint32_t head_len_not_first;

public:
  rlc_tx_um_entity(du_ue_index_t                        du_index,
                   rb_id_t                              rb_id,
                   const rlc_tx_um_config&              config,
                   rlc_tx_upper_layer_data_notifier&    upper_dn_,
                   rlc_tx_upper_layer_control_notifier& upper_cn_,
                   rlc_tx_lower_layer_notifier&         lower_dn_);

  // Interfaces for higher layers
  void handle_sdu(rlc_sdu sdu_) override;
  void discard_sdu(uint32_t pdcp_sn) override;

  // Interfaces for lower layers
  byte_buffer_slice_chain pull_pdu(uint32_t grant_len) override;
  uint32_t                get_buffer_state() override;

private:
  bool get_si_and_expected_header_size(uint32_t      so,
                                       uint32_t      sdu_len,
                                       uint32_t      grant_len,
                                       rlc_si_field& si,
                                       uint32_t&     head_len) const;

  /// Called when buffer state needs to be updated and forwarded to lower layers.
  void handle_buffer_state_update();
  /// Called when buffer state needs to be updated and forwarded to lower layers while already holding a lock.
  void handle_buffer_state_update_nolock();

  uint32_t get_buffer_state_nolock();

  void log_state(srslog::basic_levels level) { logger.log(level, "TX entity state. {} next_so={}", st, next_so); }
};

} // namespace srsgnb

namespace fmt {
template <>
struct formatter<srsgnb::rlc_tx_um_state> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::rlc_tx_um_state& st, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "tx_next={}", st.tx_next);
  }
};

} // namespace fmt
