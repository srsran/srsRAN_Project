/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
