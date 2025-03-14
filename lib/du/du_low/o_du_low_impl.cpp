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

#include "o_du_low_impl.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_sector_adaptor.h"
#include "srsran/phy/upper/upper_phy.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_du;

o_du_low_impl::o_du_low_impl(std::unique_ptr<du_low>                         du_lo_,
                             std::unique_ptr<fapi_adaptor::phy_fapi_adaptor> fapi_adaptor_,
                             unsigned                                        nof_cells) :
  du_lo(std::move(du_lo_)), fapi_adaptor(std::move(fapi_adaptor_)), metrics_collector(du_lo->get_metrics_collector())
{
  srsran_assert(du_lo, "Invalid DU low");
  srsran_assert(fapi_adaptor, "Invalid PHY-FAPI adapter");

  for (unsigned i = 0; i != nof_cells; ++i) {
    upper_phy&                             upper          = du_lo->get_upper_phy(i);
    fapi_adaptor::phy_fapi_sector_adaptor& sector_adaptor = fapi_adaptor->get_sector_adaptor(i);

    upper.set_rx_results_notifier(sector_adaptor.get_rx_results_notifier());
    upper.set_timing_notifier(sector_adaptor.get_timing_notifier());
    upper.set_error_notifier(sector_adaptor.get_error_notifier());
  }
}

du_low& o_du_low_impl::get_du_low()
{
  return *du_lo;
}

fapi_adaptor::phy_fapi_adaptor& o_du_low_impl::get_phy_fapi_adaptor()
{
  return *fapi_adaptor;
}

o_du_low_metrics_collector_impl* o_du_low_impl::get_metrics_collector()
{
  return metrics_collector.enabled() ? &metrics_collector : nullptr;
}

void o_du_low_impl::start()
{
  // Nothing to do as the FAPI adaptor and DU low are stateless.
}

void o_du_low_impl::stop()
{
  srsran_assert(du_lo, "Invalid DU low object");
  du_lo->get_operation_controller().stop();
}
