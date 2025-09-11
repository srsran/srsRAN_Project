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

#pragma once

#include "../../resource_request_pool.h"
#include "pdxch_baseband_modulator.h"
#include "pdxch_processor_modulator_notifier.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/phy/lower/amplitude_controller/amplitude_controller.h"
#include "srsran/phy/lower/modulation/ofdm_modulator.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_notifier.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_request_handler.h"
#include "srsran/phy/lower/processors/lower_phy_center_freq_controller.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// \brief Implements a PDxCH baseband processor.
///
/// Downlink physical channel transmission requests are handled asynchronously by modulators. These notify the
/// completion of their processing via \ref pdxch_processor_modulator_notifier interface.
///
/// The modulation results are stored in a circular buffer that implements an exchange mechanism. The modulation results
/// are taken from the circular buffer when a new slot is processed through the \ref pdxch_processor_baseband interface.
class pdxch_processor_impl : public pdxch_processor,
                             private pdxch_processor_baseband,
                             private pdxch_processor_request_handler,
                             private lower_phy_center_freq_controller,
                             public pdxch_processor_modulator_notifier
{
public:
  /// Collects the necessary parameters and dependencies for creating a physical downlink channel baseband processor
  /// configuration.
  struct configuration {
    subcarrier_spacing scs;
    cyclic_prefix      cp;
    sampling_rate      srate;
    unsigned           nof_tx_ports;
    task_executor&     executor;
  };

  /// Constructs a physical downlink channel baseband processor.
  pdxch_processor_impl(std::unique_ptr<ofdm_symbol_modulator> modulator,
                       std::unique_ptr<amplitude_controller>  amplitude_control,
                       const configuration&                   config) :
    logger(srslog::fetch_basic_logger("PHY")),
    bb_buffers(buffer_request_pool::request_array_size + max_slot_modulation_concurrency + 1,
               config.nof_tx_ports,
               config.srate.to_kHz()),
    common_ofdm_modulator(std::move(modulator)),
    common_amplitude_control(std::move(amplitude_control)),
    modulators(max_slot_modulation_concurrency)
  {
    // Generate baseband modulators.
    std::generate(modulators.begin(), modulators.end(), [this, &config]() {
      return std::make_unique<pdxch_baseband_modulator>(config.scs,
                                                        config.cp,
                                                        config.srate,
                                                        config.nof_tx_ports,
                                                        config.executor,
                                                        *common_ofdm_modulator,
                                                        *common_amplitude_control,
                                                        *this);
    });
  }

  // See pdxch_processor interface for documentation.
  void connect(pdxch_processor_notifier& notifier) override;

  // See pdxch_processor interface for documentation.
  void stop() override
  {
    // Signal the stop. The downlink transmission handler stops accepting transmissions.
    stopped = true;

    // Wait for all modulators to complete their current tasks.
    std::for_each(
        modulators.begin(), modulators.end(), [](const auto& modulator) { modulator->wait_for_completion(); });
  }

  // See pdxch_processor interface for documentation.
  pdxch_processor_request_handler& get_request_handler() override;

  // See pdxch_processor interface for documentation.
  pdxch_processor_baseband& get_baseband() override;

  // See pdxch_processor interface for documentation.
  lower_phy_center_freq_controller& get_center_freq_control() override;

private:
  /// Maximum number of slots that can be concurrently modulated.
  static constexpr unsigned max_slot_modulation_concurrency = 4;

  /// Buffer request pool alias.
  using buffer_request_pool = resource_request_pool<pdxch_processor_baseband::slot_result>;

  /// Retrieves the PDxCH modulator associated to the given slot.
  pdxch_baseband_modulator& get_modulator(slot_point slot)
  {
    return *modulators[slot.system_slot() % modulators.size()];
  }

  // See pdxch_processor_baseband interface for documentation.
  pdxch_processor_baseband::slot_result process_slot(slot_context context) override;

  // See pdxch_processor_request_handler interface for documentation.
  void handle_request(const shared_resource_grid& grid, const resource_grid_context& context) override;

  // See lower_phy_center_freq_controller interface for documentation.
  bool set_carrier_center_frequency(double carrier_center_frequency_Hz) override;

  // See pdxch_processor_modulator_notifier interface for documentation.
  void on_modulation_completion(pdxch_processor_baseband::slot_result result, resource_grid_context context) override;

  /// Physical layer logger. Used for logging errors when the number of buffers are exhausted or a modulator is busy.
  srslog::basic_logger& logger;
  /// Set to true when upper layers request to stop. The PDxCH processor does not accept more calls
  std::atomic<bool> stopped = false;
  /// Physical channel baseband processor event notifier.
  pdxch_processor_notifier* notifier = nullptr;
  /// Baseband buffer pool.
  baseband_gateway_buffer_pool bb_buffers;
  /// Common OFDM symbol modulators. It must be thread-safe.
  std::unique_ptr<ofdm_symbol_modulator> common_ofdm_modulator;
  /// Common amplitude controller. It must be thread-safe.
  std::unique_ptr<amplitude_controller> common_amplitude_control;
  /// Downlink physical channel baseband modulators.
  std::vector<std::unique_ptr<pdxch_baseband_modulator>> modulators;
  /// Circular concurrent pool of transmit requests.
  buffer_request_pool requests;
};

} // namespace srsran
