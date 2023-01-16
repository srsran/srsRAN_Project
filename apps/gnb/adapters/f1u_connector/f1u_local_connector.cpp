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

srs_cu_up::f1u_bearer* f1u_local_connector::create_cu_bearer(uint32_t                             ul_teid,
                                                             srs_cu_up::f1u_rx_delivery_notifier& rx_delivery_notifier,
                                                             srs_cu_up::f1u_rx_sdu_notifier&      rx_sdu_notifier)
{
  logger.info("Creating CU F1-U bearer. UL-TEID={}", ul_teid);
  std::unique_ptr<f1u_dl_local_adapter>  cu_tx = std::make_unique<f1u_dl_local_adapter>();
  std::unique_ptr<srs_cu_up::f1u_bearer> f1u_bearer =
      srs_cu_up::create_f1u_bearer(drb_id_t{}, *cu_tx, rx_delivery_notifier, rx_sdu_notifier);
  srs_cu_up::f1u_bearer* ptr = f1u_bearer.get();
  f1u_cu_bearer          cu_bearer(std::move(cu_tx), std::move(f1u_bearer));
  cu_map.insert({ul_teid, std::move(cu_bearer)});
  return ptr;
}

void f1u_local_connector::attach_dl_teid(uint32_t ul_teid, uint32_t dl_teid)
{
  if (cu_map.find(ul_teid) == cu_map.end()) {
    logger.warning("Could not find UL-TEID at CU to connect. UL-TEID={}, DL-TEID={}", ul_teid, dl_teid);
    return;
  }
  logger.debug("Connecting CU F1-U bearer. UL-TEID={}, DL-TEID={}", ul_teid, dl_teid);

  if (du_map.find(dl_teid) == du_map.end()) {
    logger.warning("Could not find DL-TEID at DU to connect. UL-TEID={}, DL-TEID={}", ul_teid, dl_teid);
    return;
  }
  logger.debug("Connecting DU F1-U bearer. UL-TEID={}, DL-TEID={}", ul_teid, dl_teid);

  auto& du_tun = du_map.at(dl_teid);
  auto& cu_tun = cu_map.at(ul_teid);
  du_tun.du_tx->attach_cu_handler(cu_tun.f1u_bearer->get_rx_pdu_handler());
}

void f1u_local_connector::remove_cu_bearer(uint32_t ul_teid)
{
  auto bearer_it = cu_map.find(ul_teid);
  if (bearer_it == cu_map.end()) {
    logger.warning("Could not find UL-TEID at CU to remove. UL-TEID={}", ul_teid);
    return;
  }
  logger.debug("Removing CU F1-U bearer. UL-TEID={}", ul_teid);
  cu_map.erase(bearer_it);
}

srs_du::f1u_bearer*
f1u_local_connector::create_du_bearer(uint32_t dl_teid, uint32_t ul_teid, srs_du::f1u_rx_sdu_notifier& du_rx)
{
  if (cu_map.find(ul_teid) == cu_map.end()) {
    logger.warning(
        "Could not find CU F1-U bearer, when creating DU F1-U bearer. DL-TEID={}, UL-TEID={}", dl_teid, ul_teid);
    return nullptr;
  }

  logger.debug("Creating DU F1-U bearer. DL-TEID={}, UL-TEID={}", dl_teid, ul_teid);
  std::unique_ptr<f1u_ul_local_adapter> du_tx      = std::make_unique<f1u_ul_local_adapter>();
  std::unique_ptr<srs_du::f1u_bearer>   f1u_bearer = srs_du::create_f1u_bearer(drb_id_t{}, du_rx, *du_tx);
  srs_du::f1u_bearer*                   ptr        = f1u_bearer.get();
  auto&                                 cu_tun     = cu_map.at(ul_teid);
  cu_tun.cu_tx->attach_du_handler(f1u_bearer->get_rx_pdu_handler());

  du_tx->attach_cu_handler(cu_tun.f1u_bearer->get_rx_pdu_handler());

  f1u_du_bearer du_bearer(std::move(du_tx), std::move(f1u_bearer));
  du_map.insert({dl_teid, std::move(du_bearer)});
  return ptr;
}
