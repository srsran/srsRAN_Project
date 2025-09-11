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

#include "srsran/phy/lower/processors/downlink/downlink_processor_factories.h"
#include "downlink_processor_impl.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_factories.h"

using namespace srsran;

namespace {

class lower_phy_downlink_processor_factory_sw : public lower_phy_downlink_processor_factory
{
public:
  lower_phy_downlink_processor_factory_sw(std::shared_ptr<pdxch_processor_factory> pdxch_proc_factory_) :
    pdxch_proc_factory(std::move(pdxch_proc_factory_))
  {
    srsran_assert(pdxch_proc_factory, "Invalid PDxCH processor factory.");
  }

  std::unique_ptr<lower_phy_downlink_processor> create(const downlink_processor_configuration& config) override
  {
    pdxch_processor_configuration pdxch_proc_config = {.cp                  = config.cp,
                                                       .scs                 = config.scs,
                                                       .srate               = config.rate,
                                                       .bandwidth_rb        = config.bandwidth_prb,
                                                       .center_freq_Hz      = config.center_frequency_Hz,
                                                       .nof_tx_ports        = config.nof_tx_ports,
                                                       .modulation_executor = config.modulation_executor};

    downlink_processor_baseband_configuration baseband_config = {.sector_id    = config.sector_id,
                                                                 .scs          = config.scs,
                                                                 .cp           = config.cp,
                                                                 .rate         = config.rate,
                                                                 .nof_tx_ports = config.nof_tx_ports,
                                                                 .nof_slot_tti_in_advance =
                                                                     config.nof_slot_tti_in_advance};

    return std::make_unique<downlink_processor_impl>(pdxch_proc_factory->create(pdxch_proc_config), baseband_config);
  }

private:
  std::shared_ptr<pdxch_processor_factory> pdxch_proc_factory;
};

} // namespace

std::shared_ptr<lower_phy_downlink_processor_factory>
srsran::create_downlink_processor_factory_sw(std::shared_ptr<pdxch_processor_factory> pdxch_proc_factory)
{
  return std::make_shared<lower_phy_downlink_processor_factory_sw>(std::move(pdxch_proc_factory));
}
