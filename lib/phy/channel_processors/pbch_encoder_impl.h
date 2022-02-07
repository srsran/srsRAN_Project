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
#ifndef SRSGNB_CHANNEL_PROCESSORS_PBCH_ENCODER_IMPL_H_
#define SRSGNB_CHANNEL_PROCESSORS_PBCH_ENCODER_IMPL_H_

#include "srsgnb/phy/channel_coding/crc_calculator.h"
#include "srsgnb/phy/channel_coding/polar_interleaver.h"
#include "srsgnb/phy/channel_processors/pbch_encoder.h"
#include "srsgnb/phy/sequence_generators/pseudo_random_generator.h"

namespace srsgnb {

class pbch_encoder_impl : public pbch_encoder
{
private:
  std::unique_ptr<polar_interleaver>       interleaver;
  std::unique_ptr<crc_calculator>          crc24c;
  std::unique_ptr<pseudo_random_generator> scrambler;

  /**
   * @brief Implements TS 38.312 section 7.1.1 PBCH payload generation
   * @param msg PBCH message
   * @param a Generated payload
   */
  void payload_generate(const srsgnb::pbch_encoder::pbch_msg_t& msg, std::array<uint8_t, A>& a);

  /**
   * @brief Implements TS 38.312 section 7.1.2 Scrambling
   * @param msg PBCH message
   * @param a Payload
   * @param a_prime Scrambled payload
   */
  void scramble(const srsgnb::pbch_encoder::pbch_msg_t& msg,
                const std::array<uint8_t, A>&           a,
                std::array<uint8_t, A>&                 a_prime);

  /**
   * @brief Implements TS 38.312 section 7.1.2 Scrambling
   * @param a_prime Payload after scrambling
   * @param k Data with CRC attached
   */
  void crc_attach(const std::array<uint8_t, A>& a_prime, std::array<uint8_t, K>& k);

public:
  struct args_t {
    std::unique_ptr<crc_calculator> crc24c;
  };

  explicit pbch_encoder_impl(args_t& args);

  ~pbch_encoder_impl() = default;

  void encode(const pbch_msg_t& pbch_msg, std::array<uint8_t, 864>& encoded) override;
};

} // namespace srsgnb

#endif // SRSGNB_CHANNEL_PROCESSORS_PBCH_ENCODER_IMPL_H_
