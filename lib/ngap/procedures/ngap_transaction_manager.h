/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/support/async/async_event_source.h"
#include "srsran/support/async/protocol_transaction_manager.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_transaction_manager
{
public:
  ngap_transaction_manager(timer_factory timers) :
    ng_setup_outcome(timers),
    ng_reset_outcome(timers),
    handover_preparation_outcome(timers),
    handover_cancel_outcome(timers)
  {
  }

  /// NG Setup Response/Failure Event Source.
  protocol_transaction_event_source<asn1::ngap::ng_setup_resp_s, asn1::ngap::ng_setup_fail_s> ng_setup_outcome;

  /// NG Reset Acknowledge Event Source.
  protocol_transaction_event_source<asn1::ngap::ng_reset_ack_s> ng_reset_outcome;

  /// Handover Preparation Response/Failure Event Source.
  protocol_transaction_event_source<asn1::ngap::ho_cmd_s, asn1::ngap::ho_prep_fail_s> handover_preparation_outcome;

  /// Handover Cancel Ack Event Source.
  protocol_transaction_event_source<asn1::ngap::ho_cancel_ack_s> handover_cancel_outcome;
};

} // namespace srs_cu_cp
} // namespace srsran
