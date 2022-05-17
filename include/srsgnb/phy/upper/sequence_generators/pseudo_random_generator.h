/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_SEQUENCE_GENERATORS_PSEUDO_RANDOM_H
#define SRSGNB_PHY_UPPER_SEQUENCE_GENERATORS_PSEUDO_RANDOM_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include <memory>

namespace srsgnb {

/// Describes a pseudo-random sequence generator interface that implements TS 38.211 section 5.2.1
class pseudo_random_generator
{
public:
  /// Default destructor.
  virtual ~pseudo_random_generator() = default;

  /// \brief Describes the internal Pseudo random generator state
  struct state_s {
    unsigned x1;
    unsigned x2;
  };

  /// \brief Initiates the internal Pseudo random sequence with the given c_init
  ///
  /// \param c_init Initial sequence state
  virtual void init(unsigned c_init) = 0;

  /// \brief Initiates the internal Pseudo random sequence with a given internal X1 and X2 states
  ///
  /// \param [in] state Provides current X1 and X2 states
  virtual void init(const state_s& state) = 0;

  /// \brief Accessor to extract the current X1 and X2 states
  ///
  /// \return The current state
  virtual state_s get_state() const = 0;

  /// \brief Advanced the pseudo random sequence state without generating or applying any mask
  ///
  /// \param count The number of bits to advance
  virtual void advance(unsigned count) = 0;

  /// \brief Apply XOR sequence to a byte buffer.
  ///
  /// \param [in] in Provides the input data.
  /// \param [out] out Provides the ouput data destination.
  virtual void apply_xor_byte(span<const uint8_t> in, span<uint8_t> out) = 0;

  /// \brief Apply XOR sequence to a bit buffer.
  ///
  /// \param [in] in Provides the input data.
  /// \param [out] out Provides the ouput data destination.
  virtual void apply_xor_bit(span<const uint8_t> in, span<uint8_t> out) = 0;

  /// \brief Apply XOR sequence to a signed 8-bit buffer.
  ///
  /// \param [in] in Provides the input data.
  /// \param [out] out Provides the ouput data destination.
  virtual void apply_xor(span<const int8_t> in, span<int8_t> out) = 0;

  /// \brief Generate a floating point pseudo random sequence based on the internal state and the amplitude in \c
  /// value.
  ///
  /// Generates the pseudo-random sequence using the provided \c value as absolute value and the sequence is reflected
  /// in the sign.
  ///
  /// \param [out] buffer Provides the sequence destination.
  /// \param [in] value Provides the sequence amplitude.
  virtual void generate(span<float> buffer, float value) = 0;

  /// \brief Generate a complex floating point pseudo random sequence based on the internal state and the amplitude in
  /// \c value.
  ///
  /// Generates the pseudo-random sequence using the provided \c value as absolute value for the real and imaginary
  /// parts and the generated sequence is reflected in the sign.
  ///
  /// \param [out] buffer Provides the sequence destination.
  /// \param [in] value Provides the sequence amplitude.
  virtual void generate(span<cf_t> buffer, float value) { generate({(float*)buffer.data(), 2 * buffer.size()}, value); }
};

/// Creates a generic pseudo-random sequence generator
std::unique_ptr<pseudo_random_generator> create_pseudo_random();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SEQUENCE_GENERATORS_PSEUDO_RANDOM_H
