/*
 *
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/f1u/cu_up/f1u_bearer_factory.h"
#include "f1u_bearer_impl.h"

using namespace srsgnb;
using namespace srs_cu_up;

std::unique_ptr<f1u_bearer> srsgnb::srs_cu_up::create_f1u_bearer(drb_id_t                  drb_id,
                                                                 f1u_tx_pdu_notifier&      tx_pdu_notifier,
                                                                 f1u_rx_delivery_notifier& rx_delivery_notifier,
                                                                 f1u_rx_sdu_notifier&      rx_sdu_notifier)
{
  auto bearer = std::make_unique<f1u_bearer_impl>(drb_id, tx_pdu_notifier, rx_delivery_notifier, rx_sdu_notifier);
  return bearer;
}
