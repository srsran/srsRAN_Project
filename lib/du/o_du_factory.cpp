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

#include "srsran/du/o_du_factory.h"
#include "o_du_impl.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_du;

std::unique_ptr<o_du> srs_du::make_o_du(o_du_dependencies&& dependencies)
{
  srslog::fetch_basic_logger("DU").info("O-DU created successfully");

  return std::make_unique<o_du_impl>(std::move(dependencies));
}
