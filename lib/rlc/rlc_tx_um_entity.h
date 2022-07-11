/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_RLC_RLC_TX_UM_ENTITY_H
#define SRSGNB_LIB_RLC_RLC_TX_UM_ENTITY_H

#include "rlc_sdu_queue.h"
#include "rlc_tx_entity.h"
#include "rlc_um_pdu.h"
#include "srsgnb/rlc/rlc_metrics.h"

namespace srsgnb {

///
/// \brief Tx state variables
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

  // Tx state variables
  rlc_tx_um_state st;

  // Tx SDU buffers
  rlc_sdu_queue sdu_queue;
  rlc_sdu       sdu;
  uint32_t      next_so = 0; // The segment offset for the next generated PDU

  // Mutexes
  std::mutex mutex;

  const uint32_t mod; // Tx counter modulus

  // Header sizes are computed during configure based on SN length
  const uint32_t head_len_full      = 0;
  const uint32_t head_len_first     = 0;
  const uint32_t head_len_not_first = 0;

public:
  rlc_tx_um_entity(du_ue_index_t                        du_index,
                   lcid_t                               lcid,
                   const rlc_tx_um_config&              config,
                   rlc_tx_upper_layer_control_notifier& upper_cn);

  // Interfaces for higher layers
  void handle_sdu(rlc_sdu sdu) override;

  // Interfaces for lower layers
  byte_buffer_slice_chain pull_pdu(uint32_t nof_bytes) override;
  void                    get_buffer_state(uint32_t& bytes) override;

private:
  bool get_si_and_expected_header_size(uint32_t      so,
                                       uint32_t      sdu_len,
                                       uint32_t      nof_bytes,
                                       rlc_si_field& si,
                                       uint32_t&     head_len) const;

  void debug_state() { logger.log_debug("tx_next={}, next_so={}", st.tx_next, next_so); }
};

} // namespace srsgnb

#endif // SRSGNB_LIB_RLC_RLC_TX_UM_ENTITY_H
