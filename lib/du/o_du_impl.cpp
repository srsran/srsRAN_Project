/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "o_du_impl.h"
#include "srsran/du/du_high/o_du_high.h"
#include "srsran/du/du_low/du_low.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_sector_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_sector_adaptor.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_du;

o_du_impl::o_du_impl(o_du_impl_dependencies&& dependencies) :
  du_hi(std::move(dependencies.du_hi)), du_lo(std::move(dependencies.du_lo))
{
  srsran_assert(du_lo, "Invalid DU low");
  srsran_assert(du_hi, "Invalid DU high");
}

void o_du_impl::start()
{
  du_hi->get_power_controller().start();
}

void o_du_impl::stop()
{
  du_lo->get_du_low().get_power_controller().stop();
  du_hi->get_power_controller().stop();
}

o_du_high& o_du_impl::get_o_du_high()
{
  return *du_hi;
}

o_du_low& o_du_impl::get_o_du_low()
{
  return *du_lo;
}
