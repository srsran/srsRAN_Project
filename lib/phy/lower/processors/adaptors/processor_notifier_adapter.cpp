/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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