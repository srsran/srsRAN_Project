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

namespace srsgnb {

/// This class will be used to provide the interfaces to
/// the DU to create and manage F1-U bearers.
class f1u_du_gateway
{
public:
  f1u_du_gateway()                                 = default;
  virtual ~f1u_du_gateway()                        = default;
  f1u_du_gateway(const f1u_du_gateway&)            = default;
  f1u_du_gateway& operator=(const f1u_du_gateway&) = default;
  f1u_du_gateway(f1u_du_gateway&&)                 = default;
  f1u_du_gateway& operator=(f1u_du_gateway&&)      = default;

  virtual srs_du::f1u_bearer*
  create_du_ul_bearer(uint32_t dl_teid, uint32_t ul_teid, srs_du::f1u_rx_sdu_notifier& du_rx) = 0;
};

} // namespace srsgnb
