/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1AP_DU_EVENT_MANAGER_H
#define SRSGNB_F1AP_DU_EVENT_MANAGER_H

#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/support/async/event_signal.h"
#include "srsgnb/support/async/protocol_transaction_manager.h"

namespace srsgnb {
namespace srs_du {

using f1ap_outcome     = expected<asn1::f1ap::successful_outcome_s, asn1::f1ap::unsuccessful_outcome_s>;
using f1ap_transaction = protocol_transaction<f1ap_outcome>;

class f1ap_event_manager
{
public:
  /// Transaction Response Container, which gets indexed by transaction_id.
  constexpr static size_t                                          MAX_NOF_TRANSACTIONS = 256;
  protocol_transaction_manager<f1ap_outcome, MAX_NOF_TRANSACTIONS> transactions;

  /// F1 UE Context Modification Outcome
  using f1ap_ue_context_modification_outcome_t =
      expected<const asn1::f1ap::ue_context_mod_confirm_s*, const asn1::f1ap::ue_context_mod_refuse_s*>;
  event_signal<f1ap_ue_context_modification_outcome_t> f1ap_ue_context_modification_response;

  explicit f1ap_event_manager(timer_manager& timers) :
    transactions(timers, f1ap_outcome{asn1::f1ap::unsuccessful_outcome_s{}})
  {}
};

} // namespace srs_du
} // namespace srsgnb

#endif // SRSGNB_F1AP_DU_EVENT_MANAGER_H
