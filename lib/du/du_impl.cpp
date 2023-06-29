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
#include "srsran/du_high/du_high_factory.h"

using namespace srsran;

du_impl::du_impl(const du_config& du_cfg) : logger(srslog::fetch_basic_logger("DU")), du_hi(make_du_high(du_cfg.du_hi))
{
  logger.info("DU created successfully");
}

void du_impl::start()
{
  logger.info("Starting DU-High...");
  du_hi->start();
  logger.info("DU-High started successfully");
}

void du_impl::stop()
{
  logger.info("Closing DU-high...");
  du_hi->stop();
  logger.info("DU-high closed successfully");
}
