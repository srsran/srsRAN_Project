/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Pseudo-random generator implementation declaration.

#pragma once

#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include <cstdint>
#include <memory>

namespace srsgnb {

/// SSE-optimized implementation of the pseudo-random generator.
class pseudo_random_generator_impl : public pseudo_random_generator
{
private:
  /// Length of the generator seed in bits.
  static constexpr unsigned SEQUENCE_SEED_LEN = 31;

  /// \brief Parameter \f$N_{\textup{C}}\f$, as defined in TS38.211 Section 5.2.1.
  ///
  /// Corresponds to the delay between the state sequences \f$x_1(n), x_2(n)\f$ and the output sequence \f$c(n) =
  /// x_1(n + N_{\textup{C}}) \oplus x_2(n + N_{\textup{C}})\f$.
  static constexpr unsigned SEQUENCE_NC = 1600;

  /// \name Parameters for parallel generation of the state sequences.
  ///
  /// The parallel generation of the state sequences \f$x_1(n)\f$ and \f$x_2(n)\f$, as defined in
  /// TS38.211 Section 5.2.1, exploits the fact that these sequences have a memory of 31 terms, but only the four
  /// oldest terms contribute to the the generation of the next one.
  ///@{
  /// Number of bits generated in parallel.
  static constexpr unsigned SEQUENCE_PAR_BITS = 24;
  /// Bitmask for the parallelized bits.
  static constexpr unsigned SEQUENCE_MASK = ((1U << SEQUENCE_PAR_BITS) - 1U);
  ///@}

  /// State sequence \f$x_1(n)\f$ initializer.
  class x1_init_s
  {
  private:
    /// Memory register for sequence \f$x_1(n)\f$.
    unsigned x1;

  public:
    /// Initializes the first 31 elements of \f$x_1(n)\f$ and advances to position \f$N_{\textup{C}}\f$.
    x1_init_s();
    /// Returns the \f$x_1(n)\f$ state register after initialization.
    unsigned get() const;
  };

  /// \brief State sequence \f$x_2(n)\f$ initializer.
  ///
  /// Similarly to x1_init_s, this class is used to initialize the sequence \f$x_2(n)\f$ and advance it until position
  /// \f$n = N_{\textup{C}}\f$. Here, however, the process is carried out simultaneously for all possible seeds of the
  /// form \f$c_{\textup{init}} = 2^k\f$ for \f$k=0,1,\dots,30\f$. The initial state corresponding to any other seed can
  /// easily be computed from these basic ones after noticing that the map sending a seed to the corresponding sequence
  /// defines a group isomorphism between the set of seeds and the set of sequences (both groups under bitwise XOR).
  /// That is, if
  /// \f{align*}
  /// c_{\textup{init}}^{(1)} &\mapsto x_2^{(1)}(n) &&\text{and} &
  /// c_{\textup{init}}^{(2)} &\mapsto x_2^{(2)}(n)
  /// \f}
  /// then
  /// \f[
  /// c_{\textup{init}}^{(1)} \oplus c_{\textup{init}}^{(2)} \mapsto x_2^{(1)}(n) \oplus x_2^{(2)}(n)
  /// \f]
  /// with the XOR operator acting bitwise between seeds and for all \f$n\f$ between sequences.
  class x2_init_s
  {
  private:
    /// Memory register for sequence \f$x_2(n)\f$, for all basic seeds.
    std::array<unsigned, SEQUENCE_SEED_LEN> x2;

  public:
    /// Initializes the first 31 elements of \f$x_2(n)\f$ and advances to position \f$N_{\textup{C}}\f$.
    x2_init_s();
    /// For the given seed, returns the \f$x_2(n)\f$ state register after initialization.
    unsigned get(unsigned c_init) const;
  };

  /// Sequence \f$x_1(n)\f$ initializer object.
  static const x1_init_s x1_init;
  /// Sequence \f$x_2(n)\f$ initializer object.
  static const x2_init_s x2_init;

  /// Memory register for sequence \f$x_1(n)\f$.
  uint32_t x1 = 0;
  /// Memory register for sequence \f$x_2(n)\f$.
  uint32_t x2 = 0;

  /// \brief Advances sequence \f$x_1(n)\f$ SEQUENCE_PAR_BITS steps simultaneously.
  /// \param[in] state Current 32-bit long state.
  /// \return New 32-bit long state.
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

  /// \brief Advances sequence \f$x_1(n)\f$ one steps.
  /// \param[in] state Current 32-bit long state.
  /// \return New 32-bit long state.
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

  /// \brief Advances sequence \f$x_2(n)\f$ SEQUENCE_PAR_BITS steps simultaneously.
  /// \param[in] state Current 32-bit long state.
  /// \return New 32-bit long state.
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

  /// \brief Advances sequence \f$x_2(n)\f$ one steps.
  /// \param[in] state Current 32-bit long state.
  /// \return New 32-bit long state.
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
  /// Default constructor.
  pseudo_random_generator_impl() = default;

  // See interface for the documentation.
  void init(unsigned c_init) override;

  // See interface for the documentation.
  void init(const state_s& c_init) override;

  // See interface for the documentation.
  state_s get_state() const override;

  // See interface for the documentation.
  void advance(unsigned count) override;

  // See interface for the documentation.
  void apply_xor_byte(span<uint8_t> out, span<const uint8_t> in) override;

  // See interface for the documentation.
  void apply_xor_bit(span<uint8_t> out, span<const uint8_t> in) override;

  // See interface for the documentation.
  void apply_xor(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) override;

  // See interface for the documentation.
  void generate(span<float> buffer, float value) override;
};
} // namespace srsgnb
