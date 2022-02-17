#ifndef SRSGNB_PHY_CHANNEL_CODING_POLAR_ENCODER_H
#define SRSGNB_PHY_CHANNEL_CODING_POLAR_ENCODER_H

#include "srsgnb/adt/span.h"
#include <cstdint>
#include <memory>

namespace srsgnb {

class polar_encoder
{
public:
  virtual ~polar_encoder() = default;

  /*!
   * Encodes the input vector into a codeword with the specified polar encoder.
   * \param[in] input The encoder input vector.
   * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the encoder input/output vector.
   *     It cannot be larger than the maximum code_size_log specified in q.code_size_log of
   *     the srsran_polar_encoder_t structure.
   * \param[out] output The encoder output vector.
   */
  virtual void encode(span<const uint8_t> input, unsigned code_size_log, span<uint8_t> output) = 0;
};

std::unique_ptr<polar_encoder> create_polar_encoder_pipelined(unsigned code_size_log);

} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_POLAR_ENCODER_H
