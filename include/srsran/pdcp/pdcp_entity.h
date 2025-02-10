/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_rx_metrics.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/pdcp/pdcp_tx_metrics.h"
#include "srsran/ran/rb_id.h"
#include "srsran/support/async/manual_event.h"
#include "srsran/support/timers.h"

namespace srsran {

struct pdcp_metrics_container {
  uint32_t                  ue_index;
  rb_id_t                   rb_id;
  pdcp_tx_metrics_container tx;
  pdcp_rx_metrics_container rx;
  timer_duration            metrics_period;
};

/// Interface for the PDCP bearer.
/// Provides getters and setters for the RX and TX parts of the PDCP entity.
class pdcp_entity
{
public:
  pdcp_entity()          = default;
  virtual ~pdcp_entity() = default;

  virtual pdcp_tx_lower_interface&         get_tx_lower_interface()         = 0;
  virtual pdcp_tx_upper_control_interface& get_tx_upper_control_interface() = 0;
  virtual pdcp_tx_upper_data_interface&    get_tx_upper_data_interface()    = 0;
  virtual pdcp_rx_upper_control_interface& get_rx_upper_control_interface() = 0;
  virtual pdcp_rx_lower_interface&         get_rx_lower_interface()         = 0;
  virtual void                             stop()                           = 0;
  virtual manual_event_flag&               rx_crypto_awaitable()            = 0;
};

} // namespace srsran
