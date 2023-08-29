/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  lower_phy_downlink_processor_factory_sw(std::shared_ptr<pdxch_processor_factory>      pdxch_proc_factory_,
                                          std::shared_ptr<amplitude_controller_factory> amplitude_control_factory_) :
    pdxch_proc_factory(std::move(pdxch_proc_factory_)), amplitude_control_factory(std::move(amplitude_control_factory_))
  {
    srsran_assert(pdxch_proc_factory, "Invalid PDxCH processor factory.");
    srsran_assert(amplitude_control_factory, "Invalid amplitude control factory.");
  }

  std::unique_ptr<lower_phy_downlink_processor> create(const downlink_processor_configuration& config) override
  {
    srsran_assert(config.logger, "Invalid logger.");

    pdxch_processor_configuration pdxch_proc_config;
    pdxch_proc_config.cp             = config.cp;
    pdxch_proc_config.scs            = config.scs;
    pdxch_proc_config.srate          = config.rate;
    pdxch_proc_config.bandwidth_rb   = config.bandwidth_prb;
    pdxch_proc_config.center_freq_Hz = config.center_frequency_Hz;
    pdxch_proc_config.nof_tx_ports   = config.nof_tx_ports;

    downlink_processor_baseband_configuration baseband_config;
    baseband_config.sector_id               = config.sector_id;
    baseband_config.scs                     = config.scs;
    baseband_config.cp                      = config.cp;
    baseband_config.rate                    = config.rate;
    baseband_config.nof_tx_ports            = config.nof_tx_ports;
    baseband_config.nof_slot_tti_in_advance = config.nof_slot_tti_in_advance;
    baseband_config.logger                  = config.logger;

    return std::make_unique<downlink_processor_impl>(
        pdxch_proc_factory->create(pdxch_proc_config), amplitude_control_factory->create(), baseband_config);
  }

private:
  std::shared_ptr<pdxch_processor_factory>      pdxch_proc_factory;
  std::shared_ptr<amplitude_controller_factory> amplitude_control_factory;
};

} // namespace

std::shared_ptr<lower_phy_downlink_processor_factory>
srsran::create_downlink_processor_factory_sw(std::shared_ptr<pdxch_processor_factory>      pdxch_proc_factory,
                                             std::shared_ptr<amplitude_controller_factory> amplitude_control_factory)
{
  return std::make_shared<lower_phy_downlink_processor_factory_sw>(std::move(pdxch_proc_factory),
                                                                   std::move(amplitude_control_factory));
}