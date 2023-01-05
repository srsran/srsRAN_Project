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

#include "f1u_local_connector.h"
#include "srsgnb/f1u/cu_up/f1u_bearer_factory.h"
#include "srsgnb/f1u/du/f1u_bearer_factory.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/srslog/srslog.h"

using namespace srsgnb;

srs_cu_up::f1u_bearer*
f1u_local_connector::create_cu_dl_bearer(uint32_t                             dl_teid,
                                         srs_cu_up::f1u_rx_delivery_notifier& rx_delivery_notifier,
                                         srs_cu_up::f1u_rx_sdu_notifier&      rx_sdu_notifier)
{
  logger.info("Creating CU F1-U bearer. DL-TEID={}", dl_teid);
  std::unique_ptr<f1u_dl_local_adapter>  cu_tx = std::make_unique<f1u_dl_local_adapter>();
  std::unique_ptr<srs_cu_up::f1u_bearer> f1u_bearer =
      srs_cu_up::create_f1u_bearer(drb_id_t{}, *cu_tx, rx_delivery_notifier, rx_sdu_notifier);
  srs_cu_up::f1u_bearer* ptr = f1u_bearer.get();
  f1u_dl_cu_bearer       dl_bearer(std::move(cu_tx), std::move(f1u_bearer));
  dl_map.insert({dl_teid, std::move(dl_bearer)});
  return ptr;
}

void f1u_local_connector::attach_cu_ul_bearer(uint32_t dl_teid, uint32_t ul_teid)
{
  logger.debug("Connecting CU F1-U bearer. DL-TEID={}", dl_teid);
  if (dl_map.find(dl_teid) == dl_map.end()) {
    logger.warning("Could not find DL-TEID. DL-TEID={}, UL-TEID={}", dl_teid, ul_teid);
    return;
  }
  if (ul_map.find(ul_teid) == ul_map.end()) {
    logger.warning("Could not find UL-TEID. UL-TEID={}, UL-TEID={}", dl_teid, ul_teid);
    return;
  }
  auto& dl_tun = dl_map.at(dl_teid);
  auto& ul_tun = ul_map.at(ul_teid);
  ul_tun.du_tx->attach_cu_handler(dl_tun.f1u_bearer->get_rx_pdu_handler());
}

srs_du::f1u_bearer*
f1u_local_connector::create_du_ul_bearer(uint32_t dl_teid, uint32_t ul_teid, srs_du::f1u_rx_sdu_notifier& du_rx)
{
  logger.debug("Connecting CU F1-U bearer. DL-TEID={}, UL-TEID={}", dl_teid, ul_teid);
  if (dl_map.find(dl_teid) == dl_map.end()) {
    logger.warning("Could not find DL-TEID. DL-TEID={}, UL-TEID={}", dl_teid, ul_teid);
    return nullptr;
  }
  std::unique_ptr<f1u_ul_local_adapter> du_tx      = std::make_unique<f1u_ul_local_adapter>();
  std::unique_ptr<srs_du::f1u_bearer>   f1u_bearer = srs_du::create_f1u_bearer(drb_id_t{}, du_rx, *du_tx);
  srs_du::f1u_bearer*                   ptr        = f1u_bearer.get();
  auto&                                 dl_tun     = dl_map.at(dl_teid);
  dl_tun.cu_tx->attach_du_handler(f1u_bearer->get_rx_pdu_handler());

  du_tx->attach_cu_handler(dl_tun.f1u_bearer->get_rx_pdu_handler());

  f1u_ul_du_bearer ul_bearer(std::move(du_tx), std::move(f1u_bearer));
  ul_map.insert({ul_teid, std::move(ul_bearer)});
  return ptr;
}
