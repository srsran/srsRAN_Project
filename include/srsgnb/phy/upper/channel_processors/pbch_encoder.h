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
  static const unsigned PAYLOAD_SIZE = 24;
  static const unsigned A            = (PAYLOAD_SIZE + 8);
  static const unsigned CRC_LEN      = 24;
  static const unsigned B            = (A + CRC_LEN);
  static const unsigned K            = B;
  static const unsigned N            = 864;
  static const unsigned E            = N;
  static const unsigned POLAR_N_MAX  = 9;
  static const unsigned POLAR_N      = (1U << POLAR_N_MAX);

  /// Describes the NR PBCH message transmission
  struct pbch_msg_t {
    /// Physical cell identifier
    unsigned N_id;
    /// SSB candidate index in a 5ms burst
    unsigned ssb_idx;
    /// Maximum number if SS/PBCH block candidates in a 5ms burst, described in TS 38.213 section 4.1
    unsigned L_max;
    /// Indicates if the SS/PBCH block transmission is in an odd half frame true if subframe index is >=5
    bool hrf;
    /// Actual PBCH payload provided by higher layers
    std::array<uint8_t, A> payload;
    /// SFN
    unsigned sfn;
    /// Subcarrier offset described in TS 38.211 7.4.3.1
    unsigned k_ssb;
  };

  /// Default destructor
  virtual ~pbch_encoder() = default;

  /// \brief Encodes a PBCH message
  /// \param [in] pbch_msg
  /// \param [out] encoded
  virtual void encode(const pbch_msg_t& pbch_msg, span<uint8_t> encoded) = 0;
};

/// Creates a generic PBCH encoder instance
std::unique_ptr<pbch_encoder> create_pbch_encoder();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_PBCH_ENCODER_H
