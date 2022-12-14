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
#include "srsgnb/f1u/common/f1u_connector.h"
#include <map>

namespace srsgnb {

struct f1u_dl_cu_bearer {
  f1u_dl_local_adapter&                  cu_tx;
  std::unique_ptr<srs_cu_up::f1u_bearer> f1u_bearer = nullptr;
  f1u_dl_cu_bearer(f1u_dl_local_adapter& cu_tx_, std::unique_ptr<srs_cu_up::f1u_bearer> f1u_bearer_) :
    cu_tx(cu_tx_), f1u_bearer(std::move(f1u_bearer_))
  {
  }
};

struct f1u_ul_du_bearer {
  f1u_ul_local_adapter&       du_tx;
  srs_du::f1u_rx_pdu_handler& du_rx;
  f1u_ul_du_bearer(f1u_ul_local_adapter& du_tx_, srs_du::f1u_rx_pdu_handler& du_rx_) : du_tx(du_tx_), du_rx(du_rx_) {}
};

/// \brief Object used to connect the DU and CU-UP F1-U bearers
/// On the co-located case this is done by connecting both entities
/// directly.
class f1u_local_connector final : public f1u_du_connector, public f1u_cu_up_connector
{
public:
  f1u_local_connector() : logger(srslog::fetch_basic_logger("F1-U")){};

  srs_cu_up::f1u_bearer* create_cu_dl_bearer(uint32_t                        dl_teid,
                                             srs_cu_up::f1u_rx_sdu_notifier& rx_sdu_notifier) override;
  void                   attach_cu_ul_bearer(uint32_t dl_teid, uint32_t ul_teid) override;
  void                   create_du_ul_bearer(uint32_t                     dl_teid,
                                             uint32_t                     ul_teid,
                                             srs_du::f1u_rx_pdu_handler&  du_rx,
                                             srs_du::f1u_tx_pdu_notifier& du_tx);
  void                   create_du_ul_bearer(uint32_t                     dl_teid,
                                             uint32_t                     ul_teid,
                                             srs_du::f1u_rx_sdu_notifier& du_rx,
                                             srs_du::f1u_tx_pdu_notifier& du_tx) override;

private:
  srslog::basic_logger&                logger;
  std::map<uint32_t, f1u_dl_cu_bearer> dl_map;
  std::map<uint32_t, f1u_ul_du_bearer> ul_map;
};
}; // namespace srsgnb
