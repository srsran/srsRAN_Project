/*
 *
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "f1u_local_bearer_adapter.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/f1u/du/f1u_gateway.h"
#include "srsran/srslog/srslog.h"
#include <cstdint>
#include <unordered_map>

namespace srsran {

struct f1u_cu_bearer {
  std::unique_ptr<f1u_dl_local_adapter> cu_tx      = nullptr;
  srs_cu_up::f1u_bearer*                f1u_bearer = nullptr;
  optional<gtpu_teid_t>                 dl_teid; ///< holds the dl_teid to disconnect UL at DU upon bearer removal
  f1u_cu_bearer(std::unique_ptr<f1u_dl_local_adapter> cu_tx_, srs_cu_up::f1u_bearer* f1u_bearer_) :
    cu_tx(std::move(cu_tx_)), f1u_bearer(f1u_bearer_)
  {
  }
};

struct f1u_du_bearer {
  std::unique_ptr<f1u_ul_local_adapter> du_tx      = nullptr;
  std::unique_ptr<srs_du::f1u_bearer>   f1u_bearer = nullptr;
  gtpu_teid_t                           ul_teid; ///< holds the ul_teid to disconnect DL at CU upon bearer removal
  f1u_du_bearer(std::unique_ptr<f1u_ul_local_adapter> du_tx_,
                std::unique_ptr<srs_du::f1u_bearer>   f1u_bearer_,
                gtpu_teid_t                           ul_teid_) :
    du_tx(std::move(du_tx_)), f1u_bearer(std::move(f1u_bearer_)), ul_teid(ul_teid_)
  {
  }
};

/// \brief Object used to connect the DU and CU-UP F1-U bearers
/// On the co-located case this is done by connecting both entities directly.
///
/// Note that CU and DU bearer creation and removal can be performed from different threads and are therefore
/// protected by a common mutex.
class f1u_local_connector final : public srs_du::f1u_du_gateway, public f1u_cu_up_gateway
{
public:
  f1u_local_connector() :
    logger_cu(srslog::fetch_basic_logger("CU-F1-U")), logger_du(srslog::fetch_basic_logger("DU-F1-U"))
  {
  }

  srs_du::f1u_du_gateway* get_f1u_du_gateway() { return this; }
  f1u_cu_up_gateway*      get_f1u_cu_up_gateway() { return this; }

  std::unique_ptr<srs_cu_up::f1u_bearer> create_cu_bearer(uint32_t                             ue_index,
                                                          gtpu_teid_t                          ul_teid,
                                                          srs_cu_up::f1u_rx_delivery_notifier& rx_delivery_notifier,
                                                          srs_cu_up::f1u_rx_sdu_notifier&      rx_sdu_notifier,
                                                          timer_factory                        timers) override;
  void                                   attach_dl_teid(gtpu_teid_t ul_teid, gtpu_teid_t dl_teid) override;
  void                                   disconnect_cu_bearer(gtpu_teid_t ul_teid) override;

  srs_du::f1u_bearer* create_du_bearer(uint32_t                     ue_index,
                                       drb_id_t                     drb_id,
                                       srs_du::f1u_config           config,
                                       gtpu_teid_t                  dl_teid,
                                       gtpu_teid_t                  ul_teid,
                                       srs_du::f1u_rx_sdu_notifier& du_rx,
                                       timer_factory                timers) override;
  void                remove_du_bearer(gtpu_teid_t dl_teid) override;

private:
  srslog::basic_logger& logger_cu;
  srslog::basic_logger& logger_du;
  std::unordered_map<gtpu_teid_t, f1u_cu_bearer, gtpu_teid_hasher_t>
      cu_map; // Key is UL-TEID (i.e., the CU's local TEID)
  std::unordered_map<gtpu_teid_t, f1u_du_bearer, gtpu_teid_hasher_t>
             du_map;    // Key is DL-TEID (i.e., the DU's local TEID)
  std::mutex map_mutex; // shared mutex for access to cu_map and du_map
};
}; // namespace srsran
