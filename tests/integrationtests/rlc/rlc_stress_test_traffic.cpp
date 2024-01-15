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

#include "rlc_stress_test_traffic.h"

using namespace srsran;

void stress_traffic_sink::on_new_sdu(byte_buffer pdu)
{
  logger.log_info(pdu.begin(), pdu.end(), "Received PDU ({} B)", pdu.length());
}

void stress_traffic_source::send_pdu()
{
  byte_buffer sdu;

  // random or fixed SDU size
  if (args.sdu_size < 1) {
    sdu_size = int_dist(rgen);
  } else {
    sdu_size = args.sdu_size;
  }

  for (uint32_t i = 0; i < sdu_size; i++) {
    report_fatal_error_if_not(sdu.append(payload), "Failed to allocate SDU");
    payload++;
  }

  logger.log_info(sdu.begin(), sdu.end(), "Sending SDU ({} B)", sdu.length());
  pdcp_tx_upper->handle_sdu(std::move(sdu));
}
