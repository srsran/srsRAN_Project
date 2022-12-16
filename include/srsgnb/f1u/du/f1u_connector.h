/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/f1u/du/f1u_bearer.h"
#include "srsgnb/f1u/du/f1u_rx_pdu_handler.h"
#include "srsgnb/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsgnb/f1u/du/f1u_tx_pdu_notifier.h"

/*
 * This file will hold the interfaces for the connector between
 * the DU and CU-UP. At the moment, only co-located configurations
 * are supported.
 */

namespace srsgnb {

/// TODO write docs.
class f1u_du_connector
{
public:
  f1u_du_connector()                                    = default;
  virtual ~f1u_du_connector()                           = default;
  f1u_du_connector(const f1u_du_connector&)             = delete;
  f1u_du_connector& operator=(const f1u_du_connector&)  = delete;
  f1u_du_connector(const f1u_du_connector&&)            = delete;
  f1u_du_connector& operator=(const f1u_du_connector&&) = delete;

  virtual srs_du::f1u_bearer*
  create_du_ul_bearer(uint32_t dl_teid, uint32_t ul_teid, srs_du::f1u_rx_sdu_notifier& du_rx) = 0;
};

} // namespace srsgnb
