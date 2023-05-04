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

/// \brief Calculates the reception-to-transmission (Rx-to-Tx) delay as a number of samples.
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
/// because of the delays introduced by the underlying radio device. The Rx-to-Tx delay is thus the number of
/// samples by which the sequence of transmitted samples must precede that of received samples, as seen by the lower
/// PHY processor, in order to achieve the aforementioned carrier alignment between uplink and downlink frames.
///
/// More specifically, let \f$N_{\textup{offset}}\f$ be the "round-trip" delay introduced by the radio device
/// (i.e., it is the sum of the delay experienced by a received sample before reaching the lower PHY plus the delay
/// undergone by a downlink sample leaving the lower PHY, before transmission). Also, let \f$N_{\textup{TA,
/// offset}}\f$ be the time-advance offset between downlink and uplink, as defined in TS38.211 Section 4.3.1. Finally,
/// let \f$N_{\textup{cal}}\f$ be a calibration term that takes into account time-advance impairments due to radio
/// device buffering. All three offsets are expressed as a number of samples. Then, the Rx-to-Tx delay \f$N\f$ is
/// given by \f[ N=N_{\textup{offset}} - N_{\textup{cal}} + N_{\textup{TA, offset}} \f] samples or, equivalently,
/// physical layer units of time.
///
/// \param[in] ul_to_dl_subframe_offset   UL-to-DL offset in subframes (recall, 1 subframe = 1ms). It maps to
///                                       \f$N_{\textup{offset}}\f$.
/// \param[in] time_alignment_calibration Time-advance radio calibration time, in number of samples. It
///                                       maps to \f$N_{\textup{cal}}\f$. Positive values cause a reduction of the RF
///                                       transmission delay with respect to the RF reception, while negative values
///                                       increase it.
/// \param[in] ta_offset                  Time advance offset (see \ref lower_phy_ta_offset for more information). It
///                                       maps to \f$N_{\textup{TA, offset}}\f$.
/// \param[in] srate                      Sampling rate.
/// \return The reception-to-transmission delay as a number of samples.
static unsigned get_rx_to_tx_delay(unsigned      ul_to_dl_subframe_offset,
                                   int           time_alignment_calibration,
                                   n_ta_offset   ta_offset,
                                   sampling_rate srate)
{
  // Calculate time between the UL signal reception and the transmission.
  phy_time_unit ul_to_dl_delay = phy_time_unit::from_seconds(0.001 * static_cast<double>(ul_to_dl_subframe_offset));

  // UL to DL delay in number of samples.
  unsigned ul_to_dl_delay_samples = ul_to_dl_delay.to_samples(srate.to_Hz());

  // Apply time alingment calibration.
  ul_to_dl_delay_samples -= time_alignment_calibration;

  // Apply the time alignment offset.
  phy_time_unit time_alignment_offset = phy_time_unit::from_units_of_Tc(static_cast<unsigned>(ta_offset));

  ul_to_dl_delay_samples += time_alignment_offset.to_samples(srate.to_Hz());

  return ul_to_dl_delay_samples;
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

    const lower_phy_sector_description& sector = config.sectors.front();

    // Set the uplink processor initial slot to zero.
    unsigned ul_init_slot_idx = 0;

    // Calculate the downlink processor initial slot.
    unsigned dl_init_slot_idx =
        ul_init_slot_idx + config.ul_to_dl_subframe_offset * get_nof_slots_per_subframe(config.scs);

    // Prepare downlink processor configuration.
    downlink_processor_configuration dl_proc_config;
    dl_proc_config.sector_id               = 0;
    dl_proc_config.scs                     = config.scs;
    dl_proc_config.cp                      = config.cp;
    dl_proc_config.rate                    = config.srate;
    dl_proc_config.bandwidth_prb           = sector.bandwidth_rb;
    dl_proc_config.center_frequency_Hz     = sector.dl_freq_hz;
    dl_proc_config.nof_tx_ports            = sector.nof_tx_ports;
    dl_proc_config.initial_slot_index      = dl_init_slot_idx;
    dl_proc_config.nof_slot_tti_in_advance = config.max_processing_delay_slots;

    // Create downlink processor.
    std::unique_ptr<lower_phy_downlink_processor> dl_proc = downlink_proc_factory->create(dl_proc_config);
    srsran_assert(dl_proc, "Failed to create DL processor.");

    // Prepare uplink processor configuration.
    uplink_processor_configuration ul_proc_config;
    ul_proc_config.sector_id           = 0;
    ul_proc_config.scs                 = config.scs;
    ul_proc_config.cp                  = config.cp;
    ul_proc_config.rate                = config.srate;
    ul_proc_config.bandwidth_prb       = sector.bandwidth_rb;
    ul_proc_config.center_frequency_Hz = sector.ul_freq_hz;
    ul_proc_config.nof_rx_ports        = sector.nof_rx_ports;
    ul_proc_config.initial_slot_index  = ul_init_slot_idx;

    // Create uplink processor.
    std::unique_ptr<lower_phy_uplink_processor> ul_proc = uplink_proc_factory->create(ul_proc_config);
    srsran_assert(dl_proc, "Failed to create the UL processor.");

    // Prepare processor baseband adaptor configuration.
    lower_phy_baseband_processor::configuration proc_bb_adaptor_config;
    proc_bb_adaptor_config.rx_task_executor = config.rx_task_executor;
    proc_bb_adaptor_config.tx_task_executor = config.tx_task_executor;
    proc_bb_adaptor_config.ul_task_executor = config.ul_task_executor;
    proc_bb_adaptor_config.dl_task_executor = config.dl_task_executor;
    proc_bb_adaptor_config.receiver         = &config.bb_gateway->get_receiver(0);
    proc_bb_adaptor_config.transmitter      = &config.bb_gateway->get_transmitter(0);
    proc_bb_adaptor_config.ul_bb_proc       = &ul_proc->get_baseband();
    proc_bb_adaptor_config.dl_bb_proc       = &dl_proc->get_baseband();
    proc_bb_adaptor_config.nof_tx_ports     = config.sectors.back().nof_tx_ports;
    proc_bb_adaptor_config.nof_rx_ports     = config.sectors.back().nof_rx_ports;
    proc_bb_adaptor_config.rx_to_tx_delay   = get_rx_to_tx_delay(
        config.ul_to_dl_subframe_offset, config.time_alignment_calibration, config.ta_offset, config.srate);

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
