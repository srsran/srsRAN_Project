#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_RATE_MATCHER_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_RATE_MATCHER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_code.h"
#include <memory>
#include <stdint.h>

namespace srsgnb {

class polar_rate_matcher
{
public:
  virtual ~polar_rate_matcher() = default;

  /*!
   * Carries out the actual rate-matching.
   * \param[in] input        The codeword obtained from the polar encoder.
   * \param[out] output      The rate-matched codeword resulting from the rate-matching
   *                         operation.
   * \param[in] n            \f$log_2\f$ of the codeword length.
   * \param[in] E            Rate-matched codeword length.
   * \param[in] K            Message size (including CRC).
   * \return An integer: 0 if the function executes correctly, -1 otherwise.
   */
  virtual void rate_match(span<const uint8_t> input, span<uint8_t> output, const polar_code& code) = 0;
};

std::unique_ptr<polar_rate_matcher> create_polar_rate_matcher();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_RATE_MATCHER_H