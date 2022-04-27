#ifndef SRSGNB_PHY_UPPER_PBCH_ENCODER_H
#define SRSGNB_PHY_UPPER_PBCH_ENCODER_H

#include "srsgnb/adt/span.h"
#include <array>
#include <memory>

namespace srsgnb {

/// Describes the PBCH encoder interface
class pbch_encoder
{
public:
  /// BCH payload size.
  static const unsigned PAYLOAD_SIZE = 32;
  /// Generated payload length. TS 38.212 refers to it as \c A.
  static const unsigned A = 32;
  /// CRC length in bits.
  static const unsigned CRC_LEN = 24;
  /// Payload size plus CRC length.
  static const unsigned B = (PAYLOAD_SIZE + CRC_LEN);
  /// Number of encoded bits.
  static const unsigned E = 864;
  /// Polar encoder order logarithmic.
  static const unsigned POLAR_N_MAX = 9;
  /// Polar encoder order linear.
  static const unsigned POLAR_N = (1U << POLAR_N_MAX);

  /// Describes a PBCH message to encode.
  struct pbch_msg_t {
    /// Physical cell identifier.
    unsigned N_id;
    /// SSB candidate index in a 5ms burst.
    unsigned ssb_idx;
    /// Maximum number if SS/PBCH block candidates in a 5ms burst, described in TS 38.213 section 4.1.
    unsigned L_max;
    /// Indicates if the SS/PBCH block transmission is in an odd half frame true if subframe index is >=5.
    bool hrf;
    /// Actual PBCH payload provided by higher layers.
    std::array<uint8_t, A> payload;
    /// System Frame Number.
    unsigned sfn;
    /// Subcarrier offset described in TS 38.211 7.4.3.1.
    unsigned k_ssb;
  };

  /// Default destructor
  virtual ~pbch_encoder() = default;

  /// \brief Encodes a PBCH message.
  /// \param [out] encoded Encoded data.
  /// \param [in] pbch_msg PBCH message to encode.
  virtual void encode(span<uint8_t> encoded, const pbch_msg_t& pbch_msg) = 0;
};

/// Creates a generic PBCH encoder instance
std::unique_ptr<pbch_encoder> create_pbch_encoder();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_PBCH_ENCODER_H
