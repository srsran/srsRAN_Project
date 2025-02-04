/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/du/o_du_factory.h"
#include "o_du_impl.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_du;

std::unique_ptr<o_du> srs_du::make_o_du(o_du_dependencies&& dependencies)
{
  srslog::fetch_basic_logger("DU").info("O-DU created successfully");

  return std::make_unique<o_du_impl>(std::move(dependencies));
}
