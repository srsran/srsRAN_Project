/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_low_impl.h"

using namespace srsran;

du_low_impl::du_low_impl(std::vector<std::unique_ptr<upper_phy>> upper_) : upper(std::move(upper_))
{
  srsran_assert(!upper.empty(), "Invalid upper PHY");
}

upper_phy& du_low_impl::get_upper_phy(unsigned cell_id)
{
  srsran_assert(cell_id < upper.size(), "Invalid cell index '{}'. Valid cell indexes [0-{})", cell_id, upper.size());

  return *upper[cell_id];
}

void du_low_impl::stop()
{
  for (auto& cell : upper) {
    cell->stop();
  }
}