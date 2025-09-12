/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../resource_request_pool.h"
#include "srsran/adt/circular_array.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/phy/lower/modulation/ofdm_demodulator.h"
#include "srsran/phy/lower/processors/lower_phy_center_freq_controller.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_notifier.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_request_handler.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// Implements PUxCH baseband processor.
class puxch_processor_impl : public puxch_processor,
                             private puxch_processor_baseband,
                             private lower_phy_center_freq_controller,
                             private puxch_processor_request_handler
{
public:
  struct configuration {
    cyclic_prefix cp;
    unsigned      nof_rx_ports;
    unsigned      request_queue_size;
    unsigned      dft_size;
  };

  puxch_processor_impl(std::unique_ptr<ofdm_symbol_demodulator> demodulator_, const configuration& config) :
    nof_symbols_per_slot(get_nsymb_per_slot(config.cp)),
    nof_rx_ports(config.nof_rx_ports),
    demodulator(std::move(demodulator_)),
    cf_buffer({2 * config.dft_size, nof_rx_ports})
  {
    srsran_assert(demodulator, "Invalid demodulator.");
  }

  // See interface for documentation.
  void connect(puxch_processor_notifier& notifier_) override { notifier = &notifier_; }

  // See interface for documentation.
  void stop() override { stopped = true; }

  // See interface for documentation.
  puxch_processor_request_handler& get_request_handler() override { return *this; }

  // See interface for documentation.
  puxch_processor_baseband& get_baseband() override { return *this; }

  // See interface for documentation.
  lower_phy_center_freq_controller& get_center_freq_control() override;

private:
  // See interface for documentation.
  bool process_symbol(const baseband_gateway_buffer_reader& samples,
                      const lower_phy_rx_symbol_context&    context) override;

  // See interface for documentation.
  void handle_request(const shared_resource_grid& grid, const resource_grid_context& context) override;

  // See interface for documentation.
  bool set_carrier_center_frequency(double carrier_center_frequency_Hz) override;

  /// Scaling factor for converting from 16-bit complex integer to complex float.
  static constexpr float scaling_factor_ci16_to_cf = std::numeric_limits<int16_t>::max();

  std::atomic<bool>                           stopped = false;
  unsigned                                    nof_symbols_per_slot;
  unsigned                                    nof_rx_ports;
  puxch_processor_notifier*                   notifier = nullptr;
  std::unique_ptr<ofdm_symbol_demodulator>    demodulator;
  slot_point                                  current_slot;
  shared_resource_grid                        current_grid;
  resource_request_pool<shared_resource_grid> requests;
  /// \brief Temporary intermediate buffer to hold a symbol's complex-based floating-point samples for demodulation.
  ///
  /// Set to twice the DFT size to ensure that it can accommodate the maximum number of samples for demodulating an OFDM
  /// symbol.
  dynamic_tensor<2, cf_t> cf_buffer;
};

} // namespace srsran
