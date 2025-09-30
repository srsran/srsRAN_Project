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

#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/support/async/protocol_transaction_manager.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_ue_transaction_manager
{
public:
  ngap_ue_transaction_manager(timer_factory timers) :
    handover_preparation_outcome(timers), handover_cancel_outcome(timers), dl_ran_status_transfer_outcome(timers)
  {
  }

  void cancel_all()
  {
    handover_preparation_outcome.stop();
    handover_cancel_outcome.stop();
  }

  /// Handover Preparation Response/Failure Event Source.
  protocol_transaction_event_source<asn1::ngap::ho_cmd_s, asn1::ngap::ho_prep_fail_s> handover_preparation_outcome;

  /// Handover Cancel Ack Event Source.
  protocol_transaction_event_source<asn1::ngap::ho_cancel_ack_s> handover_cancel_outcome;

  /// DL RAN Status Transfer source.
  protocol_transaction_event_source<asn1::ngap::dl_ran_status_transfer_s> dl_ran_status_transfer_outcome;
};

} // namespace srs_cu_cp
} // namespace srsran
