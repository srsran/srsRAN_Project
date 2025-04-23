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

#include "o_cu_cp_impl.h"

using namespace srsran;
using namespace srs_cu_cp;

o_cu_cp_impl::o_cu_cp_impl(std::unique_ptr<cu_cp> cu_cp_) : cu_impl(std::move(cu_cp_))
{
  srsran_assert(cu_impl, "Invalid CU-CP");
}

void o_cu_cp_impl::start()
{
  cu_impl->start();
}

void o_cu_cp_impl::stop()
{
  cu_impl->stop();
}

o_cu_cp_with_e2_impl::o_cu_cp_with_e2_impl(std::unique_ptr<e2_agent> e2agent_, std::unique_ptr<cu_cp> cu_cp_) :
  o_cu_cp_impl(std::move(cu_cp_)), e2agent(std::move(e2agent_))
{
  srsran_assert(e2agent, "Invalid E2 agent");
}

void o_cu_cp_with_e2_impl::start()
{
  o_cu_cp_impl::start();
  e2agent->start();
}

void o_cu_cp_with_e2_impl::stop()
{
  e2agent->stop();
  o_cu_cp_impl::stop();
}
