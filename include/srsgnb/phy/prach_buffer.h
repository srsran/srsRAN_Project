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
#include "srsgnb/adt/span.h"

namespace srsgnb {

/// PRACH buffer interface.
class prach_buffer
{
public:
  /// Default destructor.
  virtual ~prach_buffer() = default;

  /// \brief Checks whether the buffer is empty (no samples have been written).
  /// \return True if the buffer is empty, false otherwise.
  virtual bool is_empty() const = 0;

  /// \brief Checks whether the buffer capacity has been reached.
  /// \return True if the buffer is full, false otherwise.
  virtual bool is_full() const = 0;

  /// \brief Gets the PRACH buffer samples.
  /// \return A read-only view of the stored samples.
  virtual span<const cf_t> get_samples() = 0;

  /// \brief Appends a sequence of baseband samples to the buffer.
  /// \param[in] samples A read-only view of the samples to be written into the buffer.
  virtual void write(span<const cf_t> samples) = 0;
};

} // namespace srsgnb
