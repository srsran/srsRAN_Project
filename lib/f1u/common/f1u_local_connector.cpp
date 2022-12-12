/*
 *
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1u_connector_impl.h"
#include "srsgnb/srslog/srslog.h"

using namespace srsgnb;

void f1u_local_connector::attach_cu_bearer(uint32_t                       dl_teid,
                                           f1u_dl_local_adapter&          cu_tx,
                                           srs_cu_up::f1u_rx_pdu_handler& cu_rx)
{
  logger.info("Connecting CU F1-U bearer. DL-TEID={}", dl_teid);
}

void f1u_local_connector::attach_du_bearer(uint32_t                     dl_teid,
                                           uint32_t                     ul_teid,
                                           srs_du::f1u_tx_pdu_notifier& du_tx,
                                           srs_du::f1u_rx_pdu_handler&  du_rx)
{
  logger.info("Connecting CU F1-U bearer. UL-TEID={}", ul_teid);
}
