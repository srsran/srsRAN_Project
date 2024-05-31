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

#include "du_wrapper_impl.h"
#include "srsran/du/du_high_wrapper.h"
#include "srsran/du_low/du_low.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

du_wrapper_impl::du_wrapper_impl(du_wrapper_impl_dependencies&& dependencies) :
  du_lo(std::move(dependencies.du_lo)), du_hi(std::move(dependencies.du_hi))
{
  srsran_assert(du_lo, "Invalid DU low");
  srsran_assert(du_hi, "Invalid DU high");
}

void du_wrapper_impl::start()
{
  du_hi->start();
}

void du_wrapper_impl::stop()
{
  du_lo->get_du_low().stop();
  du_hi->stop();
}

du_high_wrapper& du_wrapper_impl::get_du_high_wrapper()
{
  return *du_hi;
}

du_low_wrapper& du_wrapper_impl::get_du_low_wrapper()
{
  return *du_lo;
}
