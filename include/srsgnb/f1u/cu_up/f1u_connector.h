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

#include "srsgnb/f1u/cu_up/f1u_bearer.h"
#include "srsgnb/f1u/cu_up/f1u_rx_delivery_notifier.h"
#include "srsgnb/f1u/cu_up/f1u_rx_sdu_notifier.h"
#include "srsgnb/srslog/srslog.h"

/*
 * This file will hold the interfaces for the connector between
 * the DU and CU-UP. At the moment, only co-located configurations
 * are supported.
 */
namespace srsgnb {

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

  virtual srs_cu_up::f1u_bearer* create_cu_dl_bearer(uint32_t                             dl_teid,
                                                     srs_cu_up::f1u_rx_delivery_notifier& rx_delivery_notifier,
                                                     srs_cu_up::f1u_rx_sdu_notifier&      rx_sdu_notifier) = 0;
  virtual void                   attach_cu_ul_bearer(uint32_t dl_teid, uint32_t ul_teid)              = 0;
};

} // namespace srsgnb
