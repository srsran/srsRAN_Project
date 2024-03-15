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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/support/srsran_assert.h"
#include <array>
#include <cmath>

namespace srsran {

/// Complex exponential table.
class complex_exponential_table
{
public:
  /// \brief Creates a complex exponential table.
  ///
  /// The table is filled as \f$x[n]=A\cdot e^{j2\pi\frac{n}{N}}\f$.
  ///
  /// \param table_size_ Table size, parameter \f$N\f$.
  /// \param scale       Complex exponential amplitude, parameter \f$A\f$.
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
  const cf_t& operator[](unsigned index) const
  {
    srsran_assert(index < table_size, "Index (i.e., {}) exceeds the table size (i.e., {})", index, table_size);
    return table[index];
  }

  /// \brief Generates a complex exponential.
  /// \param[out] out       Complex exponential destination.
  /// \param[in]  initial   Initial complex exponential argument.
  /// \param[in]  frequency Complex exponential discrete frequency.
  void generate(span<cf_t> out, std::size_t initial, std::size_t frequency) const
  {
    std::generate(out.begin(), out.end(), [this, frequency, n = initial]() mutable {
      std::size_t temp = n;
      n += frequency;
      return table[temp % table_size];
    });
  }

  /// \brief Generate complex exponential from a vector of unsigned indexes.
  /// \param[out] out Complex exponential destination.
  /// \param[in]  in  Complex exponential indexes.
  template <typename Integer>
  void generate(span<cf_t> out, span<const unsigned> in) const
  {
    srsran_assert(in.size() == out.size(), "The input and output sizes must be equal.");
    std::transform(in.begin(), in.end(), out.begin(), [this](Integer index) { return table[index % table_size]; });
  }

  /// \brief Generate complex exponential from a vector of signed indexes.
  /// \param[out] out Complex exponential destination.
  /// \param[in]  in  Complex exponential indexes.
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
