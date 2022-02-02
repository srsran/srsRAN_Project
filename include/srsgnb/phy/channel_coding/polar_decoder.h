#ifndef SRSGNB_PHY_CHANNEL_CODING_POLAR_DECODER_H
#define SRSGNB_PHY_CHANNEL_CODING_POLAR_DECODER_H

#include "srsgnb/adt/bit_buffer.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/channel_coding/polar_encoder.h"
#include <cstdint>
#include <memory>

namespace srsgnb {

class polar_decoder
{
public:
  /*!
   * Decodes the input (int8_t) codeword with the specified polar decoder.
   * \param[in] q A pointer to the desired polar decoder.
   * \param[in] input_llr The decoder LLR input vector.
   * \param[out] data_decoded The decoder output vector.
   * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the decoder input/output vector.
   * \param[in] frozen_set The position of the frozen bits in increasing order.
   * \param[in] frozen_set_size The size of the frozen_set.
   * \return An integer: 0 if the function executes correctly, -1 otherwise.
   */
  virtual void decode(span<const int8_t>   input_llr,
                      bit_buffer           data_decoded,
                      const uint8_t        code_size_log,
                      span<const uint16_t> frozen_set,
                      span<const uint16_t> frozen_set_size) = 0;
};

/*!
 * Initializes all the polar decoder variables according to fixed-point (8 bit) Simplified Successive Cancellation (SSC)
 * decoder algorithm and the maximum given code size.
 * \param[in] code_size_log The polar encoder implementation
 * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the decoder input/output vector.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
std::unique_ptr<polar_decoder> create_polar_decoder_ssc(std::unique_ptr<polar_encoder> enc, unsigned code_size_log);

} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_POLAR_DECODER_H