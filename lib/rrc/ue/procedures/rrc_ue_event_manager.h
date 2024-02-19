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

#include "srsran/adt/expected.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg.h"
#include "srsran/support/async/event_signal.h"
#include "srsran/support/async/protocol_transaction_manager.h"

namespace srsran {
namespace srs_cu_cp {

using rrc_outcome     = asn1::rrc_nr::ul_dcch_msg_s;
using rrc_transaction = protocol_transaction<rrc_outcome>;

/// TODO: Do we need a separate class for each procedure? Note that the outcome's expected only has two choices.
class rrc_ue_event_manager
{
public:
  /// Transaction Response Container, which gets indexed by transaction_id.
  constexpr static size_t                   MAX_NOF_TRANSACTIONS = 4; // Two bit RRC transaction id
  protocol_transaction_manager<rrc_outcome> transactions;

  explicit rrc_ue_event_manager(timer_factory timers) : transactions(MAX_NOF_TRANSACTIONS, timers) {}
};

} // namespace srs_cu_cp
} // namespace srsran
