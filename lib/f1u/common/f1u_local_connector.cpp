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
#include "srsgnb/f1u/cu_up/f1u_bearer_factory.h"
#include "srsgnb/f1u/du/f1u_bearer_factory.h"
#include "srsgnb/srslog/srslog.h"

using namespace srsgnb;

srs_cu_up::f1u_bearer* f1u_local_connector::create_cu_dl_bearer(uint32_t                        dl_teid,
                                                                srs_cu_up::f1u_rx_sdu_notifier& rx_sdu_notifier)
{
  logger.info("Creating CU F1-U bearer. DL-TEID={}", dl_teid);
  f1u_tx_delivery_local_adapter          tx_delivery = {};
  f1u_dl_local_adapter                   cu_tx       = {};
  std::unique_ptr<srs_cu_up::f1u_bearer> f1u_bearer =
      srs_cu_up::create_f1u_bearer(drb_id_t{}, cu_tx, tx_delivery, rx_sdu_notifier);
  srs_cu_up::f1u_bearer* ptr = f1u_bearer.get();
  f1u_dl_cu_bearer       dl_bearer(cu_tx, std::move(f1u_bearer));
  dl_map.insert({dl_teid, std::move(dl_bearer)});
  return ptr;
}

void f1u_local_connector::attach_cu_ul_bearer(uint32_t dl_teid, uint32_t ul_teid)
{
  logger.info("Connecting CU F1-U bearer. DL-TEID={}", dl_teid);
  if (dl_map.find(dl_teid) == dl_map.end()) {
    logger.info("Could not find DL-TEID. DL-TEID={}, UL-TEID={}", dl_teid, ul_teid);
    return;
  }
  if (ul_map.find(ul_teid) == ul_map.end()) {
    logger.info("Could not find UL-TEID. UL-TEID={}, UL-TEID={}", dl_teid, ul_teid);
    return;
  }
  auto& dl_tun = dl_map.at(dl_teid);
  auto& ul_tun = ul_map.at(ul_teid);
  ul_tun.du_tx.attach_cu_handler(dl_tun.f1u_bearer->get_rx_pdu_handler());
}

void f1u_local_connector::create_du_ul_bearer(uint32_t                     dl_teid,
                                              uint32_t                     ul_teid,
                                              srs_du::f1u_rx_sdu_notifier& du_rx,
                                              srs_du::f1u_tx_pdu_notifier& tx_pdu_notifier)
{
  logger.info("Connecting CU F1-U bearer. DL-TEID={}, UL-TEID={}", dl_teid, ul_teid);
  if (dl_map.find(dl_teid) == dl_map.end()) {
    logger.info("Could not find DL-TEID. DL-TEID={}, UL-TEID", dl_teid, ul_teid);
    return;
  }
  f1u_ul_local_adapter                du_tx      = {};
  std::unique_ptr<srs_du::f1u_bearer> f1u_bearer = srs_du::create_f1u_bearer(drb_id_t{}, du_rx, tx_pdu_notifier);
  auto&                               dl_tun     = dl_map.at(dl_teid);
  // dl_tun.cu_tx.attach_du_handler(du_rx);

  du_tx.attach_cu_handler(dl_tun.f1u_bearer->get_rx_pdu_handler());

  // f1u_ul_du_bearer ul_bearer(du_tx, du_rx);
  // ul_map.insert({ul_teid, ul_bearer});
}
