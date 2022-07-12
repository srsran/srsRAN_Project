/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_RX_UM_BEARER_H
#define SRSGNB_RLC_RX_UM_BEARER_H

#include "rlc_rx_entity.h"
#include "rlc_sdu_queue.h"
#include "rlc_um_pdu.h"
#include "srsgnb/rlc/rlc_metrics.h"
#include "srsgnb/support/timers.h"
#include <map>

namespace srsgnb {

class rlc_rx_um_entity : public rlc_rx_entity
{
private:
  // Config storage
  const rlc_rx_um_config cfg;

  // Mutexes
  std::mutex mutex;

  uint32_t RX_Next_Reassembly = 0; // the earliest SN that is still considered for reassembly
  uint32_t RX_Timer_Trigger   = 0; // the SN following the SN which triggered t-Reassembly
  uint32_t RX_Next_Highest    = 0; // the SN following the SN of the UMD PDU with the highest SN among
                                   // received UMD PDUs. It serves as the higher edge of the reassembly window.

  const uint32_t mod; // Rx counter modulus
  const uint32_t UM_Window_Size;

  // Rx window
  struct rlc_umd_pdu_segments {
    std::map<uint32_t, rlc_um_pdu> segments; // Map of segments with SO as key
    byte_buffer                    sdu;
    uint32_t                       next_expected_so;
    uint32_t                       total_sdu_length;
  };
  std::map<uint32_t, rlc_umd_pdu_segments> rx_window;

  void update_total_sdu_length(rlc_umd_pdu_segments& pdu_segments, const rlc_um_pdu& rx_pdu);

  // TS 38.322 Sec. 7.3
  unique_timer reassembly_timer; // to detect loss of RLC PDUs at lower layers

  bool sn_in_reassembly_window(const uint32_t sn);
  bool sn_invalid_for_rx_buffer(const uint32_t sn);
  void handle_rx_buffer_update(const uint32_t sn);
  bool has_missing_byte_segment(const uint32_t sn);

  constexpr uint32_t RX_MOD_NR_BASE(uint32_t x) { return (x - RX_Next_Highest - UM_Window_Size) % mod; }

  void debug_state()
  {
    logger.log_debug("RX_Next_Reassembly={}, RX_Timer_Trigger={}, RX_Next_Highest={}, t_Reassembly={}",
                     RX_Next_Reassembly,
                     RX_Timer_Trigger,
                     RX_Next_Highest,
                     reassembly_timer.is_running() ? "running" : "stopped");
  }

public:
  rlc_rx_um_entity(du_ue_index_t                     du_index,
                   lcid_t                            lcid,
                   const rlc_rx_um_config&           config,
                   rlc_rx_upper_layer_data_notifier& upper_dn,
                   timer_manager&                    timers);

  void timer_expired(uint32_t timeout_id);

  void handle_pdu(shared_byte_buffer_view buf) override;
};

} // namespace srsgnb

#endif
