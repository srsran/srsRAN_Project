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
  pdxch_processor_factory_sw(unsigned request_queue_size_, std::shared_ptr<ofdm_modulator_factory> ofdm_mod_factory_) :
    request_queue_size(request_queue_size_), ofdm_mod_factory(std::move(ofdm_mod_factory_))
  {
    srsran_assert(request_queue_size != 0, "Requested queue size must be greater than zero.");
    srsran_assert(ofdm_mod_factory, "Invalid OFDM modulator factory.");
  }

  std::unique_ptr<pdxch_processor> create(const pdxch_processor_configuration& config) override
  {
    ofdm_modulator_configuration mod_config;
    mod_config.numerology     = to_numerology_value(config.scs);
    mod_config.bw_rb          = config.bandwidth_rb;
    mod_config.dft_size       = config.srate.get_dft_size(config.scs);
    mod_config.cp             = config.cp;
    mod_config.scale          = modulator_scaling;
    mod_config.center_freq_hz = config.center_freq_Hz;

    pdxch_processor_impl::configuration pdxch_config;
    pdxch_config.cp                 = config.cp;
    pdxch_config.nof_tx_ports       = config.nof_tx_ports;
    pdxch_config.request_queue_size = request_queue_size;
    return std::make_unique<pdxch_processor_impl>(ofdm_mod_factory->create_ofdm_symbol_modulator(mod_config),
                                                  pdxch_config);
  }

private:
  static constexpr float                  modulator_scaling = 1.0F;
  unsigned                                request_queue_size;
  std::shared_ptr<ofdm_modulator_factory> ofdm_mod_factory;
};

} // namespace

std::shared_ptr<pdxch_processor_factory>
srsran::create_pdxch_processor_factory_sw(unsigned                                request_queue_size,
                                          std::shared_ptr<ofdm_modulator_factory> ofdm_mod_factory)
{
  return std::make_shared<pdxch_processor_factory_sw>(request_queue_size, std::move(ofdm_mod_factory));
}