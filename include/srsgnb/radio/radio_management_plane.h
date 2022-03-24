
#ifndef SRSGNB_RADIO_RADIO_MANAGEMENT_PLANE_H
#define SRSGNB_RADIO_RADIO_MANAGEMENT_PLANE_H

namespace srsgnb {

/// Defines a radio baseband unit management plane interface.
class radio_management_plane
{
public:
  /// Default destructor.
  virtual ~radio_management_plane() = default;

  /// \brief Set the transmission gain of a given port in runtime.
  /// \param[in] port_id Indicates the port identifier.
  /// \param[in] gain_dB Indicates the new gain.
  /// \note The port identifier is indexed from stream ascending order and then port.
  virtual void set_tx_gain(unsigned port_id, double gain_dB) = 0;

  /// \brief Set the reception gain of a given port in runtime.
  /// \param[in] port_id Indicates the port identifier.
  /// \param[in] gain_dB Indicates the new gain.
  /// \note The port identifier is indexed from stream ascending order and then port.
  virtual void set_rx_gain(unsigned port_id, double gain_dB) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_RADIO_RADIO_MANAGEMENT_PLANE_H
