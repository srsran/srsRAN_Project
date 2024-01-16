/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "lib/rlc/rlc_bearer_logger.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_tx.h"

namespace srsran {
class rrc_dummy : public pdcp_rx_upper_control_notifier, public pdcp_tx_upper_control_notifier
{
  rlc_bearer_logger logger;

public:
  explicit rrc_dummy(uint32_t id) : logger("RRC", {0, id, drb_id_t::drb1, "DL/UL"}) {}

  // PDCP -> RRC
  void on_integrity_failure() final {}
  void on_protocol_failure() final {}
  void on_max_count_reached() final {}
};

} // namespace srsran
