#ifndef SRSGNB_PHY_PBCH_ENCODER_H
#define SRSGNB_PHY_PBCH_ENCODER_H

#include "srsgnb/adt/bit_buffer.h"
#include <array>

namespace srsgnb {

class pbch_encoder
{
public:
  /**
   * @brief Describes the NR PBCH message
   */
  struct pbch_msg_t {
    std::array<uint8_t, 32> payload;   ///< Actual PBCH payload provided by higher layers
    unsigned                sfn_4lsb;  ///< SFN 4 LSB
    unsigned                ssb_idx;   ///< SS/PBCH blocks index described in TS 38.213 4.1
    unsigned                k_ssb_msb; ///< Subcarrier offset MSB described in TS 38.211 7.4.3.1
    bool                    hrf;       ///< Half Radio Frame bit
  };

  virtual ~pbch_encoder() = default;

  /**
   * @brief Encodes a PBCH message
   * @param pbch_msg PBCH message to encode
   * @param ouput Encoded bits
   */
  virtual void encode(const pbch_msg_t& pbch_msg, std::array<uint8_t, 864>& encoded) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_PBCH_ENCODER_H
