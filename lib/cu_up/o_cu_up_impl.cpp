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

#include "o_cu_up_impl.h"

using namespace srsran;
using namespace srs_cu_up;

void o_cu_up_impl::start()
{
  cu_up->start();
}

void o_cu_up_impl::stop()
{
  cu_up->stop();
}

void o_cu_up_with_e2_impl::start()
{
  o_cu_up_impl::start();
  e2agent->start();
}

void o_cu_up_with_e2_impl::stop()
{
  e2agent->stop();
  o_cu_up_impl::stop();
}
