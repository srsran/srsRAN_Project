#ifndef SRSGNB_PHY_PBCH_ENCODER_H
#define SRSGNB_PHY_PBCH_ENCODER_H

#include "srsgnb/phy/channel_coding/crc_calculator.h"
#include "srsgnb/phy/channel_coding/polar_allocator.h"
#include "srsgnb/phy/channel_coding/polar_encoder.h"
#include "srsgnb/phy/channel_coding/polar_interleaver.h"
#include "srsgnb/phy/channel_coding/polar_rate_matcher.h"
#include "srsgnb/phy/sequence_generators/pseudo_random_generator.h"
#include <array>
#include <memory>

namespace srsgnb {

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

  /**
   * @brief Describes the NR PBCH message transmission
   */
  struct pbch_msg_t {
    unsigned N_id;    ///< Physical cell identifier
    unsigned ssb_idx; ///< SSB candidate index
    unsigned Lmax;    ///< Number of SSB opportunities, described in TS 38.213 4.1 ...
    bool     hrf;     ///< Half Radio Frame bit

    // BCH payload
    std::array<uint8_t, A> payload; ///< Actual PBCH payload provided by higher layers
    unsigned               sfn;     ///< SFN
    unsigned               k_ssb;   ///< Subcarrier offset described in TS 38.211 7.4.3.1
  };

  virtual ~pbch_encoder() = default;

  /**
   * @brief Encodes a PBCH message
   * @param pbch_msg PBCH message to encode
   * @param ouput Encoded bits
   */
  virtual void encode(const pbch_msg_t& pbch_msg, std::array<uint8_t, E>& encoded) = 0;
};

std::unique_ptr<pbch_encoder> create_pbch_encoder();

} // namespace srsgnb

#endif // SRSGNB_PHY_PBCH_ENCODER_H
