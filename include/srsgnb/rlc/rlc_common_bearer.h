/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_COMMON_BEARER_H
#define SRSGNB_RLC_COMMON_BEARER_H

#include "srsgnb/rlc/rlc.h"
#include "srsgnb/rlc/rlc_logger.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

/// Base class used for transmitting RLC bearers.
/// It provides interfaces for the RLC bearers, for both higher layers and lower layers
/// It also stores interfaces for the higher layers, both for the user-plane
/// and the control plane.
class rlc_tx_common_bearer : public rlc_tx_sdu_handler, public rlc_tx_pdu_transmitter
{
protected:
  rlc_tx_common_bearer(du_ue_index_t du_index, lcid_t lcid, rlc_tx_upper_layer_control_notifier& upper_cn) :
    logger(du_index, lcid), upper_cn(upper_cn)
  {}

  rlc_logger                           logger;
  rlc_tx_upper_layer_control_notifier& upper_cn;
};

/// Base class used for receiving RLC bearers.
/// It provides interfaces for the RLC bearers, for the lower layers
class rlc_rx_common_bearer : public rlc_rx_pdu_handler
{
protected:
  rlc_rx_common_bearer(du_ue_index_t du_index, lcid_t lcid, rlc_rx_upper_layer_data_notifier& upper_dn) :
    logger(du_index, lcid), upper_dn(upper_dn)
  {}

  rlc_logger                        logger;
  rlc_rx_upper_layer_data_notifier& upper_dn;
};

/// Class used to store common parameters for all RLC bearer types.
/// It will contain the base class for TX and RX entities and getters
/// for them.
///
/// \param du_index UE identifier within the DU
/// \param lcid LCID for the bearer
class rlc_common_bearer
{
public:
  rlc_common_bearer(du_ue_index_t du_index, lcid_t lcid) : logger(du_index, lcid) {}

  rlc_tx_sdu_handler*     get_tx_sdu_handler() { return tx.get(); };
  rlc_tx_pdu_transmitter* get_tx_pdu_transmitter() { return tx.get(); };
  rlc_rx_pdu_handler*     get_rx_pdu_handler() { return rx.get(); };

protected:
  rlc_logger logger;

  std::unique_ptr<rlc_tx_common_bearer> tx = {};
  std::unique_ptr<rlc_rx_common_bearer> rx = {};
};

} // namespace srsgnb
#endif

