/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/phy/lower/processors/uplink/uplink_processor_factories.h"
#include "uplink_processor_impl.h"

using namespace srsran;

namespace {

class lower_phy_uplink_processor_factory_sw : public lower_phy_uplink_processor_factory
{
public:
  lower_phy_uplink_processor_factory_sw(std::shared_ptr<prach_processor_factory> prach_proc_factory_,
                                        std::shared_ptr<puxch_processor_factory> puxch_proc_factory_) :
    prach_proc_factory(std::move(prach_proc_factory_)), puxch_proc_factory(std::move(puxch_proc_factory_))
  {
    srsran_assert(prach_proc_factory, "Invalid PRACH processor factory.");
    srsran_assert(puxch_proc_factory, "Invalid PUxCH processor factory.");
  }

  std::unique_ptr<lower_phy_uplink_processor> create(const uplink_processor_configuration& config) override
  {
    // Prepare PUxCH processor configuration.
    puxch_processor_configuration puxch_proc_config;
    puxch_proc_config.cp                = config.cp;
    puxch_proc_config.scs               = config.scs;
    puxch_proc_config.srate             = config.rate;
    puxch_proc_config.bandwidth_rb      = config.bandwidth_prb;
    puxch_proc_config.dft_window_offset = dft_window_offset;
    puxch_proc_config.center_freq_Hz    = config.center_frequency_Hz;
    puxch_proc_config.nof_rx_ports      = config.nof_rx_ports;

    // Prepare uplink processor configuration.
    lower_phy_uplink_processor_impl::configuration proc_config;
    proc_config.sector_id    = config.sector_id;
    proc_config.scs          = config.scs;
    proc_config.cp           = config.cp;
    proc_config.rate         = config.rate;
    proc_config.nof_rx_ports = config.nof_rx_ports;

    return std::make_unique<lower_phy_uplink_processor_impl>(
        prach_proc_factory->create(), puxch_proc_factory->create(puxch_proc_config), proc_config);
  }

private:
  static constexpr float                   dft_window_offset = 0.5F;
  std::shared_ptr<prach_processor_factory> prach_proc_factory;
  std::shared_ptr<puxch_processor_factory> puxch_proc_factory;
};

} // namespace

std::shared_ptr<lower_phy_uplink_processor_factory>
srsran::create_uplink_processor_factory_sw(std::shared_ptr<prach_processor_factory> prach_proc_factory,
                                           std::shared_ptr<puxch_processor_factory> puxch_proc_factory)
{
  return std::make_shared<lower_phy_uplink_processor_factory_sw>(std::move(prach_proc_factory),
                                                                 std::move(puxch_proc_factory));
}