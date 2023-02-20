/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/support/async/event_signal.h"
#include "srsgnb/support/async/protocol_transaction_manager.h"

namespace srsgnb {
namespace srs_cu_cp {

#define RRC_PROC_TIMEOUT (-1)

using rrc_outcome     = expected<asn1::rrc_nr::ul_dcch_msg_s, int8_t>;
using rrc_transaction = protocol_transaction<rrc_outcome>;

/// TODO: Do we need a separate class for each procedure? Note that the outcome's expected only has two choices.
class rrc_ue_event_manager
{
public:
  /// Transaction Response Container, which gets indexed by transaction_id.
  constexpr static size_t MAX_NOF_TRANSACTIONS = 4; // Two bit RRC transaction id
  protocol_transaction_manager<rrc_outcome, MAX_NOF_TRANSACTIONS> transactions;

  explicit rrc_ue_event_manager(timer_manager& timers) : transactions(timers, RRC_PROC_TIMEOUT) {}
};

} // namespace srs_cu_cp
} // namespace srsgnb
