/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/support/srsran_assert.h"
#include <array>
#include <cmath>

namespace srsran {

/// Complex sinusoid table.
class complex_exponential_table
{
public:
  /// \brief Creates a complex exponential table.
  ///
  /// The table contains the sequence \f$x[n]=A\cdot e^{j2\pi\frac{n}{N}}\f$, \f$n \in \{0, 1, \dots, N-1\}\f$.
  ///
  /// \param[in] table_size_ Table size, parameter \f$N\f$.
  /// \param[in] amplitude   Complex exponential amplitude, parameter \f$A\f$.
  complex_exponential_table(unsigned table_size_, float amplitude) : table_size(table_size_), table(table_size_)
  {
    std::generate(table.begin(), table.end(), [this, amplitude, n = 0]() mutable {
      return std::polar(amplitude,
                        static_cast<float>(2 * M_PI) * static_cast<float>(n++) / static_cast<float>(table_size));
    });
  }

  /// Get the table size.
  std::size_t size() const { return table_size; }

  /// \brief Gets a coefficient from the table.
  /// \param[in] index Coefficient index.
  /// \remark An assertion is triggered if the index exceeds the table size.
  cf_t operator[](unsigned index) const
  {
    srsran_assert(index < table_size, "Index (i.e., {}) exceeds the table size (i.e., {})", index, table_size);
    return table[index];
  }

  /// \brief Generates a complex sinusoid from the table.
  ///
  /// \param[out] out       Output complex sinusoid: <tt>out[n] = x[(initial + n * frequency) % N]</tt>.
  /// \param[in]  initial   Initial argument.
  /// \param[in]  frequency Discrete angular frequency.
  void generate(span<cf_t> out, std::size_t initial, std::size_t frequency) const
  {
    std::generate(out.begin(), out.end(), [this, frequency, n = initial]() mutable {
      std::size_t temp = n;
      n += frequency;
      return table[temp % table_size];
    });
  }

  /// \brief Samples the table complex sinusoid at the given unsigned indices.
  /// \param[out] out Output sequence: <tt>out[n] = x[in[n] % N]</tt>, \f$in[n]\ge-N\f$
  /// \param[in]  in  Sampling indices.
  void generate(span<cf_t> out, span<const unsigned> in) const
  {
    srsran_assert(in.size() == out.size(), "The input and output sizes must be equal.");
    std::transform(in.begin(), in.end(), out.begin(), [this](unsigned index) { return table[index % table_size]; });
  }

  /// \brief Samples the table complex sinusoid at the given signed indices.
  /// \param[out] out Output sequence: <tt>out[n] = x[(in[n] + N) % N]</tt>.
  /// \param[in]  in  Sampling indices.
  void generate(span<cf_t> out, span<const int> in) const
  {
    srsran_assert(in.size() == out.size(), "The input and output sizes must be equal.");
    std::transform(
        in.begin(), in.end(), out.begin(), [this](int index) { return table[(table_size + index) % table_size]; });
  }

private:
  std::size_t       table_size;
  std::vector<cf_t> table;
};

} // namespace srsran
