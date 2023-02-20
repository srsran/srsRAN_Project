/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/f1u/f1u_cu_up_rx.h"
#include "srsgnb/f1u/f1u_cu_up_transport.h"
#include "srsgnb/f1u/f1u_cu_up_tx.h"

namespace srsgnb {

class f1u_cu_up_entity
{
public:
  f1u_cu_up_entity()                                    = default;
  virtual ~f1u_cu_up_entity()                           = default;
  f1u_cu_up_entity(const f1u_cu_up_entity&)             = delete;
  f1u_cu_up_entity& operator=(const f1u_cu_up_entity&)  = delete;
  f1u_cu_up_entity(const f1u_cu_up_entity&&)            = delete;
  f1u_cu_up_entity& operator=(const f1u_cu_up_entity&&) = delete;

  virtual f1u_cu_up_tx_upper_data_interface&   get_tx_upper_data_interface()   = 0;
  virtual f1u_cu_up_lower_transport_interface& get_lower_transport_interface() = 0;
};

} // namespace srsgnb
