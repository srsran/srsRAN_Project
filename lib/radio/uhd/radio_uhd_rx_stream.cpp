/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "radio_uhd_rx_stream.h"

using namespace srsran;

bool radio_uhd_rx_stream::receive_block(unsigned&                nof_rxd_samples,
                                        baseband_gateway_buffer& data,
                                        unsigned                 offset,
                                        uhd::rx_metadata_t&      metadata)
{
  // Extract number of samples.
  unsigned num_samples = data.get_nof_samples() - offset;

  // Protect concurrent call of reception and stop.
  std::unique_lock<std::mutex> lock(stream_mutex);

  // Ignore reception if it is not streaming.
  if (state != states::STREAMING) {
    nof_rxd_samples = num_samples;
    return true;
  }

  // Make sure the number of channels is equal.
  report_fatal_error_if_not(data.get_nof_channels() == nof_channels, "Number of channels does not match.");

  // Flatten buffers.
  static_vector<void*, RADIO_MAX_NOF_CHANNELS> buffs_flat_ptr(nof_channels);
  for (unsigned channel = 0; channel != nof_channels; ++channel) {
    buffs_flat_ptr[channel] = (void*)data[channel].subspan(offset, num_samples).data();
  }

  uhd::rx_streamer::buffs_type buffs_cpp(buffs_flat_ptr.data(), nof_channels);

  return safe_execution([this, buffs_cpp, num_samples, &metadata, &nof_rxd_samples]() {
    nof_rxd_samples = stream->recv(buffs_cpp, num_samples, metadata, RECEIVE_TIMEOUT_S, ONE_PACKET);
  });
}

radio_uhd_rx_stream::radio_uhd_rx_stream(uhd::usrp::multi_usrp::sptr& usrp,
                                         const stream_description&    description,
                                         radio_notification_handler&  notifier_) :
  id(description.id), notifier(notifier_)
{
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

bool radio_uhd_rx_stream::receive(baseband_gateway_buffer& buffs, uhd::time_spec_t& time_spec)
{
  uhd::rx_metadata_t md;
  unsigned           nsamples            = buffs[0].size();
  unsigned           rxd_samples_total   = 0;
  unsigned           timeout_trial_count = 0;

  // Receive stream in multiple blocks
  while (rxd_samples_total < nsamples) {
    unsigned rxd_samples = 0;
    if (!receive_block(rxd_samples, buffs, rxd_samples_total, md)) {
      printf("Error: failed receiving packet. %s.\n", get_error_message().c_str());
      return false;
    }

    // Save timespec for first block.
    if (rxd_samples_total == 0) {
      time_spec = md.time_spec;
    }

    // Increment the total amount of received samples.
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
          return false;
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
        return false;
    }

    // Notify if the event type was set.
    if (event.type != radio_notification_handler::event_type::UNDEFINED) {
      notifier.on_radio_rt_event(event);
    }
  }

  // If it reaches here, there is no error.
  return true;
}

bool radio_uhd_rx_stream::stop()
{
  // Protect concurrent call of stop and reception.
  std::unique_lock<std::mutex> lock(stream_mutex);

  // Transition state to stop before locking to prevent real time priority thread owning the lock constantly.
  state = states::STOP;

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