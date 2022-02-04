#ifndef SRSGNB_PHY_CHANNEL_CODING_POLAR_RATE_MATCHER_H
#define SRSGNB_PHY_CHANNEL_CODING_POLAR_RATE_MATCHER_H

#include "srsgnb/adt/bit_buffer.h"
#include <memory>

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
   * \param[in] ibil         Indicator of bit interleaver (set to 0 to disable).
   * \return An integer: 0 if the function executes correctly, -1 otherwise.
   */
  virtual void rate_match(const bit_buffer& input, bit_buffer& output, const polar_code& code, unsigned ibil) = 0;
};

std::unique_ptr<polar_rate_matcher> create_polar_rate_matcher();

} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_POLAR_RATE_MATCHER_H