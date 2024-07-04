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

#include "cu_up_wrapper.h"

using namespace srsran;

cu_up_wrapper::cu_up_wrapper(std::unique_ptr<srs_cu_up::ngu_gateway>     gateway_,
                             std::unique_ptr<srs_cu_up::cu_up_interface> cu_up_) :
  gateway(std::move(gateway_)), cu_up(std::move(cu_up_))
{
  srsran_assert(gateway, "Invlid NGU gateway");
  srsran_assert(cu_up, "Invalid CU-UP");
}

void cu_up_wrapper::start()
{
  cu_up->start();
}

void cu_up_wrapper::stop()
{
  cu_up->stop();
}

std::optional<uint16_t> cu_up_wrapper::get_n3_bind_port()
{
  return cu_up->get_n3_bind_port();
}
