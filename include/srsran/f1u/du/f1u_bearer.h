/*
 *
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1u/du/f1u_rx_pdu_handler.h"
#include "srsran/f1u/du/f1u_tx_delivery_handler.h"
#include "srsran/f1u/du/f1u_tx_sdu_handler.h"
#include "srsran/ran/up_transport_layer_info.h"

namespace srsran {
namespace srs_du {

class f1u_bearer
{
public:
  virtual ~f1u_bearer() = default;

  virtual f1u_rx_pdu_handler&      get_rx_pdu_handler()      = 0;
  virtual f1u_tx_delivery_handler& get_tx_delivery_handler() = 0;
  virtual f1u_tx_sdu_handler&      get_tx_sdu_handler()      = 0;

  virtual void stop() = 0;
};

/// This class represents the interface through which a F1-U bearer disconnects from its gateway upon removal
class f1u_bearer_disconnector
{
public:
  virtual ~f1u_bearer_disconnector() = default;

  virtual void remove_du_bearer(const up_transport_layer_info& dl_up_tnl_info) = 0;
};

} // namespace srs_du
} // namespace srsran
