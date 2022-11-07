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
#include "srsgnb/asn1/e1ap.h"
#include "srsgnb/support/async/event_signal.h"
#include "srsgnb/support/async/protocol_transaction_manager.h"

namespace srsgnb {
namespace srs_cu_cp {

using e1ap_outcome     = expected<asn1::e1ap::successful_outcome_s, asn1::e1ap::unsuccessful_outcome_s>;
using e1ap_transaction = protocol_transaction<e1ap_outcome>;

class e1_event_manager
{
public:
  /// Transaction Response Container, which gets indexed by transaction_id.
  constexpr static size_t                                          MAX_NOF_TRANSACTIONS = 256;
  protocol_transaction_manager<e1ap_outcome, MAX_NOF_TRANSACTIONS> transactions;

  // /// CU-CP initiated E1 Setup Procedure
  // using cu_cp_e1_setup_procedure_outcome_t =
  //     expected<const asn1::e1ap::gnb_cu_cp_e1_setup_resp_s*, const asn1::e1ap::gnb_cu_cp_e1_setup_fail_s*>;
  // event_signal<cu_cp_e1_setup_procedure_outcome_t> cu_cp_e1_setup_response_message;

  /// E1 Context Release Complete
  using e1_bearer_context_release_outcome_t = const asn1::e1ap::bearer_context_release_complete_s*;
  event_signal<e1_bearer_context_release_outcome_t> e1ap_bearer_context_release_complete;

  /// E1 Bearer Context Setup procedure outcome.
  using e1_bearer_context_setup_outcome_t =
      expected<const asn1::e1ap::bearer_context_setup_resp_s*, const asn1::e1ap::bearer_context_setup_fail_s*>;
  event_signal<e1_bearer_context_setup_outcome_t> e1ap_bearer_context_setup_response_message;

  /// E1 Bearer Context Modification procedure outcome.
  using e1_bearer_context_modification_outcome_t =
      expected<const asn1::e1ap::bearer_context_mod_resp_s*, const asn1::e1ap::bearer_context_mod_fail_s*>;
  event_signal<e1_bearer_context_modification_outcome_t> e1ap_bearer_context_modification_response_message;

  explicit e1_event_manager(timer_manager& timers) :
    transactions(timers, e1ap_outcome{asn1::e1ap::unsuccessful_outcome_s{}})
  {
  }
};

} // namespace srs_cu_cp
} // namespace srsgnb