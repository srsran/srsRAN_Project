
#ifndef SRSGNB_LIB_RADIO_ZMQ_RADIO_ZMQ_RX_STREAM_H
#define SRSGNB_LIB_RADIO_ZMQ_RADIO_ZMQ_RX_STREAM_H

#include "radio_zmq_rx_channel.h"
#include <memory>

namespace srsgnb {

class radio_zmq_rx_stream
{
private:
  /// Indicates whether the class was initialized successfully.
  bool successful = false;
  /// Stores independent channels.
  std::vector<std::unique_ptr<radio_zmq_rx_channel> > channels;
  /// Counts the number of received samples.
  uint64_t sample_count = 0;

public:
  /// Describes the necessary parameters to create a ZMQ Tx stream.
  struct stream_description {
    /// Indicates the socket type.
    int socket_type;
    /// Indicates the addresses to bind. The number of elements indicate the number of channels.
    std::vector<std::string> address;
    /// Stream identifier.
    unsigned stream_id;
    /// Stream identifier string.
    std::string stream_id_str;
    /// Logging level.
    std::string log_level;
    /// Indicates the socket send and receive timeout in milliseconds. It is ignored if it is zero.
    unsigned trx_timeout_ms;
    /// Indicates the socket linger timeout in milliseconds. If is ignored if trx_timeout_ms is zero.
    unsigned linger_timeout_ms;
    /// Indicates the channel buffer size.
    unsigned buffer_size;
  };

  radio_zmq_rx_stream(void*                       zmq_context,
                      const stream_description&   config,
                      task_executor&              async_executor_,
                      radio_notification_handler& notification_handler);

  bool is_successful() const { return successful; }

  uint64_t get_sample_count() const { return sample_count; }

  void receive(baseband_gateway_buffer& data);

  void stop();

  void wait_stop();
};

} // namespace srsgnb
#endif // SRSGNB_LIB_RADIO_ZMQ_RADIO_ZMQ_RX_STREAM_H
