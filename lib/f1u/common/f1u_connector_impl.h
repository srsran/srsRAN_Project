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

#include "srsgnb/f1u/cu_up/f1u_rx_pdu_handler.h"
#include "srsgnb/f1u/cu_up/f1u_tx_pdu_notifier.h"
#include "srsgnb/f1u/du/f1u_rx_pdu_handler.h"
#include "srsgnb/f1u/du/f1u_tx_pdu_notifier.h"
#include "srsgnb/srslog/srslog.h"
#include <cstdint>

namespace srsgnb {

/// \brief Object used to connect the DU and CU-UP F1-U bearers
/// On the co-located case this is done by connecting both entities
/// directly.
class f1u_connector_impl
{
public:
  f1u_connector_impl() : logger(srslog::fetch_basic_logger("F1-U")){};

  void attach_cu_bearer(uint32_t dl_teid, srs_cu_up::f1u_tx_pdu_notifier& cu_tx, srs_cu_up::f1u_rx_pdu_handler& cu_rx);
  void attach_du_bearer(uint32_t                     dl_teid,
                        uint32_t                     ul_teid,
                        srs_du::f1u_tx_pdu_notifier& du_tx,
                        srs_du::f1u_rx_pdu_handler&  du_rx);

private:
  srslog::basic_logger& logger;
};
}; // namespace srsgnb
