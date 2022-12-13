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
  f1u_dl_cu_bearer dl_bearer(cu_tx, cu_rx);
  dl_map.insert({dl_teid, dl_bearer});
}

void f1u_local_connector::attach_du_bearer(uint32_t                    dl_teid,
                                           uint32_t                    ul_teid,
                                           f1u_ul_local_adapter&       du_tx,
                                           srs_du::f1u_rx_pdu_handler& du_rx)
{
  logger.info("Connecting CU F1-U bearer. DL-TEID={}, UL-TEID={}", dl_teid, ul_teid);
  if (dl_map.find(dl_teid) == dl_map.end()) {
    logger.info("Could not find DL-TEID. DL-TEID={}, UL-TEID", dl_teid, ul_teid);
    return;
  }
  auto& dl_tun = dl_map.at(dl_teid);
  dl_tun.cu_tx.attach_du_handler(du_rx);

  du_tx.attach_cu_handler(dl_tun.cu_rx);

  f1u_ul_du_bearer ul_bearer(du_tx, du_rx);
  ul_map.insert({ul_teid, ul_bearer});
}
