/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/support/async/event_signal.h"
#include "srsran/support/async/protocol_transaction_manager.h"

namespace srsran {
namespace srs_cu_cp {

class f1ap_ue_transaction_manager
{
public:
  f1ap_ue_transaction_manager(timer_factory timers) :
    context_setup_outcome(timers),
    context_modification_outcome(timers),
    context_release_complete(timers),
    positioning_information_outcome(timers)
  {
  }

  void cancel_all()
  {
    context_setup_outcome.stop();
    context_modification_outcome.stop();
    context_release_complete.stop();
    positioning_information_outcome.stop();
  }

  /// F1AP UE Context Setup Response/Failure Event Source.
  protocol_transaction_event_source<asn1::f1ap::ue_context_setup_resp_s, asn1::f1ap::ue_context_setup_fail_s>
      context_setup_outcome;

  /// F1AP UE Context Modification Response/Failure Event Source.
  protocol_transaction_event_source<asn1::f1ap::ue_context_mod_resp_s, asn1::f1ap::ue_context_mod_fail_s>
      context_modification_outcome;

  /// F1AP UE Context Release Event Source.
  protocol_transaction_event_source<asn1::f1ap::ue_context_release_complete_s> context_release_complete;

  /// F1AP Positioning Information Event Source.
  protocol_transaction_event_source<asn1::f1ap::positioning_info_resp_s, asn1::f1ap::positioning_info_fail_s>
      positioning_information_outcome;
};

} // namespace srs_cu_cp
} // namespace srsran