/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_stress_test_pdcp.h"

using namespace srsgnb;

void pdcp_traffic_sink::on_new_sdu(byte_buffer_slice_chain pdu)
{
  logger.log_info(pdu.begin(), pdu.end(), "Received PDU ({} B)", pdu.length());
}

void pdcp_traffic_source::send_pdu()
{
  rlc_sdu sdu;
  sdu.pdcp_count = pdcp_count++;

  // random or fixed SDU size
  if (args.sdu_size < 1) {
    sdu_size = int_dist(rgen);
  } else {
    sdu_size = args.sdu_size;
  }

  for (uint32_t i = 0; i < sdu_size; i++) {
    sdu.buf.append(payload);
    payload++;
  }

  logger.log_info(sdu.buf.begin(), sdu.buf.end(), "Sending SDU ({} B, COUNT={})", sdu.buf.length(), sdu.pdcp_count);
  rlc_tx_upper->handle_sdu(sdu);
}
