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

#ifndef SRSGNB_LIB_PHY_SEQUENCE_PSEUDO_RANDOM_IMPL_H_
#define SRSGNB_LIB_PHY_SEQUENCE_PSEUDO_RANDOM_IMPL_H_

#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include <cstdint>
#include <memory>

namespace srsgnb {
class pseudo_random_generator_impl : public pseudo_random_generator
{
private:
  /**
   * Length of the seed, used for the feedback delay. Do not change.
   */
  static const unsigned SEQUENCE_SEED_LEN = 31;

  /**
   * Nc parameter defined in 3GPP. Do not change.
   */
  static const unsigned SEQUENCE_NC = 1600;

  /**
   * Parallel bit generation for x1/x2 sequences parameters. Exploits the fact that the sequence generation is 31 chips
   * ahead and the maximum register shift is 3 (for x2). The maximum number of parallel bits is 28, 16 is optimal for
   * SSE.
   */
  static const unsigned SEQUENCE_PAR_BITS = 24;
  static const unsigned SEQUENCE_MASK     = ((1U << SEQUENCE_PAR_BITS) - 1U);

  /**
   * Static precomputed x1 and x2 states after Nc shifts
   * -------------------------------------------------------
   *
   * The pre-computation of the Pseudo-Random sequences is based in their linearity properties.
   *
   * Having two seeds seed_1 and seed_2 generate x2_1 and x2_2 respectively:
   *     seed_1 -> x2_1
   *     seed_2 -> x2_2
   *
   * Then, the linearity property satisfies:
   *     seed_1 ^ seed_2 -> x2_1 ^ x2_2
   *
   * Because of this, a different x2 can be pre-computed for each bit of the seed.
   *
   */
  class x1_init_s
  {
  private:
    unsigned x1;

  public:
    x1_init_s();
    unsigned get() const;
  };

  struct x2_init_s {
  private:
    std::array<unsigned, SEQUENCE_SEED_LEN> x2;

  public:
    x2_init_s();
    unsigned get(unsigned c_init) const;
  };

  static const x1_init_s x1_init;
  static const x2_init_s x2_init;

  uint32_t x1;
  uint32_t x2;

  /**
   * Computes one step of the X1 sequence for SEQUENCE_PAR_BITS simultaneously
   * @param state 32 bit current state
   * @return new 32 bit state
   */
  static inline uint32_t step_par_x1(uint32_t state)
  {
    // Perform XOR
    uint32_t f = state ^ (state >> 3U);

    // Prepare feedback
    f = ((f & SEQUENCE_MASK) << (SEQUENCE_SEED_LEN - SEQUENCE_PAR_BITS));

    // Insert feedback
    state = (state >> SEQUENCE_PAR_BITS) ^ f;

    return state;
  }

  /**
   * Computes one step of the X1 sequence for 1bit
   * @param state 32 bit current state
   * @return new 32 bit state
   */
  static inline uint32_t step_x1(uint32_t state)
  {
    // Perform XOR
    uint32_t f = state ^ (state >> 3U);

    // Prepare feedback
    f = ((f & 1U) << (SEQUENCE_SEED_LEN - 1U));

    // Insert feedback
    state = (state >> 1U) ^ f;

    return state;
  }

  /**
   * Computes one step of the X2 sequence for SEQUENCE_PAR_BITS simultaneously
   * @param state 32 bit current state
   * @return new 32 bit state
   */
  static inline uint32_t step_par_x2(uint32_t state)
  {
    // Perform XOR
    uint32_t f = state ^ (state >> 1U) ^ (state >> 2U) ^ (state >> 3U);

    // Prepare feedback
    f = ((f & SEQUENCE_MASK) << (SEQUENCE_SEED_LEN - SEQUENCE_PAR_BITS));

    // Insert feedback
    state = (state >> SEQUENCE_PAR_BITS) ^ f;

    return state;
  }

  /**
   * Computes one step of the X2 sequence for 1bit
   * @param state 32 bit current state
   * @return new 32 bit state
   */
  static inline uint32_t step_x2(uint32_t state)
  {
    // Perform XOR
    uint32_t f = state ^ (state >> 1U) ^ (state >> 2U) ^ (state >> 3U);

    // Prepare feedback
    f = ((f & 1U) << (SEQUENCE_SEED_LEN - 1U));

    // Insert feedback
    state = (state >> 1U) ^ f;

    return state;
  }

public:
  pseudo_random_generator_impl()  = default;
  ~pseudo_random_generator_impl() = default;
  void    init(unsigned c_init) override;
  void    init(const state_s& c_init) override;
  state_s get_state() const override;
  void    advance(unsigned count) override;
  void    apply_xor_byte(span<const uint8_t> in, span<uint8_t> out) override;
  void    apply_xor_bit(span<const uint8_t> in, span<uint8_t> out) override;
  void    apply_xor(span<const int8_t> in, span<int8_t> out) override;
  void    generate(span<float> buffer, float value) override;
};
} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_SEQUENCE_PSEUDO_RANDOM_IMPL_H_
