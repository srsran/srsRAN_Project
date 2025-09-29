/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/rrc/rrc_du_factory.h"
#include "rrc_du_impl.h"
#include "srsran/rrc/rrc_config.h"

using namespace srsran;
using namespace srs_cu_cp;

std::unique_ptr<rrc_du> srsran::srs_cu_cp::create_rrc_du(const rrc_cfg_t& cfg)
{
  return std::make_unique<rrc_du_impl>(cfg);
}
