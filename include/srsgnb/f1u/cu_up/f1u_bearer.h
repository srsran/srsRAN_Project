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

#include "f1u_rx_pdu_handler.h"
#include "f1u_tx_sdu_handler.h"

namespace srsgnb {
namespace srs_cu_up {

class f1u_bearer_origin;

class f1u_bearer
{
public:
  f1u_bearer()          = default;
  virtual ~f1u_bearer() = default;

  f1u_bearer(const f1u_bearer&)            = delete;
  f1u_bearer& operator=(const f1u_bearer&) = delete;

  virtual f1u_rx_pdu_handler& get_rx_pdu_handler() = 0;
  virtual f1u_tx_sdu_handler& get_tx_sdu_handler() = 0;
};

/// This class represents the interface through which a F1-U bearer disconnects from its gateway upon destruction
class f1u_bearer_origin
{
public:
  virtual ~f1u_bearer_origin()                    = default;
  virtual void remove_cu_bearer(uint32_t ul_teid) = 0;
};

} // namespace srs_cu_up
} // namespace srsgnb
