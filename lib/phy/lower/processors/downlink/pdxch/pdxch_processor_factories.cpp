/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  std::unique_ptr<pdxch_processor> create(const pdxch_processor_configuration& config,
                                          task_executor&                       modulation_executor) override
  {
    ofdm_modulator_configuration mod_config = {.numerology     = to_numerology_value(config.scs),
                                               .bw_rb          = config.bandwidth_rb,
                                               .dft_size       = config.srate.get_dft_size(config.scs),
                                               .cp             = config.cp,
                                               .scale          = modulator_scaling,
                                               .center_freq_Hz = config.center_freq_Hz};

    pdxch_processor_impl::configuration pdxch_config = {
        .scs = config.scs, .cp = config.cp, .srate = config.srate, .nof_tx_ports = config.nof_tx_ports};

    return std::make_unique<pdxch_processor_impl>(ofdm_mod_factory->create_ofdm_symbol_modulator(mod_config),
                                                  amplitude_control_factory->create(),
                                                  modulation_executor,
                                                  pdxch_config);
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
