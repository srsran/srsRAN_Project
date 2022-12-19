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

#pragma once

#include "f1u_local_bearer_adapter.h"
#include "srsgnb/f1u/common/f1u_local_connector.h"
#include "srsgnb/srslog/srslog.h"
#include <cstdint>
#include <map>

namespace srsgnb {

struct f1u_dl_cu_bearer {
  std::unique_ptr<f1u_dl_local_adapter>  cu_tx      = nullptr;
  std::unique_ptr<srs_cu_up::f1u_bearer> f1u_bearer = nullptr;
  f1u_dl_cu_bearer(std::unique_ptr<f1u_dl_local_adapter> cu_tx_, std::unique_ptr<srs_cu_up::f1u_bearer> f1u_bearer_) :
    cu_tx(std::move(cu_tx_)), f1u_bearer(std::move(f1u_bearer_))
  {
  }
};

struct f1u_ul_du_bearer {
  std::unique_ptr<f1u_ul_local_adapter> du_tx      = nullptr;
  std::unique_ptr<srs_du::f1u_bearer>   f1u_bearer = nullptr;
  f1u_ul_du_bearer(std::unique_ptr<f1u_ul_local_adapter> du_tx_, std::unique_ptr<srs_du::f1u_bearer> f1u_bearer_) :
    du_tx(std::move(du_tx_)), f1u_bearer(std::move(f1u_bearer_))
  {
  }
};

/// \brief Object used to connect the DU and CU-UP F1-U bearers
/// On the co-located case this is done by connecting both entities
/// directly.
class f1u_local_connector_impl final : public f1u_local_connector, public f1u_du_connector, public f1u_cu_up_connector
{
public:
  f1u_local_connector_impl() : logger(srslog::fetch_basic_logger("F1-U")){};

  f1u_du_connector*    get_f1u_du_connector() override { return this; }
  f1u_cu_up_connector* get_f1u_cu_up_connector() override { return this; }

  srs_cu_up::f1u_bearer* create_cu_dl_bearer(uint32_t                             dl_teid,
                                             srs_cu_up::f1u_rx_delivery_notifier& rx_delivery_notifier,
                                             srs_cu_up::f1u_rx_sdu_notifier&      rx_sdu_notifier) override;
  void                   attach_cu_ul_bearer(uint32_t dl_teid, uint32_t ul_teid) override;
  srs_du::f1u_bearer*
  create_du_ul_bearer(uint32_t dl_teid, uint32_t ul_teid, srs_du::f1u_rx_sdu_notifier& du_rx) override;

private:
  srslog::basic_logger&                logger;
  std::map<uint32_t, f1u_dl_cu_bearer> dl_map;
  std::map<uint32_t, f1u_ul_du_bearer> ul_map;
};
}; // namespace srsgnb
