/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/f1u/f1u_cu_up_rx.h"
#include "srsran/f1u/f1u_cu_up_transport.h"
#include "srsran/f1u/f1u_cu_up_tx.h"

namespace srsran {

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

} // namespace srsran
