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
#include <map>

namespace srsran {

struct f1u_cu_bearer {
  std::unique_ptr<f1u_dl_local_adapter> cu_tx      = nullptr;
  srs_cu_up::f1u_bearer*                f1u_bearer = nullptr;
  optional<uint32_t>                    dl_teid; ///< holds the dl_teid to disconnect UL at DU upon bearer removal
  f1u_cu_bearer(std::unique_ptr<f1u_dl_local_adapter> cu_tx_, srs_cu_up::f1u_bearer* f1u_bearer_) :
    cu_tx(std::move(cu_tx_)), f1u_bearer(f1u_bearer_)
  {
  }
};

struct f1u_du_bearer {
  std::unique_ptr<f1u_ul_local_adapter> du_tx      = nullptr;
  std::unique_ptr<srs_du::f1u_bearer>   f1u_bearer = nullptr;
  f1u_du_bearer(std::unique_ptr<f1u_ul_local_adapter> du_tx_, std::unique_ptr<srs_du::f1u_bearer> f1u_bearer_) :
    du_tx(std::move(du_tx_)), f1u_bearer(std::move(f1u_bearer_))
  {
  }
};

/// \brief Object used to connect the DU and CU-UP F1-U bearers
/// On the co-located case this is done by connecting both entities
/// directly.
class f1u_local_connector final : public srs_du::f1u_du_gateway, public f1u_cu_up_gateway
{
public:
  f1u_local_connector() : logger(srslog::fetch_basic_logger("F1-U")) {}

  srs_du::f1u_du_gateway* get_f1u_du_gateway() { return this; }
  f1u_cu_up_gateway*      get_f1u_cu_up_gateway() { return this; }

  std::unique_ptr<srs_cu_up::f1u_bearer> create_cu_bearer(uint32_t                             ue_index,
                                                          uint32_t                             ul_teid,
                                                          srs_cu_up::f1u_rx_delivery_notifier& rx_delivery_notifier,
                                                          srs_cu_up::f1u_rx_sdu_notifier& rx_sdu_notifier) override;
  void                                   attach_dl_teid(uint32_t ul_teid, uint32_t dl_teid) override;
  void                                   disconnect_cu_bearer(uint32_t ul_teid) override;

  srs_du::f1u_bearer*
  create_du_bearer(uint32_t ue_index, uint32_t dl_teid, uint32_t ul_teid, srs_du::f1u_rx_sdu_notifier& du_rx) override;
  void remove_du_bearer(uint32_t dl_teid) override;

private:
  srslog::basic_logger&             logger;
  std::map<uint32_t, f1u_cu_bearer> cu_map;    // Key is UL-TEID (i.e., the CU's local TEID)
  std::map<uint32_t, f1u_du_bearer> du_map;    // Key is DL-TEID (i.e., the DU's local TEID)
  std::mutex                        map_mutex; // shared mutex for access to cu_map and du_map
};
}; // namespace srsran
