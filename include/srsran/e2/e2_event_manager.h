/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/support/async/event_signal.h"
#include "srsran/support/async/protocol_transaction_manager.h"
#include <map>

namespace srsran {

using e2ap_outcome     = expected<asn1::e2ap::successful_outcome_s, asn1::e2ap::unsuccessful_outcome_s>;
using e2ap_transaction = protocol_transaction<e2ap_outcome>;

class e2_event_manager
{
public:
  /// Transaction Response Container, which gets indexed by transaction_id.
  constexpr static size_t                    MAX_NOF_TRANSACTIONS = 256;
  protocol_transaction_manager<e2ap_outcome> transactions;

  std::map<int, std::unique_ptr<protocol_transaction_event_source<asn1::e2ap::ricsubscription_delete_request_s>>>
      sub_del_reqs;

  void add_sub_del_req(int ric_instance_id, timer_factory timer)
  {
    sub_del_reqs[ric_instance_id] =
        std::make_unique<protocol_transaction_event_source<asn1::e2ap::ricsubscription_delete_request_s>>(timer);
  }
  explicit e2_event_manager(timer_factory timers) : transactions(MAX_NOF_TRANSACTIONS, timers) {}
};

} // namespace srsran