/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_ofh_error_handler_impl.h"

using namespace srsran;

void ru_ofh_error_handler_impl::on_late_downlink_message(const ofh::error_context& context)
{
  ru_error_context ru_err_context;
  ru_err_context.slot   = context.slot;
  ru_err_context.sector = context.sector;

  error_notifier.on_late_downlink_message(ru_err_context);
}
