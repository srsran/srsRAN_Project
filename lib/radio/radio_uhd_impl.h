
#ifndef SRSGNB_LIB_RADIO_RADIO_UHD_IMPL_H
#define SRSGNB_LIB_RADIO_RADIO_UHD_IMPL_H

#include "srsgnb/radio/radio_management_plane.h"
#include "srsgnb/radio/radio_session.h"

namespace srsgnb {

/// Describes a UHD session.
class uhd_session : public radio_session,
                    private radio_management_plane,
                    private radio_data_plane,
                    private radio_data_plane_transmitter,
                    private radio_data_plane_receiver
{
private:
  radio_notifier& notifier;

  /// Indicates that the UHD session is succesfully constructed.
  bool valid = false;

public:
  uhd_session(const radio_configuration::radio& radio_config, radio_notifier& notifier_) : notifier(notifier_)
  {
    // TBD.

    valid = true;
  }

  bool is_valid() const { return valid; }

  // See interface for documentation
  radio_management_plane& get_management_plane() override { return *this; }

  // See interface for documentation
  radio_data_plane& get_data_plane() override { return *this; }

  // See interface for documentation
  void stop() override
  {
    // TBD.
  }

  // See interface for documentation
  radio_data_plane_transmitter& get_transmitter() override { return *this; }

  // See interface for documentation
  radio_data_plane_receiver& get_receiver() override { return *this; }

  // See interface for documentation
  void set_tx_gain(unsigned int port_id, double gain_dB) override
  {
    // TBD.
  }

  // See interface for documentation
  void set_rx_gain(unsigned int port_id, double gain_dB) override
  {
    // TBD.
  }

  // See interface for documentation
  void transmit(unsigned int                                                             stream_id,
                const radio_data_plane_transmitter::metadata&                            metadata,
                const static_vector<span<const radio_sample_type>, RADIO_MAX_NOF_PORTS>& data) override
  {}

  // See interface for documentation
  radio_data_plane_receiver::metadata receive(static_vector<span<radio_sample_type>, RADIO_MAX_NOF_PORTS>& data,
                                              unsigned int stream_id) override
  {
    return radio_data_plane_receiver::metadata();
  }
};

class radio_session_factory_uhd_impl : public radio_session_factory
{
public:
  // See interface for documentation
  std::unique_ptr<radio_session> create(const radio_configuration::radio& config, radio_notifier& notifier) override;
};

/// \brief Creates a UHD based radio session factory.
std::unique_ptr<radio_session_factory> create_radio_uhd_factory();

} // namespace srsgnb

#endif // SRSGNB_LIB_RADIO_RADIO_UHD_IMPL_H
