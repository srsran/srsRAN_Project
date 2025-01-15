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
#include "srsran/du/du_high/o_du_high_factory.h"
#include "srsran/du/du_low/o_du_low_factory.h"

using namespace srsran;
using namespace srs_du;

std::unique_ptr<o_du> srs_du::make_o_du(std::unique_ptr<o_du_high> odu_hi, std::unique_ptr<o_du_low> odu_lo)
{
  srsran_assert(odu_hi, "Invalid O-DU high");
  srsran_assert(odu_lo, "Invalid O-DU low");

  srslog::fetch_basic_logger("DU").info("O-DU created successfully");

  o_du_impl_dependencies o_du_deps;
  o_du_deps.du_lo = std::move(odu_lo);
  o_du_deps.du_hi = std::move(odu_hi);

  return std::make_unique<o_du_impl>(std::move(o_du_deps));
}
