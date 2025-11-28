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

#include "srsran/phy/lower/lower_phy_factory.h"
#include "lower_phy_impl.h"

using namespace srsran;

/// \brief Calculates the transmit time offset as a number of samples.
///
/// On the radio channel, uplink and downlink frames coexist on the same carrier. As explained in TS38.211
/// Section 4.3.1, the sequence of uplink frames precedes the sequence of downlink frames by a given time
/// \f$T_{\textup{offset}} = N_{\textup{TA,offset}} \cdot T_c\f$ (recall that TS38.211 Section 4.3.1 describes the
/// issue from the UE standpoint, hence the extra term \f$N_{\textup{TA}} \cdot T_c\f$). In other words, uplink frame
/// \f$i\f$ starts \f$T_{\textup{offset}}\f$ units of time before the downlink frame with the same index \f$i\f$.
/// Equivalently, one can index the OFDM symbols comprising the frames and draw the same timing relationship between
/// symbols with the same index. By increasing the granularity even further, the alignment can be thought of as if at
/// sample level.
///
/// On the other hand, for an uplink and a downlink sample to coincide on the carrier at the same time, it is required
/// that the lower PHY processes the downlink (transmit) sample in advance with respect to the uplink (receive) one,
/// because of the delays introduced by the underlying radio device. The transmit time offset is thus the number of
/// samples by which the sequence of transmitted samples must precede the reception time instant, as seen by the lower
/// PHY processor, in order to achieve the aforementioned carrier alignment between uplink and downlink frames.
///
/// More specifically, let \f$N_{\textup{TA, offset}}\f$ be the time-advance offset between downlink and uplink, as
/// defined in TS38.211 Section 4.3.1. Also, let \f$N_{\textup{cal}}\f$ be a calibration term that takes into account
/// time-advance impairments due to radio device buffering. Both offsets are expressed as a number of samples.
/// Then, the transmit time offset \f$N\f$ is given by \f[ N= N_{\textup{TA, offset}} - N_{\textup{cal}} \f] samples or,
/// equivalently, physical layer units of time.
///
/// \param[in] time_alignment_calibration Time-advance radio calibration time, in number of samples. It
///                                       maps to \f$N_{\textup{cal}}\f$. Positive values cause a reduction of the RF
///                                       transmission delay with respect to the RF reception, while negative values
///                                       increase it.
/// \param[in] ta_offset                  Time advance offset (see \ref lower_phy_ta_offset for more information). It
///                                       maps to \f$N_{\textup{TA, offset}}\f$.
/// \param[in] srate                      Sampling rate.
/// \return The transmit time offset as a number of samples.
static int get_tx_time_offset(int time_alignment_calibration, n_ta_offset ta_offset, sampling_rate srate)
{
  // Calculate the time alignment offset.
  phy_time_unit time_alignment_offset = phy_time_unit::from_units_of_Tc(static_cast<unsigned>(ta_offset));

  // Convert to samples and apply time alignment calibration.
  return time_alignment_offset.to_samples(srate.to_Hz()) - time_alignment_calibration;
}

namespace {

class lower_phy_factory_sw : public lower_phy_factory
{
public:
  lower_phy_factory_sw(std::shared_ptr<lower_phy_downlink_processor_factory> downlink_proc_factory_,
                       std::shared_ptr<lower_phy_uplink_processor_factory>   uplink_proc_factory_) :
    downlink_proc_factory(std::move(downlink_proc_factory_)), uplink_proc_factory(std::move(uplink_proc_factory_))
  {
    srsran_assert(downlink_proc_factory, "Invalid downlink processor factory.");
    srsran_assert(uplink_proc_factory, "Invalid uplink processor factory.");
  }

  std::unique_ptr<srsran::lower_phy> create(const lower_phy_configuration& config,
                                            const lower_phy_dependencies&  dependencies) override
  {
    srsran_assert((config.dft_window_offset >= 0.0) && (config.dft_window_offset < 1.0F),
                  "DFT window offset if out-of-range");
    srsran_assert(config.srate.to_Hz() > 0, "Invalid sampling rate.");

    srsran_assert((config.scs == subcarrier_spacing::kHz15) || (config.scs == subcarrier_spacing::kHz30) ||
                      (config.scs == subcarrier_spacing::kHz120),
                  "Subcarrier spacing of {} is not supported. Only {}, {} and {} are supported.",
                  to_string(config.scs),
                  to_string(subcarrier_spacing::kHz15),
                  to_string(subcarrier_spacing::kHz30),
                  to_string(subcarrier_spacing::kHz120));
    unsigned nof_samples_per_slot = config.srate.to_kHz() / pow2(to_numerology_value(config.scs));

    unsigned rx_buffer_size = dependencies.bb_gateway.get_receiver_optimal_buffer_size();
    switch (config.baseband_rx_buffer_size_policy) {
      case lower_phy_baseband_buffer_size_policy::slot:
        rx_buffer_size = nof_samples_per_slot;
        break;
      case lower_phy_baseband_buffer_size_policy::half_slot:
        rx_buffer_size = nof_samples_per_slot / 2;
        break;
      case lower_phy_baseband_buffer_size_policy::single_packet:
        report_fatal_error_if_not(rx_buffer_size > 0, "The radio does not have a transmitter optimal buffer size.");
        break;
      case lower_phy_baseband_buffer_size_policy::optimal_slot:
        report_fatal_error_if_not(rx_buffer_size > 0, "The radio does not have a transmitter optimal buffer size.");
        rx_buffer_size = (nof_samples_per_slot / rx_buffer_size) * rx_buffer_size;
        break;
    }

    // Get transmit time offset between the UL and the DL.
    int tx_time_offset = get_tx_time_offset(config.time_alignment_calibration, config.ta_offset, config.srate);

    // Maximum time delay between reception and transmission in samples (1ms plus the time offset).
    unsigned rx_to_tx_max_delay = config.srate.to_kHz() + tx_time_offset;

    // Prepare downlink processor configuration.
    downlink_processor_configuration dl_proc_config = {.sector_id               = config.sector_id,
                                                       .scs                     = config.scs,
                                                       .cp                      = config.cp,
                                                       .rate                    = config.srate,
                                                       .bandwidth_prb           = config.bandwidth_rb,
                                                       .center_frequency_Hz     = config.dl_freq_hz,
                                                       .nof_tx_ports            = config.nof_tx_ports,
                                                       .nof_slot_tti_in_advance = config.max_processing_delay_slots};

    // Create downlink processor.
    std::unique_ptr<lower_phy_downlink_processor> dl_proc =
        downlink_proc_factory->create(dl_proc_config, dependencies.dl_task_executor);
    srsran_assert(dl_proc, "Failed to create DL processor.");

    // Prepare uplink processor configuration.
    uplink_processor_configuration ul_proc_config = {.sector_id           = config.sector_id,
                                                     .scs                 = config.scs,
                                                     .cp                  = config.cp,
                                                     .rate                = config.srate,
                                                     .bandwidth_prb       = config.bandwidth_rb,
                                                     .center_frequency_Hz = config.ul_freq_hz,
                                                     .nof_rx_ports        = config.nof_rx_ports};

    // Create uplink processor.
    std::unique_ptr<lower_phy_uplink_processor> ul_proc = uplink_proc_factory->create(ul_proc_config);
    srsran_assert(dl_proc, "Failed to create the UL processor.");

    // Prepare processor baseband adaptor configuration.
    lower_phy_baseband_processor_configuration proc_bb_adaptor_config = {
        .srate                  = config.srate,
        .scs                    = config.scs,
        .nof_tx_ports           = config.nof_tx_ports,
        .nof_rx_ports           = config.nof_rx_ports,
        .tx_time_offset         = static_cast<baseband_gateway_timestamp>(tx_time_offset),
        .rx_to_tx_max_delay     = config.srate.to_kHz() + proc_bb_adaptor_config.tx_time_offset,
        .rx_buffer_size         = rx_buffer_size,
        .nof_rx_buffers         = std::max(4U, rx_to_tx_max_delay / rx_buffer_size),
        .system_time_throttling = config.system_time_throttling,
        .stop_nof_slots         = 2 * config.max_processing_delay_slots};

    // Prepare processor baseband adaptor dependencies.
    lower_phy_baseband_processor_dependencies proc_bb_adaptor_deps = {
        .rx_task_executor = dependencies.rx_task_executor,
        .tx_task_executor = dependencies.tx_task_executor,
        .ul_task_executor = dependencies.ul_task_executor,
        .receiver         = dependencies.bb_gateway.get_receiver(),
        .transmitter      = dependencies.bb_gateway.get_transmitter(),
        .ul_bb_proc       = ul_proc->get_baseband(),
        .dl_bb_proc       = dl_proc->get_baseband(),
    };

    // Create lower PHY controller from the processor baseband adaptor.
    std::unique_ptr<lower_phy_controller> controller =
        std::make_unique<lower_phy_baseband_processor>(proc_bb_adaptor_config, proc_bb_adaptor_deps);
    srsran_assert(controller, "Failed to create the lower PHY controller.");

    // Prepare lower PHY dependencies.
    lower_phy_impl::dependencies lower_phy_deps = {.downlink_proc      = std::move(dl_proc),
                                                   .uplink_proc        = std::move(ul_proc),
                                                   .controller         = std::move(controller),
                                                   .rx_symbol_notifier = dependencies.rx_symbol_notifier,
                                                   .timing_notifier    = dependencies.timing_notifier,
                                                   .error_notifier     = dependencies.error_notifier,
                                                   .metrics_notifier   = dependencies.metric_notifier};

    return std::make_unique<lower_phy_impl>(std::move(lower_phy_deps));
  }

private:
  std::shared_ptr<lower_phy_downlink_processor_factory> downlink_proc_factory;
  std::shared_ptr<lower_phy_uplink_processor_factory>   uplink_proc_factory;
};

} // namespace

std::shared_ptr<lower_phy_factory>
srsran::create_lower_phy_factory_sw(std::shared_ptr<lower_phy_downlink_processor_factory> downlink_proc_factory,
                                    std::shared_ptr<lower_phy_uplink_processor_factory>   uplink_proc_factory)
{
  return std::make_shared<lower_phy_factory_sw>(std::move(downlink_proc_factory), std::move(uplink_proc_factory));
}
