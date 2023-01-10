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
#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/support/async/async_event_source.h"
#include "srsgnb/support/async/event_signal.h"

namespace srsgnb {
namespace srs_cu_cp {

class f1ap_ue_transaction_manager
{
public:
  f1ap_ue_transaction_manager(timer_manager& timers) :
    context_modification_outcome(timers, asn1::f1ap::ue_context_mod_fail_s{})
  {
  }

  /// F1 Context Release Complete
  using f1_ue_context_release_outcome_t = const asn1::f1ap::ue_context_release_complete_s*;
  event_signal<f1_ue_context_release_outcome_t> f1ap_ue_context_release_complete;

  /// F1 UE context setup procedure outcome.
  using f1_ue_context_setup_outcome_t =
      expected<const asn1::f1ap::ue_context_setup_resp_s*, const asn1::f1ap::ue_context_setup_fail_s*>;
  event_signal<f1_ue_context_setup_outcome_t> f1ap_ue_context_setup_outcome;

  /// F1 UE Context Modification procedure outcome.
  using context_modification_outcome_t = expected<asn1::f1ap::ue_context_mod_resp_s, asn1::f1ap::ue_context_mod_fail_s>;
  async_event_source<context_modification_outcome_t> context_modification_outcome;
};

} // namespace srs_cu_cp
} // namespace srsgnb