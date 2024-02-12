/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"
#include "srsran/support/async/event_signal.h"
#include "srsran/support/async/protocol_transaction_manager.h"

namespace srsran {
namespace srs_cu_cp {

class e1ap_bearer_transaction_manager
{
public:
  e1ap_bearer_transaction_manager(timer_factory timers) :
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
} // namespace srsran