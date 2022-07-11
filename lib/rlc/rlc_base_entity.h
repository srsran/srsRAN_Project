/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_RLC_RLC_BASE_ENTITY_H
#define SRSGNB_LIB_RLC_RLC_BASE_ENTITY_H

#include "rlc_rx_entity.h"
#include "rlc_tx_entity.h"
#include "srsgnb/ran/bearer_logger.h"
#include "srsgnb/rlc/rlc_entity.h"
#include "srsgnb/rlc/rlc_metrics.h"

namespace srsgnb {

/// Class used to store common members to all RLC entites.
/// It will contain the base class for TX and RX entities and getters
/// for them. It will also contain UE index, LCID and logging helpers
///
/// \param du_index UE identifier within the DU
/// \param lcid LCID for the bearer
class rlc_base_entity : public rlc_entity
{
public:
  rlc_base_entity(du_ue_index_t du_index, lcid_t lcid) : logger("RLC", du_index, lcid) {}
  ~rlc_base_entity() override = default;

  rlc_tx_sdu_handler*     get_tx_sdu_handler() final { return tx.get(); };
  rlc_tx_pdu_transmitter* get_tx_pdu_transmitter() final { return tx.get(); };
  rlc_rx_pdu_handler*     get_rx_pdu_handler() final { return rx.get(); };

  rlc_bearer_metrics_container get_metrics()
  {
    rlc_bearer_metrics_container metrics;
    metrics.tx = tx->get_metrics();
    metrics.rx = rx->get_metrics();
    return metrics;
  }

  void reset_metrics()
  {
    tx->reset_metrics();
    rx->reset_metrics();
  }

protected:
  bearer_logger logger;

  std::unique_ptr<rlc_tx_entity> tx = {};
  std::unique_ptr<rlc_rx_entity> rx = {};
};

} // namespace srsgnb

#endif // SRSGNB_LIB_RLC_RLC_BASE_ENTITY_H
