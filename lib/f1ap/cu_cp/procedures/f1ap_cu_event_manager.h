/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
namespace srs_cu_cp {

/// Response type of a F1AP CU transaction.
using f1ap_transaction_response = expected<asn1::f1ap::successful_outcome_s, asn1::f1ap::unsuccessful_outcome_s>;

/// F1AP protocol transaction type.
using f1ap_transaction = protocol_transaction<f1ap_transaction_response>;

class f1ap_event_manager
{
  /// Transaction Response Container, which gets indexed by transaction_id.
  static constexpr size_t MAX_NOF_TRANSACTIONS = 256;

public:
  protocol_transaction_manager<f1ap_transaction_response> transactions;

  /// F1 TRP information exchange outcome.
  using f1ap_trp_information_outcome_t =
      expected<const asn1::f1ap::trp_info_resp_s*, const asn1::f1ap::trp_info_fail_s*>;
  event_signal<f1ap_trp_information_outcome_t> f1ap_trp_information_outcome;

  /// F1 positioning measurement outcome.
  using f1ap_positioning_measurement_outcome_t =
      expected<const asn1::f1ap::positioning_meas_resp_s*, const asn1::f1ap::positioning_meas_fail_s*>;
  event_signal<f1ap_positioning_measurement_outcome_t> f1ap_positioning_measurement_outcome;

  /// F1 GNB CU configuration update outcome.
  using f1ap_gnb_cu_configuration_update_outcome_t =
      expected<const asn1::f1ap::gnb_cu_cfg_upd_ack_s*, const asn1::f1ap::gnb_cu_cfg_upd_fail_s*>;
  event_signal<f1ap_gnb_cu_configuration_update_outcome_t> f1ap_gnb_cu_configuration_update_outcome;

  explicit f1ap_event_manager(timer_factory timer_service) : transactions(MAX_NOF_TRANSACTIONS, timer_service) {}
};

} // namespace srs_cu_cp
} // namespace srsran
