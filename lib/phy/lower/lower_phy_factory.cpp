/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/lower/lower_phy_factory.h"
#include "lower_phy_impl.h"

using namespace srsran;

class lower_phy_factory_generic : public lower_phy_factory
{
public:
  lower_phy_factory_generic(std::shared_ptr<ofdm_modulator_factory>&            modulator_factory_,
                            std::shared_ptr<lower_phy_uplink_processor_factory> uplink_proc_factory_,
                            std::shared_ptr<amplitude_controller_factory>&      amplitude_control_factory_) :
    modulator_factory(modulator_factory_),
    uplink_proc_factory(uplink_proc_factory_),
    amplitude_control_factory(amplitude_control_factory_)
  {
    srsran_assert(modulator_factory, "Invalid modulator factory.");
    srsran_assert(uplink_proc_factory, "Invalid uplink processor factory.");
    srsran_assert(amplitude_control_factory, "Invalid amplitude controller factory");
  }

  std::unique_ptr<srsran::lower_phy> create(lower_phy_configuration& config) override
  {
    srsran_assert((config.dft_window_offset >= 0.0) && (config.dft_window_offset < 1.0F),
                  "DFT window offset if out-of-range");
    srsran_assert(config.sectors.size() == 1, "Only one sector is currently supported.");

    const lower_phy_sector_description& sector = config.sectors.front();

    // Prepare sector modulator.
    ofdm_modulator_configuration ofdm_modulator_config = {};
    ofdm_modulator_config.numerology                   = to_numerology_value(config.scs);
    ofdm_modulator_config.bw_rb                        = sector.bandwidth_rb;
    ofdm_modulator_config.dft_size                     = config.srate.get_dft_size(config.scs);
    ofdm_modulator_config.cp                           = config.cp;
    ofdm_modulator_config.scale                        = config.tx_scale;
    ofdm_modulator_config.center_freq_hz               = sector.dl_freq_hz;

    // Prepare sector demodulator.
    uplink_processor_configuration ul_proc_config;
    ul_proc_config.sector_id           = 0;
    ul_proc_config.scs                 = config.scs;
    ul_proc_config.cp                  = config.cp;
    ul_proc_config.rate                = config.srate;
    ul_proc_config.bandwidth_prb       = sector.bandwidth_rb;
    ul_proc_config.center_frequency_Hz = sector.ul_freq_hz;
    ul_proc_config.nof_rx_ports        = sector.port_mapping.size();
    ul_proc_config.initial_slot_index  = 0;

    lower_phy_impl::sector_processors processors;
    processors.ofdm_modulator    = modulator_factory->create_ofdm_symbol_modulator(ofdm_modulator_config);
    processors.uplink_proc       = uplink_proc_factory->create(ul_proc_config);
    processors.amplitude_control = amplitude_control_factory->create_amplitude_controller();

    return std::make_unique<lower_phy_impl>(std::move(processors), config);
  }

private:
  std::shared_ptr<ofdm_modulator_factory>             modulator_factory;
  std::shared_ptr<lower_phy_uplink_processor_factory> uplink_proc_factory;
  std::shared_ptr<amplitude_controller_factory>       amplitude_control_factory;
};

std::unique_ptr<lower_phy_factory>
srsran::create_lower_phy_factory_sw(std::shared_ptr<ofdm_modulator_factory>             modulator_factory,
                                    std::shared_ptr<lower_phy_uplink_processor_factory> uplink_proc_factory,
                                    std::shared_ptr<amplitude_controller_factory>       amplitude_control_factory)
{
  return std::make_unique<lower_phy_factory_generic>(modulator_factory, uplink_proc_factory, amplitude_control_factory);
}
