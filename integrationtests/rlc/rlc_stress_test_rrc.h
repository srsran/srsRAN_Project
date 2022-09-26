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

#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/ran/bearer_logger.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
class rrc_dummy : public pdcp_rx_upper_control_notifier, pdcp_tx_upper_control_notifier
{
  bearer_logger logger;

public:
  explicit rrc_dummy(uint32_t id) : logger("RRC", id, lcid_t{}) {}

  // PDCP -> RRC
  void on_integrity_failure() final {}
  void on_protocol_failure() final {}
  void on_max_count_reached() final {}
};

} // namespace srsgnb
