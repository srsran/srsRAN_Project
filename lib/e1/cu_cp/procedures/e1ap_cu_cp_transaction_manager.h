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
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/support/async/event_signal.h"
#include "srsgnb/support/async/protocol_transaction_manager.h"

namespace srsgnb {
namespace srs_cu_cp {

using e1ap_outcome     = expected<asn1::e1ap::successful_outcome_s, asn1::e1ap::unsuccessful_outcome_s>;
using e1ap_transaction = protocol_transaction<e1ap_outcome>;

class e1ap_transaction_manager
{
public:
  /// Transaction Response Container, which gets indexed by transaction_id.
  constexpr static size_t                                          MAX_NOF_TRANSACTIONS = 256;
  protocol_transaction_manager<e1ap_outcome, MAX_NOF_TRANSACTIONS> transactions;

  explicit e1ap_transaction_manager(timer_manager& timers) :
    transactions(timers, e1ap_outcome{asn1::e1ap::unsuccessful_outcome_s{}})
  {
  }
};

class e1ap_bearer_transaction_manager
{
public:
  e1ap_bearer_transaction_manager(timer_manager& timers) :
    context_setup_outcome(timers), context_modification_outcome(timers), context_release_complete(timers)
  {
  }

  /// E1AP Bearer Context Setup Response/Failure Event Source.
  protocol_transaction_event_source<asn1::e1ap::bearer_context_setup_resp_s, asn1::e1ap::bearer_context_setup_fail_s>
      context_setup_outcome;

  /// E1AP Bearer Context Modification Response/Failure Event Source.
  protocol_transaction_event_source<asn1::e1ap::bearer_context_mod_resp_s, asn1::e1ap::bearer_context_mod_fail_s>
      context_modification_outcome;

  /// E1AP Bearer Context Release Event Source.
  protocol_transaction_event_source<asn1::e1ap::bearer_context_release_complete_s> context_release_complete;
};

} // namespace srs_cu_cp
} // namespace srsgnb