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

#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_factories.h"
#include "pdxch_processor_impl.h"

using namespace srsran;

namespace {

class pdxch_processor_factory_sw : public pdxch_processor_factory
{
public:
  pdxch_processor_factory_sw(std::shared_ptr<ofdm_modulator_factory>       ofdm_mod_factory_,
                             std::shared_ptr<amplitude_controller_factory> amplitude_control_factory_) :
    ofdm_mod_factory(std::move(ofdm_mod_factory_)), amplitude_control_factory(std::move(amplitude_control_factory_))
  {
    srsran_assert(ofdm_mod_factory, "Invalid OFDM modulator factory.");
    srsran_assert(amplitude_control_factory, "Invalid amplitude control factory.");
  }

  std::unique_ptr<pdxch_processor> create(const pdxch_processor_configuration& config) override
  {
    ofdm_modulator_configuration mod_config;
    mod_config.numerology     = to_numerology_value(config.scs);
    mod_config.bw_rb          = config.bandwidth_rb;
    mod_config.dft_size       = config.srate.get_dft_size(config.scs);
    mod_config.cp             = config.cp;
    mod_config.scale          = modulator_scaling;
    mod_config.center_freq_Hz = config.center_freq_Hz;

    pdxch_processor_impl::configuration pdxch_config = {.scs          = config.scs,
                                                        .cp           = config.cp,
                                                        .srate        = config.srate,
                                                        .nof_tx_ports = config.nof_tx_ports,
                                                        .executor     = config.modulation_executor};

    return std::make_unique<pdxch_processor_impl>(
        ofdm_mod_factory->create_ofdm_symbol_modulator(mod_config), amplitude_control_factory->create(), pdxch_config);
  }

private:
  static constexpr float                        modulator_scaling = 1.0F;
  std::shared_ptr<ofdm_modulator_factory>       ofdm_mod_factory;
  std::shared_ptr<amplitude_controller_factory> amplitude_control_factory;
};

} // namespace

std::shared_ptr<pdxch_processor_factory>
srsran::create_pdxch_processor_factory_sw(std::shared_ptr<ofdm_modulator_factory>       ofdm_mod_factory,
                                          std::shared_ptr<amplitude_controller_factory> amplitude_control_factory)
{
  return std::make_shared<pdxch_processor_factory_sw>(std::move(ofdm_mod_factory),
                                                      std::move(amplitude_control_factory));
}
