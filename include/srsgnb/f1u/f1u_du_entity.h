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

#include "srsgnb/f1u/f1u_du_rx.h"
#include "srsgnb/f1u/f1u_du_tx.h"

namespace srsgnb {

class f1u_du_entity
{
public:
  f1u_du_entity()                                 = default;
  virtual ~f1u_du_entity()                        = default;
  f1u_du_entity(const f1u_du_entity&)             = delete;
  f1u_du_entity& operator=(const f1u_du_entity&)  = delete;
  f1u_du_entity(const f1u_du_entity&&)            = delete;
  f1u_du_entity& operator=(const f1u_du_entity&&) = delete;

  virtual f1u_du_tx_lower_data_interface& get_tx_lower_data_interface() = 0;
  virtual f1u_du_rx_lower_data_interface& get_rx_lower_data_interface() = 0;
};

} // namespace srsgnb
