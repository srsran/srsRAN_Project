/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

  std::unique_ptr<srsran::lower_phy> create(lower_phy_configuration& config) override
  {
    srsran_assert((config.dft_window_offset >= 0.0) && (config.dft_window_offset < 1.0F),
                  "DFT window offset if out-of-range");
    srsran_assert(config.sectors.size() == 1, "Only one sector is currently supported.");
    srsran_assert(config.srate.to_Hz() > 0, "Invalid sampling rate.");

    const lower_phy_sector_description& sector = config.sectors.front();

    srsran_assert(config.scs < subcarrier_spacing::kHz60,
                  "Subcarrier spacing of {} is not supported. Only {} and {} are supported.",
                  to_string(config.scs),
                  to_string(subcarrier_spacing::kHz15),
                  to_string(subcarrier_spacing::kHz30));
    unsigned nof_samples_per_slot = config.srate.to_kHz() / pow2(to_numerology_value(config.scs));

    unsigned tx_buffer_size = config.bb_gateway->get_transmitter_optimal_buffer_size();
    switch (config.baseband_tx_buffer_size_policy) {
      case lower_phy_baseband_buffer_size_policy::slot:
        tx_buffer_size = nof_samples_per_slot;
        break;
      case lower_phy_baseband_buffer_size_policy::half_slot:
        tx_buffer_size = nof_samples_per_slot / 2;
        break;
      case lower_phy_baseband_buffer_size_policy::single_packet:
        report_fatal_error_if_not(tx_buffer_size > 0, "The radio does not have a transmitter optimal buffer size.");
        break;
      case lower_phy_baseband_buffer_size_policy::optimal_slot:
        report_fatal_error_if_not(tx_buffer_size > 0, "The radio does not have a transmitter optimal buffer size.");
        tx_buffer_size = (nof_samples_per_slot / tx_buffer_size) * tx_buffer_size;
        break;
    }

    unsigned rx_buffer_size = config.bb_gateway->get_receiver_optimal_buffer_size();
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
    downlink_processor_configuration dl_proc_config;
    dl_proc_config.sector_id               = config.sector_id;
    dl_proc_config.scs                     = config.scs;
    dl_proc_config.cp                      = config.cp;
    dl_proc_config.rate                    = config.srate;
    dl_proc_config.bandwidth_prb           = sector.bandwidth_rb;
    dl_proc_config.center_frequency_Hz     = sector.dl_freq_hz;
    dl_proc_config.nof_tx_ports            = sector.nof_tx_ports;
    dl_proc_config.nof_slot_tti_in_advance = config.max_processing_delay_slots;
    dl_proc_config.logger                  = config.logger;

    // Create downlink processor.
    std::unique_ptr<lower_phy_downlink_processor> dl_proc = downlink_proc_factory->create(dl_proc_config);
    srsran_assert(dl_proc, "Failed to create DL processor.");

    // Prepare uplink processor configuration.
    uplink_processor_configuration ul_proc_config;
    ul_proc_config.sector_id           = config.sector_id;
    ul_proc_config.scs                 = config.scs;
    ul_proc_config.cp                  = config.cp;
    ul_proc_config.rate                = config.srate;
    ul_proc_config.bandwidth_prb       = sector.bandwidth_rb;
    ul_proc_config.center_frequency_Hz = sector.ul_freq_hz;
    ul_proc_config.nof_rx_ports        = sector.nof_rx_ports;

    // Create uplink processor.
    std::unique_ptr<lower_phy_uplink_processor> ul_proc = uplink_proc_factory->create(ul_proc_config);
    srsran_assert(dl_proc, "Failed to create the UL processor.");

    // Prepare processor baseband adaptor configuration.
    lower_phy_baseband_processor::configuration proc_bb_adaptor_config;
    proc_bb_adaptor_config.srate                  = config.srate;
    proc_bb_adaptor_config.rx_task_executor       = config.rx_task_executor;
    proc_bb_adaptor_config.tx_task_executor       = config.tx_task_executor;
    proc_bb_adaptor_config.ul_task_executor       = config.ul_task_executor;
    proc_bb_adaptor_config.dl_task_executor       = config.dl_task_executor;
    proc_bb_adaptor_config.receiver               = &config.bb_gateway->get_receiver();
    proc_bb_adaptor_config.transmitter            = &config.bb_gateway->get_transmitter();
    proc_bb_adaptor_config.ul_bb_proc             = &ul_proc->get_baseband();
    proc_bb_adaptor_config.dl_bb_proc             = &dl_proc->get_baseband();
    proc_bb_adaptor_config.nof_tx_ports           = config.sectors.back().nof_tx_ports;
    proc_bb_adaptor_config.nof_rx_ports           = config.sectors.back().nof_rx_ports;
    proc_bb_adaptor_config.tx_time_offset         = tx_time_offset;
    proc_bb_adaptor_config.rx_to_tx_max_delay     = config.srate.to_kHz() + proc_bb_adaptor_config.tx_time_offset;
    proc_bb_adaptor_config.rx_buffer_size         = rx_buffer_size;
    proc_bb_adaptor_config.nof_rx_buffers         = std::max(4U, rx_to_tx_max_delay / rx_buffer_size);
    proc_bb_adaptor_config.tx_buffer_size         = tx_buffer_size;
    proc_bb_adaptor_config.nof_tx_buffers         = std::max(4U, rx_to_tx_max_delay / tx_buffer_size);
    proc_bb_adaptor_config.system_time_throttling = config.system_time_throttling;

    // Create lower PHY controller from the processor baseband adaptor.
    std::unique_ptr<lower_phy_controller> controller =
        std::make_unique<lower_phy_baseband_processor>(proc_bb_adaptor_config);
    srsran_assert(controller, "Failed to create the lower PHY controller.");

    // Prepare lower PHY configuration.
    lower_phy_impl::configuration lower_phy_config;
    lower_phy_config.downlink_proc      = std::move(dl_proc);
    lower_phy_config.uplink_proc        = std::move(ul_proc);
    lower_phy_config.controller         = std::move(controller);
    lower_phy_config.rx_symbol_notifier = config.rx_symbol_notifier;
    lower_phy_config.timing_notifier    = config.timing_notifier;
    lower_phy_config.error_notifier     = config.error_notifier;

    return std::make_unique<lower_phy_impl>(lower_phy_config);
  }

private:
  std::shared_ptr<lower_phy_downlink_processor_factory> downlink_proc_factory;
  std::shared_ptr<lower_phy_uplink_processor_factory>   uplink_proc_factory;
};

std::shared_ptr<lower_phy_factory>
srsran::create_lower_phy_factory_sw(std::shared_ptr<lower_phy_downlink_processor_factory> downlink_proc_factory,
                                    std::shared_ptr<lower_phy_uplink_processor_factory>   uplink_proc_factory)
{
  return std::make_shared<lower_phy_factory_sw>(std::move(downlink_proc_factory), std::move(uplink_proc_factory));
}
