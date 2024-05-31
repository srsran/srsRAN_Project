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

#include "srsran/fapi/buffered_decorator_factories.h"
#include "buffered_decorator_impl.h"

using namespace srsran;
using namespace fapi;

std::unique_ptr<buffered_decorator> srsran::fapi::create_buffered_decorator(unsigned              l2_nof_slots_ahead,
                                                                            subcarrier_spacing    scs,
                                                                            slot_message_gateway& gateway,
                                                                            task_executor&        executor)
{
  return std::make_unique<buffered_decorator_impl>(l2_nof_slots_ahead, scs, gateway, executor);
}
