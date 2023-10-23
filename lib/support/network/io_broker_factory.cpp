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

#include "srsran/support/io/io_broker_factory.h"
#include "io_broker_epoll.h"

using namespace srsran;

std::unique_ptr<io_broker> srsran::create_io_broker(io_broker_type type, io_broker_config config)
{
  switch (type) {
    case io_broker_type::epoll:
      return std::make_unique<io_broker_epoll>(config);
    default:
      srsran_terminate("IO broker type not supported");
  }
  return nullptr;
}
