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

#include "srsgnb/f1u/common/f1u_local_bearer_adapter.h"
#include "srsgnb/f1u/cu_up/f1u_rx_pdu_handler.h"
#include "srsgnb/f1u/cu_up/f1u_tx_pdu_notifier.h"
#include "srsgnb/f1u/du/f1u_rx_pdu_handler.h"
#include "srsgnb/f1u/du/f1u_tx_pdu_notifier.h"
#include "srsgnb/srslog/srslog.h"

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

  virtual void attach_du_bearer(uint32_t                     dl_teid,
                                uint32_t                     ul_teid,
                                srs_du::f1u_tx_pdu_notifier& du_tx,
                                srs_du::f1u_rx_pdu_handler&  du_rx) = 0;
};

/// TODO write docs.
class f1u_cu_up_connector
{
public:
  f1u_cu_up_connector()                                       = default;
  virtual ~f1u_cu_up_connector()                              = default;
  f1u_cu_up_connector(const f1u_cu_up_connector&)             = delete;
  f1u_cu_up_connector& operator=(const f1u_cu_up_connector&)  = delete;
  f1u_cu_up_connector(const f1u_cu_up_connector&&)            = delete;
  f1u_cu_up_connector& operator=(const f1u_cu_up_connector&&) = delete;

  virtual void
  attach_cu_bearer(uint32_t dl_teid, f1u_dl_local_adapter& cu_tx, srs_cu_up::f1u_rx_pdu_handler& cu_rx) = 0;
};

/// TODO write docs.
class f1u_connector
{
public:
  f1u_connector()                                 = default;
  virtual ~f1u_connector()                        = default;
  f1u_connector(const f1u_connector&)             = delete;
  f1u_connector& operator=(const f1u_connector&)  = delete;
  f1u_connector(const f1u_connector&&)            = delete;
  f1u_connector& operator=(const f1u_connector&&) = delete;

  virtual f1u_du_connector*    get_f1u_du_connector()    = 0;
  virtual f1u_cu_up_connector* get_f1u_cu_up_connector() = 0;
};

} // namespace srsgnb
