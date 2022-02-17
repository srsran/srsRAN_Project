/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CHANNEL_CODING_POLAR_ENCODER_IMPL_H_
#define SRSGNB_CHANNEL_CODING_POLAR_ENCODER_IMPL_H_

#include "srsgnb/phy/upper/channel_coding/polar_encoder.h"
#include "srsgnb/srsvec/aligned_vec.h"

namespace srsgnb {

class polar_encoder_impl : public polar_encoder
{
private:
  uint8_t max_code_size_log;   /*!< \brief The \f$ log_2\f$ of the maximum supported number of bits of the encoder
                            input/output vector. */
  uint16_t max_code_size;      /*!< \brief Number of bits of the encoder input/output vector. */
  uint16_t max_code_half_size; /*!< \brief Half of the number of bits of the encoder input/output vector. */
  srsvec::aligned_vec<uint16_t> i_even; /*!< \brief Pointer to the even positions of the encoder input/output vector. */
  srsvec::aligned_vec<uint16_t> i_odd;  /*!< \brief Pointer to the odd positions of the encoder input/output vector. */
  srsvec::aligned_vec<uint8_t>  tmp;    /*!< \brief Pointer to a temporary buffer. */

public:
  polar_encoder_impl(unsigned code_size_log);
  ~polar_encoder_impl() = default;
  void encode(span<const uint8_t> input, unsigned code_size_log, span<uint8_t> output) override;
};

} // namespace srsgnb
#endif // SRSGNB_CHANNEL_CODING_POLAR_ENCODER_IMPL_H_
