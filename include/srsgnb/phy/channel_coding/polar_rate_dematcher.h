#ifndef SRSGNB_PHY_CHANNEL_CODING_POLAR_RATE_DEMATCHER_H
#define SRSGNB_PHY_CHANNEL_CODING_POLAR_RATE_DEMATCHER_H

#include "srsgnb/adt/bit_buffer.h"
#include <memory>

namespace srsgnb {

class polar_rate_dematcher
{
public:
  virtual ~polar_rate_dematcher() = default;

  /*!
   * Carries out the actual rate-dematching (int8_t inputs).
   * \param[in] input      The LLRs obtained from the channel samples that correspond to
   *                       the codeword to be first, rate-dematched and then decoded.
   * \param[out] output    The rate-dematched codeword resulting from the rate-dematching
   *                       operation.
   * \param[in] E          Rate-matched codeword length.
   * \param[in] n          \f$log_2\f$ of the codeword length.
   * \param[in] K          Message size (including CRC).
   * \return An integer: 0 if the function executes correctly, -1 otherwise.
   */
  virtual void rate_dematch(span<const int8_t> input, span<int8_t> output, const polar_code& code) = 0;
};

std::unique_ptr<polar_rate_dematcher> create_polar_rate_dematcher();

} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_POLAR_RATE_DEMATCHER_H