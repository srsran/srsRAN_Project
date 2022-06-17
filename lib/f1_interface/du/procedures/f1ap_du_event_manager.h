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

namespace srsgnb {
namespace srs_du {

class f1ap_event_manager
{
public:
  /// F1 Setup Procedure Outcome.
  using f1ap_setup_outcome_t = expected<const asn1::f1ap::f1_setup_resp_s*, const asn1::f1ap::f1_setup_fail_s*>;
  event_signal<f1ap_setup_outcome_t> f1ap_setup_response;

  /// F1 UE Context Modification Outcome
  using f1ap_ue_context_modification_outcome_t =
      expected<const asn1::f1ap::ue_context_mod_confirm_s*, const asn1::f1ap::ue_context_mod_refuse_s*>;
  event_signal<f1ap_ue_context_modification_outcome_t> f1ap_ue_context_modification_response;
};

} // namespace srs_du
} // namespace srsgnb

#endif // SRSGNB_F1AP_DU_EVENT_MANAGER_H
