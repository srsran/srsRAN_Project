/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/support/async/async_event_source.h"
#include "srsgnb/support/async/event_signal.h"
#include "srsgnb/support/async/protocol_transaction_manager.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_transaction_manager
{
public:
  ngap_transaction_manager(timer_manager& timers) : ng_setup_outcome(timers) {}

  /// NG Setup Response/Failure Event Source.
  protocol_transaction_event_source<asn1::ngap::ng_setup_resp_s, asn1::ngap::ng_setup_fail_s> ng_setup_outcome;
};

} // namespace srs_cu_cp
} // namespace srsran
