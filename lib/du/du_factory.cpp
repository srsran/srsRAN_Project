/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/du/du_factory.h"
#include "du_impl.h"

using namespace srsran;

std::unique_ptr<du> srsran::make_du(const du_config& du_cfg)
{
  return std::make_unique<du_impl>(du_cfg);
}