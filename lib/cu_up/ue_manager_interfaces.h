/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ue_context.h"
#include <cstddef>

namespace srsran {

namespace srs_cu_up {

class ue_manager_ctrl
{
public:
  virtual ~ue_manager_ctrl() = default;

  virtual ue_context* add_ue(const ue_context_cfg& ue_cfg) = 0;
  virtual void        remove_ue(ue_index_t ue_index)       = 0;
  virtual ue_context* find_ue(ue_index_t ue_index)         = 0;
  virtual size_t      get_nof_ues() const                  = 0;
};

} // namespace srs_cu_up

} // namespace srsran
