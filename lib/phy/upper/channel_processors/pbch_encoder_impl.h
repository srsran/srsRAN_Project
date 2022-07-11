/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PBCH_ENCODER_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PBCH_ENCODER_IMPL_H

#include "srsgnb/phy/upper/channel_coding/crc_calculator.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_allocator.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_code.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_encoder.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_interleaver.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_rate_matcher.h"
#include "srsgnb/phy/upper/channel_processors/pbch_encoder.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsgnb {

class pbch_encoder_impl : public pbch_encoder
{
private:
  std::unique_ptr<crc_calculator>          crc24c;
  std::unique_ptr<pseudo_random_generator> scrambler;
  std::unique_ptr<polar_interleaver>       interleaver;
  std::unique_ptr<polar_allocator>         alloc;
  std::unique_ptr<polar_code>              code;
  std::unique_ptr<polar_encoder>           encoder;
  std::unique_ptr<polar_rate_matcher>      rm;

  /// \brief Implements TS 38.312 section 7.1.1 PBCH payload generation.
  /// \param[out] a Generated payload.
  /// \param[in] msg PBCH message.
  void payload_generate(span<uint8_t> a, const srsgnb::pbch_encoder::pbch_msg_t& msg);

  /// \brief Implements TS 38.312 section 7.1.2 Scrambling.
  /// \param[out] a_prime Scrambled payload.
  /// \param[in] msg PBCH message.
  /// \param[in] a Payload.
  void scramble(span<uint8_t> a_prime, const srsgnb::pbch_encoder::pbch_msg_t& msg, span<const uint8_t> a);

  /// \brief Implements TS 38.312 section 7.1.3 Transport block CRC attachment.
  /// \param[in] b Data with CRC attached.
  /// \param[in] a_prime Payload after scrambling.
  void crc_attach(span<uint8_t> b, span<const uint8_t> a_prime);

  /// \brief Implements TS 38.312 section 7.1.4 Channel coding.
  /// \param[out] d Encoded data.
  /// \param[in] c Payload after scrambling.
  void channel_coding(span<uint8_t> d, span<const uint8_t> c);

  /// \brief Implements TS 38.312 section 7.1.5 Rate matching.
  /// \param[out] f Rate matched data.
  /// \param[in] d Encoded data.
  void rate_matching(span<uint8_t> f, span<const uint8_t> d);

public:
  /// Constructs a generic PBCH encoder.
  pbch_encoder_impl(std::unique_ptr<crc_calculator> crc24c_, std::unique_ptr<pseudo_random_generator> scrambler_) :
    crc24c(std::move(crc24c_)),
    scrambler(std::move(scrambler_)),
    interleaver(create_polar_interleaver()),
    alloc(create_polar_allocator()),
    code(create_polar_code()),
    encoder(create_polar_encoder_pipelined(POLAR_N_MAX_LOG)),
    rm(create_polar_rate_matcher())
  {
    srsran_assert(crc24c, "Invalid CRC24C.");
    srsran_assert(scrambler, "Invalid scrambler.");
    code->set(B, E, POLAR_N_MAX_LOG, polar_code_ibil::not_present);
  }

  // See interface for documentation.
  void encode(span<uint8_t> encoded, const pbch_msg_t& pbch_msg) override;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PBCH_ENCODER_IMPL_H
