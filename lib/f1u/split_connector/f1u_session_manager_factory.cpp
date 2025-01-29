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

#include "srsran/f1u/split_connector/f1u_session_manager_factory.h"
#include "f1u_session_manager_impl.h"

using namespace srsran;

std::unique_ptr<f1u_session_manager> srsran::create_f1u_cu_up_session_manager(const f1u_session_maps& f1u_sessions)
{
  return std::make_unique<f1u_session_manager_impl>(f1u_sessions);
}
