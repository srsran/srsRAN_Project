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

#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/e2ap/e2ap.h"
#include "srsgnb/support/async/event_signal.h"
#include "srsgnb/support/async/protocol_transaction_manager.h"

namespace srsgnb {

using e2ap_outcome     = expected<asn1::e2ap::successful_outcome_s, asn1::e2ap::unsuccessful_outcome_s>;
using e2ap_transaction = protocol_transaction<e2ap_outcome>;
class e2_event_manager
{
public:
  /// Transaction Response Container, which gets indexed by transaction_id.
  constexpr static size_t                                          MAX_NOF_TRANSACTIONS = 256;
  protocol_transaction_manager<e2ap_outcome, MAX_NOF_TRANSACTIONS> transactions;
  // CU initiated E2 Setup Procedure

  explicit e2_event_manager(timer_manager& timers) :
    transactions(timers, e2ap_outcome{asn1::e2ap::unsuccessful_outcome_s{}})
  {
  }
};
} // namespace srsgnb