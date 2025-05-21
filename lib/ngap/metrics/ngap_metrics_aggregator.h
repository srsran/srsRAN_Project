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

#include "srsran/ngap/ngap_metrics.h"
#include "srsran/ran/s_nssai.h"
#include <cmath>

namespace srsran::srs_cu_cp {

class ngap_metrics_aggregator
{
public:
  void aggregate_requested_pdu_session(s_nssai_t s_nssai);

  void aggregate_successful_pdu_session_setup(s_nssai_t s_nssai);

  void aggregate_failed_pdu_session_setup(s_nssai_t s_nssai, ngap_cause_t cause);

  void aggregate_cn_initiated_paging_request();

  ngap_metrics request_metrics_report() const;

private:
  ngap_metrics aggregated_ngap_metrics;
};

} // namespace srsran::srs_cu_cp
