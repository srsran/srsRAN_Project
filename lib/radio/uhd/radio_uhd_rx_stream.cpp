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

#include "radio_uhd_rx_stream.h"

using namespace srsran;

bool radio_uhd_rx_stream::receive_block(unsigned&                       nof_rxd_samples,
                                        baseband_gateway_buffer_writer& data,
                                        unsigned                        offset,
                                        uhd::rx_metadata_t&             md)
{
  // Extract number of samples.
  unsigned num_samples = data.get_nof_samples() - offset;

  // Ignore reception if it is not streaming.
  if (state != states::STREAMING) {
    nof_rxd_samples = num_samples;
    return true;
  }

  // Make sure the number of channels is equal.
  srsran_assert(data.get_nof_channels() == nof_channels, "Number of channels does not match.");

  // Flatten buffers.
  static_vector<void*, RADIO_MAX_NOF_CHANNELS> buffs_flat_ptr(nof_channels);
  for (unsigned channel = 0; channel != nof_channels; ++channel) {
    buffs_flat_ptr[channel] = (void*)data[channel].subspan(offset, num_samples).data();
  }

  uhd::rx_streamer::buffs_type buffs_cpp(buffs_flat_ptr.data(), nof_channels);

  // Protect the UHD Tx stream against concurrent access.
  std::unique_lock<std::mutex> lock(stream_mutex);

  return safe_execution([this, buffs_cpp, num_samples, &md, &nof_rxd_samples]() {
    nof_rxd_samples = stream->recv(buffs_cpp, num_samples, md, RECEIVE_TIMEOUT_S, ONE_PACKET);
  });
}

radio_uhd_rx_stream::radio_uhd_rx_stream(uhd::usrp::multi_usrp::sptr& usrp,
                                         const stream_description&    description,
                                         radio_notification_handler&  notifier_) :
  id(description.id), srate_Hz(description.srate_Hz), notifier(notifier_)
{
  srsran_assert(std::isnormal(srate_Hz) && (srate_Hz > 0.0), "Invalid sampling rate {}.", srate_Hz);

  // Build stream arguments.
  uhd::stream_args_t stream_args = {};
  stream_args.cpu_format         = "fc32";
  switch (description.otw_format) {
    case radio_configuration::over_the_wire_format::DEFAULT:
    case radio_configuration::over_the_wire_format::SC16:
      stream_args.otw_format = "sc16";
      break;
    case radio_configuration::over_the_wire_format::SC12:
      stream_args.otw_format = "sc12";
      break;
    case radio_configuration::over_the_wire_format::SC8:
      stream_args.otw_format = "sc8";
      break;
  }
  stream_args.args     = description.args;
  stream_args.channels = description.ports;

  if (!safe_execution([this, usrp, &stream_args]() {
        stream          = usrp->get_rx_stream(stream_args);
        max_packet_size = stream->get_max_num_samps();
        nof_channels    = stream->get_num_channels();
      })) {
    return;
  }

  state = states::SUCCESSFUL_INIT;
}

bool radio_uhd_rx_stream::start(const uhd::time_spec_t& time_spec)
{
  if (state != states::SUCCESSFUL_INIT) {
    return true;
  }

  // Protect the UHD Tx stream against concurrent access.
  std::unique_lock<std::mutex> lock(stream_mutex);

  if (!safe_execution([this, &time_spec]() {
        uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        stream_cmd.time_spec  = time_spec;
        stream_cmd.stream_now = (time_spec.get_real_secs() == uhd::time_spec_t());

        stream->issue_stream_cmd(stream_cmd);
      })) {
    printf("Error: failed to start receive stream %d. %s.", id, get_error_message().c_str());
  }

  // Transition to streaming state.
  state = states::STREAMING;

  return true;
}

baseband_gateway_receiver::metadata radio_uhd_rx_stream::receive(baseband_gateway_buffer_writer& buffs)
{
  baseband_gateway_receiver::metadata ret = {};
  uhd::rx_metadata_t                  md;
  unsigned                            nsamples            = buffs[0].size();
  unsigned                            rxd_samples_total   = 0;
  unsigned                            timeout_trial_count = 0;

  // Receive stream in multiple blocks.
  while (rxd_samples_total < nsamples) {
    unsigned rxd_samples = 0;
    if (!receive_block(rxd_samples, buffs, rxd_samples_total, md)) {
      printf("Error: failed receiving packet. %s.\n", get_error_message().c_str());
      return {};
    }

    // Save timespec for first block only if the last timestamp is unknown.
    if (rxd_samples_total == 0) {
      ret.ts = md.time_spec.to_ticks(srate_Hz);
    }

    // Increase the total amount of received samples.
    rxd_samples_total += rxd_samples;

    // Prepare notification event.
    radio_notification_handler::event_description event = {};
    event.stream_id                                     = id;
    event.channel_id                                    = radio_notification_handler::UNKNOWN_ID;
    event.source                                        = radio_notification_handler::event_source::RECEIVE;
    event.type                                          = radio_notification_handler::event_type::UNDEFINED;

    // Handle error.
    switch (md.error_code) {
      case uhd::rx_metadata_t::ERROR_CODE_TIMEOUT:
        ++timeout_trial_count;
        if (timeout_trial_count >= 10) {
          printf("Error: exceeded maximum number of timed out transmissions.\n");
          return ret;
        }
        break;
      case uhd::rx_metadata_t::ERROR_CODE_NONE:
        // Ignored.
        break;
      case uhd::rx_metadata_t::ERROR_CODE_LATE_COMMAND:
        event.type = radio_notification_handler::event_type::LATE;
        break;
      case uhd::rx_metadata_t::ERROR_CODE_OVERFLOW:
        event.type = radio_notification_handler::event_type::OVERFLOW;
        break;
      case uhd::rx_metadata_t::ERROR_CODE_BROKEN_CHAIN:
      case uhd::rx_metadata_t::ERROR_CODE_ALIGNMENT:
      case uhd::rx_metadata_t::ERROR_CODE_BAD_PACKET:
        printf("Error: unhandled error in Rx metadata %s.", md.strerror().c_str());
        return ret;
    }

    // Notify if the event type was set.
    if (event.type != radio_notification_handler::event_type::UNDEFINED) {
      notifier.on_radio_rt_event(event);
    }
  }

  // If it reaches here, there is no error.
  return ret;
}

bool radio_uhd_rx_stream::stop()
{
  // Transition state to stop before locking to prevent real time priority thread owning the lock constantly.
  state = states::STOP;

  // Protect the UHD Tx stream against concurrent access.
  std::unique_lock<std::mutex> lock(stream_mutex);

  // Try to stop the stream.
  if (!safe_execution([this]() {
        uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
        stream_cmd.time_spec  = uhd::time_spec_t();
        stream_cmd.stream_now = true;

        stream->issue_stream_cmd(stream_cmd);
      })) {
    printf("Error: failed to stop stream %d. %s.", id, get_error_message().c_str());
    return false;
  }

  return true;
}

void radio_uhd_rx_stream::wait_stop()
{
  // nothing to wait here
  return;
}

unsigned radio_uhd_rx_stream::get_buffer_size() const
{
  return max_packet_size;
}
