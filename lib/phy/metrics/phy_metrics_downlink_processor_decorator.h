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

#include "srsran/phy/upper/downlink_processor.h"
#include "srsran/phy/upper/upper_phy_rg_gateway.h"

namespace srsran {

/// Downlink processor metric decorator.
class phy_metrics_downlink_processor_decorator : public downlink_processor_base,
                                                 private downlink_processor_controller,
                                                 private unique_downlink_processor::downlink_processor_callback
{
public:
  /// \brief Internal resource grid gateway adaptor.
  ///
  /// Wraps a base resource grid gateway and provides the phy_metrics_downlink_processor_decorator the necessary methods
  /// to gather the metrics at specific time points.
  class rg_gateway_adaptor : private upper_phy_rg_gateway
  {
  public:
    /// Create the RG gateway adaptor from a base gateway and the DL processor metric notifier.
    rg_gateway_adaptor(upper_phy_rg_gateway& base_, downlink_processor_metric_notifier& notifier_) :
      base(base_), notifier(notifier_)
    {
    }

    /// Notifies the call to configure grid.
    void on_configure_grid(slot_point slot_)
    {
      slot = slot_;
      time_configure.store(std::chrono::high_resolution_clock::now().time_since_epoch().count(),
                           std::memory_order_relaxed);
    }

    /// Notifies the call to finish PDU.
    void on_finish_processing_pdus()
    {
      time_finish_processing_pdu.store(std::chrono::high_resolution_clock::now().time_since_epoch().count(),
                                       std::memory_order_relaxed);
    }

    /// Gets the reference to the gateway notifier.
    upper_phy_rg_gateway& get_gateway() { return *this; }

  private:
    // See upper_phy_rg_gateway interface for documentation.
    void send(const resource_grid_context& context, shared_resource_grid grid) override
    {
      // Get current time point.
      auto end_tp = std::chrono::high_resolution_clock::now();

      // Recover slot configure time point.
      std::chrono::high_resolution_clock::time_point configure_tp = std::chrono::high_resolution_clock::time_point(
          std::chrono::steady_clock::duration(time_configure.load(std::memory_order_relaxed)));

      // Recover finish processing PDU time point
      std::chrono::high_resolution_clock::time_point finish_processing_pdu_tp =
          std::chrono::high_resolution_clock::time_point(
              std::chrono::steady_clock::duration(time_finish_processing_pdu.load(std::memory_order_relaxed)));

      // Notify metrics.
      notifier.on_new_metric({.slot              = slot,
                              .elapsed_configure = end_tp - configure_tp,
                              .elapsed_finish    = end_tp - finish_processing_pdu_tp});

      // Send grid through the gateway.
      base.send(context, std::move(grid));
    }

    upper_phy_rg_gateway&               base;
    downlink_processor_metric_notifier& notifier;
    slot_point                          slot;
    std::atomic<uint64_t>               time_configure;
    std::atomic<uint64_t>               time_finish_processing_pdu;
  };

  /// Constructs the downlink processor metric decorator from a base processor controller and the resource grid gateway
  /// adaptor.
  phy_metrics_downlink_processor_decorator(std::unique_ptr<downlink_processor_base> base_,
                                           std::unique_ptr<rg_gateway_adaptor>      adaptor_) :
    base(std::move(base_)), adaptor(std::move(adaptor_))
  {
    srsran_assert(base, "Invalid base instance.");
    srsran_assert(adaptor, "Invalid RG gateway adaptor.");
  }

  // See downlink_processor_base interface for documentation.
  downlink_processor_controller& get_controller() override { return *this; }

  // See downlink_processor_base interface for documentation.
  void stop() override { base->stop(); }

  // See downlink_processor_controller interface for documentation.
  unique_downlink_processor configure_resource_grid(const resource_grid_context& context,
                                                    shared_resource_grid         grid) override
  {
    // Try to configure underlying downlink processor.
    unique_downlink_processor proc = base->get_controller().configure_resource_grid(context, std::move(grid));

    // Ensure the resulting processor is valid.
    if (!proc.is_valid()) {
      return {};
    }

    // Save underlying unique processor.
    base_processor = std::move(proc);

    // Notify the configuration of the gNb.
    adaptor->on_configure_grid(context.slot);

    // Return a unique downlink processor based on this instance.
    return unique_downlink_processor(*this);
  }

  // See downlink_processor interface for documentation.
  void process_pdcch(const pdcch_processor::pdu_t& pdu) override
  {
    if (base_processor.is_valid()) {
      base_processor->process_pdcch(pdu);
    }
  }

  // See downlink_processor interface for documentation.
  void process_pdsch(static_vector<shared_transport_block, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data,
                     const pdsch_processor::pdu_t&                                                    pdu) override
  {
    if (base_processor.is_valid()) {
      base_processor->process_pdsch(data, pdu);
    }
  }

  // See downlink_processor interface for documentation.
  void process_ssb(const ssb_processor::pdu_t& pdu) override
  {
    if (base_processor.is_valid()) {
      base_processor->process_ssb(pdu);
    }
  }

  // See downlink_processor interface for documentation.
  void process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config) override
  {
    if (base_processor.is_valid()) {
      base_processor->process_nzp_csi_rs(config);
    }
  }

  // See downlink_processor interface for documentation.
  void process_prs(const prs_generator_configuration& config) override
  {
    if (base_processor.is_valid()) {
      base_processor->process_prs(config);
    }
  }

  // See unique_downlink_processor::downlink_processor_callback interface for documentation.
  void finish_processing_pdus() override
  {
    // Notify the end of processing PDUs.
    adaptor->on_finish_processing_pdus();

    // Release underlying processor.
    base_processor.release();
  }

private:
  std::unique_ptr<downlink_processor_base> base;
  std::unique_ptr<rg_gateway_adaptor>      adaptor;
  unique_downlink_processor                base_processor;
};

} // namespace srsran
