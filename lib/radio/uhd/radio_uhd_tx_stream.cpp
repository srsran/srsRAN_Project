#include "radio_uhd_tx_stream.h"

using namespace srsgnb;

void radio_uhd_tx_stream::recv_async_msg()
{
  bool                  valid;
  uhd::async_metadata_t async_metadata;

  // Retrieve asynchronous message.
  if (!safe_execution([this, &valid, &async_metadata]() {
        valid = stream->recv_async_msg(async_metadata, RECV_ASYNC_MSG_TIMEOUT_S);
      })) {
    printf("Error: receiving asynchronous message for stream %d. %s.", stream_id, get_error_message().c_str());
    return;
  }

  // Skip if it is not valid.
  if (!valid) {
    return;
  }

  // Handle event.
  radio_notification_handler::event_description event_description = {};
  event_description.stream_id                         = stream_id;
  event_description.channel_id                        = async_metadata.channel;
  event_description.source                            = radio_notification_handler::event_source::TRANSMIT;
  event_description.type                              = radio_notification_handler::event_type::UNDEFINED;
  switch (async_metadata.event_code) {
    case uhd::async_metadata_t::EVENT_CODE_BURST_ACK:
      state_fsm.async_event_end_of_burst_ack();
      break;
    case uhd::async_metadata_t::EVENT_CODE_TIME_ERROR:
      event_description.type = radio_notification_handler::event_type::LATE;
      break;
    case uhd::async_metadata_t::EVENT_CODE_UNDERFLOW:
    case uhd::async_metadata_t::EVENT_CODE_UNDERFLOW_IN_PACKET:
      event_description.type = radio_notification_handler::event_type::UNDERFLOW;
      state_fsm.async_event_underflow(async_metadata.time_spec);
      break;
    case uhd::async_metadata_t::EVENT_CODE_SEQ_ERROR:
    case uhd::async_metadata_t::EVENT_CODE_SEQ_ERROR_IN_BURST:
    case uhd::async_metadata_t::EVENT_CODE_USER_PAYLOAD:
      event_description.type = radio_notification_handler::event_type::OTHER;
      break;
  }

  // Notify event if it is defined.
  if (event_description.type != radio_notification_handler::event_type::UNDEFINED) {
    notifier.on_radio_rt_event(event_description);
  }
}

void radio_uhd_tx_stream::run_recv_async_msg()
{
  // If stop was called, then stop enqueueing the task.
  if (state_fsm.is_stopped()) {
    return;
  }

  // Receive asynchronous message.
  recv_async_msg();

  // Enqueue the task again.
  async_executor.execute([this]() { run_recv_async_msg(); });
}

bool radio_uhd_tx_stream::transmit_block(unsigned&              nof_txd_samples,
                                         radio_baseband_buffer& buffs,
                                         unsigned               buffer_offset,
                                         uhd::time_spec_t&      time_spec)
{
  // Prepare metadata.
  uhd::tx_metadata_t metadata = {};

  // Extract number of samples.
  unsigned num_samples = buffs[0].size() - buffer_offset;

  // Make sure the number of channels is equal.
  srsran_always_assert(buffs.get_nof_channels() == nof_channels, "Number of channels does not match.");

  // Run states
  if (!state_fsm.transmit_block(metadata, time_spec)) {
    nof_txd_samples = num_samples;
    return true;
  }

  // Flatten buffers.
  static_vector<void*, RADIO_MAX_NOF_CHANNELS> buffs_flat_ptr(nof_channels);
  for (unsigned channel = 0; channel != nof_channels; ++channel) {
    buffs_flat_ptr[channel] = (void*)buffs[channel].subspan(buffer_offset, num_samples).data();
  }

  // Make UHD buffers.
  uhd::tx_streamer::buffs_type buffs_cpp(buffs_flat_ptr.data(), nof_channels);

  // Safe transmission.
  return safe_execution([this, &buffs_cpp, num_samples, &metadata, &nof_txd_samples]() {
    nof_txd_samples = stream->send(buffs_cpp, num_samples, metadata, TRANSMIT_TIMEOUT_S);
  });
}

radio_uhd_tx_stream::radio_uhd_tx_stream(uhd::usrp::multi_usrp::sptr& usrp,
                                         const stream_description&    description,
                                         task_executor&               async_executor_,
                                         radio_notification_handler&              notifier_) :
  stream_id(description.id),
  async_executor(async_executor_),
  notifier(notifier_),
  srate_hz(description.srate_hz),
  nof_channels(description.ports.size())
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

  Debug("Creating Tx stream " << stream_id);
  if (!safe_execution([this, usrp, &stream_args]() { stream = usrp->get_tx_stream(stream_args); })) {
    printf("Error:  failed to create transmit stream %d. %s.\n", stream_id, get_error_message().c_str());
    return;
  }

  // Notify FSM that it was successfully initialised.
  state_fsm.init_successful();

  // Create asynchronous task.
  run_recv_async_msg();
}

bool radio_uhd_tx_stream::transmit(radio_baseband_buffer& data, uhd::time_spec_t time_spec)
{
  // Protect stream transmitter.
  std::unique_lock<std::mutex> lock(stream_transmit_mutex);

  unsigned nsamples          = data.get_nof_samples();
  unsigned txd_samples_total = 0;

  // Receive stream in multiple blocks.
  while (txd_samples_total < nsamples) {
    unsigned txd_samples = 0;
    if (!transmit_block(txd_samples, data, txd_samples_total, time_spec)) {
      printf("Error: failed transmitting packet. %s.\n", get_error_message().c_str());
      return false;
    }

    // Save timespec for first block.
    time_spec += txd_samples * srate_hz;

    // Increment the total amount of received samples.
    txd_samples_total += txd_samples;
  }

  // If it reaches here, there is no error.
  return true;
}

void radio_uhd_tx_stream::stop()
{
  uhd::tx_metadata_t metadata;
  state_fsm.stop(metadata);

  // Send end-of-burst if it is in the middle of a burst.
  if (metadata.end_of_burst) {
    std::unique_lock<std::mutex> transmit_lock(stream_transmit_mutex);

    // Flatten buffers.
    std::array<radio_sample_type, 4>             buffer;
    static_vector<void*, RADIO_MAX_NOF_CHANNELS> buffs_flat_ptr(nof_channels);
    for (unsigned channel = 0; channel != nof_channels; ++channel) {
      buffs_flat_ptr[channel] = (void*)buffer.data();
    }

    // Make UHD buffers.
    uhd::tx_streamer::buffs_type buffs_cpp(buffs_flat_ptr.data(), nof_channels);

    // Safe transmission. Ignore return.
    safe_execution([this, &buffs_cpp, &metadata]() {
      // Actual transmission. Ignore number of transmitted samples.
      stream->send(buffs_cpp, 0, metadata, TRANSMIT_TIMEOUT_S);
    });
  }
}