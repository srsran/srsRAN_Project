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
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/support/async/event_signal.h"
#include "srsran/support/async/protocol_transaction_manager.h"

namespace srsran {
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
  event_signal<f1ap_ue_context_modification_outcome_t> f1ap_ue_context_modification_outcome;

  explicit f1ap_event_manager(timer_factory timer_service) :
    transactions(timer_service, f1ap_outcome{asn1::f1ap::unsuccessful_outcome_s{}})
  {
  }
};

} // namespace srs_du
} // namespace srsran
