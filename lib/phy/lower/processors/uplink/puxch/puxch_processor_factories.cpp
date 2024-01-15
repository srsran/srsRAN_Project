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

#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_factories.h"
#include "puxch_processor_impl.h"

using namespace srsran;

namespace {
class puxch_processor_factory_sw : public puxch_processor_factory
{
public:
  puxch_processor_factory_sw(unsigned request_queue_size_, std::shared_ptr<ofdm_demodulator_factory> ofdm_factory_) :
    request_queue_size(request_queue_size_), ofdm_factory(std::move(ofdm_factory_))
  {
    srsran_assert(request_queue_size > 0, "Invalid queue size.");
    srsran_assert(ofdm_factory, "Invalid demodulator factory.");
  }

  std::unique_ptr<puxch_processor> create(const puxch_processor_configuration& config) override
  {
    // Calculate the DFT window offset in samples.
    unsigned nof_samples_window_offset =
        static_cast<unsigned>(static_cast<float>(config.cp.get_length(1, config.scs).to_samples(config.srate.to_Hz())) *
                              config.dft_window_offset);

    // Prepare OFDM demodulator configuration.
    ofdm_demodulator_configuration demodulator_config;
    demodulator_config.numerology                = to_numerology_value(config.scs);
    demodulator_config.bw_rb                     = config.bandwidth_rb;
    demodulator_config.dft_size                  = config.srate.get_dft_size(config.scs);
    demodulator_config.cp                        = config.cp;
    demodulator_config.nof_samples_window_offset = nof_samples_window_offset;
    demodulator_config.scale                     = 1.0F;
    demodulator_config.center_freq_hz            = config.center_freq_Hz;

    // Prepare PUxCH processor configuration.
    puxch_processor_impl::configuration proc_config;
    proc_config.cp                 = config.cp;
    proc_config.nof_rx_ports       = config.nof_rx_ports;
    proc_config.request_queue_size = request_queue_size;

    return std::make_unique<puxch_processor_impl>(ofdm_factory->create_ofdm_symbol_demodulator(demodulator_config),
                                                  proc_config);
  }

private:
  unsigned                                  request_queue_size;
  std::shared_ptr<ofdm_demodulator_factory> ofdm_factory;
};

} // namespace

std::shared_ptr<puxch_processor_factory>
srsran::create_puxch_processor_factory_sw(unsigned                                  request_queue_size,
                                          std::shared_ptr<ofdm_demodulator_factory> ofdm_demod_factory)
{
  return std::make_shared<puxch_processor_factory_sw>(request_queue_size, std::move(ofdm_demod_factory));
}
