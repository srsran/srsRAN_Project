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
#include "processor_notifier_adaptor.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

void processor_notifier_adaptor::prach_adaptor::on_prach_request_late(const prach_buffer_context& context)
{
  srsran_assert(error_notifier, "The adaptor is not connected to an error notifier.");
  error_notifier->on_prach_request_late(context);
}

void processor_notifier_adaptor::prach_adaptor::on_prach_request_overflow(const prach_buffer_context& context)
{
  srsran_assert(error_notifier, "The adaptor is not connected to an error notifier.");
  error_notifier->on_prach_request_overflow(context);
}

void processor_notifier_adaptor::prach_adaptor::on_rx_prach_window(const prach_buffer&         buffer,
                                                                   const prach_buffer_context& context)
{
  srsran_assert(rx_notifier, "The adaptor is not connected to receiver notifier.");
  rx_notifier->on_rx_prach_window(context, buffer);
}