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

#ifndef SRSGNB_PHY_UPPER_SEQUENCE_GENERATORS_PSEUDO_RANDOM_H
#define SRSGNB_PHY_UPPER_SEQUENCE_GENERATORS_PSEUDO_RANDOM_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include <memory>

namespace srsgnb {

class pseudo_random_generator
{
public:
  virtual ~pseudo_random_generator() = default;

  /**
   * @brief Describes the internal Pseudo random generator state
   */
  struct state_s {
    unsigned x1;
    unsigned x2;
  };

  /**
   * @brief Initiates the internal Pseudo random sequence with the given c_init
   * @param c_init Initial sequence state
   */
  virtual void init(unsigned c_init) = 0;

  /**
   * @brief Initiates the internal Pseudo random sequence with a given internal X1 and X2 states
   * @param state provides current X1 and X2 states
   */
  virtual void init(const state_s& state) = 0;

  /**
   * @brief Accessor to extract the current X1 and X2 states
   * @return The current state
   */
  virtual state_s get_state() const = 0;

  /**
   * @brief Advanced the pseudo random sequence state without generating or applying any mask
   * @param count The number of bits to advance
   */
  virtual void advance(unsigned count) = 0;

  /**
   * @brief Apply XOR sequence to a byte buffer
   * @param in Input data
   * @param out Output data
   */
  virtual void apply_xor_byte(span<const uint8_t> in, span<uint8_t> out) = 0;

  /**
   * @brief Apply XOR sequence to a bit buffer
   * @param in Input data
   * @param out Output data
   */
  virtual void apply_xor_bit(span<const uint8_t> in, span<uint8_t> out) = 0;

  /**
   * @brief Apply XOR sequence to a signed 8-bit buffer
   * @param in Input data
   * @param out Output data
   */
  virtual void apply_xor(span<const int8_t> in, span<int8_t> out) = 0;

  /**
   * @brief Generate a pseudo random sequence based on the internal state
   * @param value
   * @param buffer
   */
  virtual void generate(float value, span<float> buffer) = 0;
};

std::unique_ptr<pseudo_random_generator> create_pseudo_random();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SEQUENCE_GENERATORS_PSEUDO_RANDOM_H
