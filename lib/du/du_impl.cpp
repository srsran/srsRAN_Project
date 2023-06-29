/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_impl.h"

using namespace srsran;

du_impl::du_impl(const du_config& du_cfg) : logger(srslog::fetch_basic_logger("DU"))
{
  logger.info("DU created successfully");
}

void du_impl::start() {}

void du_impl::stop() {}
