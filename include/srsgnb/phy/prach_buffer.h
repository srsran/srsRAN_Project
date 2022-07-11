/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/complex.h"

namespace srsgnb {

/// Describes a PRACH buffer.
class prach_buffer
{
public:
  /// Default destructor.
  virtual ~prach_buffer() = default;

  /// \brief Indicates whether any sample has been written into the buffer.
  /// \return True if no sample is available in the buffer. Otherwise, false.
  virtual bool is_empty() const = 0;

  /// \brief Indicates whether the buffer capacity has been reach.
  /// \return True if the buffer is full with samples. Otherwise, false.
  virtual bool is_full() const = 0;

  /// \brief Get the PRACH buffer samples.
  /// \return A read-only view of the stored samples.
  virtual span<const cf_t> get_samples() = 0;

  /// \brief Appends a view of baseband samples into the buffer.
  /// \param[in] samples A read-only view of the samples to write in the buffer.
  virtual void write(span<const cf_t> samples) = 0;
};

} // namespace srsgnb
