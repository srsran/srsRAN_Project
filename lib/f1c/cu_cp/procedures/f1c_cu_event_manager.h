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
#include "srsgnb/support/async/event_signal.h"

namespace srsgnb {
namespace srs_cu_cp {

class f1c_event_manager
{
public:
  /// F1 Context Release Complete
  using f1_ue_context_release_outcome_t = const asn1::f1ap::ue_context_release_complete_s*;
  event_signal<f1_ue_context_release_outcome_t> f1ap_ue_context_release_complete;

  /// F1 UE context setup procedure outcome.
  using f1_ue_context_setup_outcome_t =
      expected<const asn1::f1ap::ue_context_setup_resp_s*, const asn1::f1ap::ue_context_setup_fail_s*>;
  event_signal<f1_ue_context_setup_outcome_t> f1ap_ue_context_setup_response_message;

  /// F1 UE Context Modification procedure outcome.
  using f1_ue_context_modification_outcome_t =
      expected<const asn1::f1ap::ue_context_mod_resp_s*, const asn1::f1ap::ue_context_mod_fail_s*>;
  event_signal<f1_ue_context_modification_outcome_t> f1ap_ue_context_modification_response_message;
};

} // namespace srs_cu_cp
} // namespace srsgnb