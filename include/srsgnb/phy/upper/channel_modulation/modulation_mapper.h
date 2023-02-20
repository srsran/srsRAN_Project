/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/bit_buffer.h"
#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/ran/modulation_scheme.h"

namespace srsgnb {

/// \brief Modulation mapper public interface.
///
/// Maps bits to complex symbols as specified in TS38.211 Section 5.1.
class modulation_mapper
{
public:
  /// Default destructor.
  virtual ~modulation_mapper() = default;

  /// \brief Maps a sequence of bits  to a sequence of complex symbols according to the given modulation scheme.
  /// \param[in]  input   The sequence of bits. Each entry corresponds to a single bit.
  /// \param[out] symbols The resulting sequence of complex-valued symbols.
  /// \param[in]  scheme  The modulation scheme determining the mapping.
  /// \remark The length of \c input should be equal to the length of \c symbols times the modulation order (that is,
  /// the number of bits per modulated symbol).
  virtual void modulate(span<cf_t> symbols, const bit_buffer& input, modulation_scheme scheme) = 0;
};

} // namespace srsgnb
