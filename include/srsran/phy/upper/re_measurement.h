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

/// \file
/// \brief Resource Element (RE) measurements container.

#pragma once

#include "srsran/adt/detail/type_storage.h"
#include "srsran/adt/tensor.h"
#include "srsran/srsvec/copy.h"

namespace srsran {

/// Describes the data structure containing the RE measurements.
struct re_measurement_dimensions {
  /// Number of subcarriers.
  unsigned nof_subc = 0;
  /// Number of contiguous OFDM symbols.
  unsigned nof_symbols = 0;
  /// \brief Number of slices.
  ///
  /// A slice consists of a set of contiguous REs that spans \c nof_subc subcarriers and \c nof_symbols OFDM symbols.
  /// Typically, for a PHY channel, it represents all the REs corresponding to a single Rx antenna port (before
  /// channel equalization) or to a single Tx layer (after channel equalization).
  unsigned nof_slices = 0;

  /// \name Comparison operators.
  /// @{

  /// The RE dimensions are considered equal if the number of subcarriers, OFDM symbols and slices match.
  bool operator==(const re_measurement_dimensions& other) const
  {
    return (nof_subc == other.nof_subc) && (nof_symbols == other.nof_symbols) && (nof_slices == other.nof_slices);
  }
  bool operator!=(const re_measurement_dimensions& other) const { return !(*this == other); }
  /// @}
};

/// \brief Container interface for RE measurements.
///
/// \anchor slice0be287
/// This class is used to store measurements/observations for all resource elements (REs) corresponding to a physical
/// channel. More specifically, the container organizes the REs in \e slices: A slice includes all the REs seen by a
/// single receive antenna port (before channel equalization) or by a single transmit layer (after channel equalization)
/// and, hence, spans all allocated resource blocks and OFDM symbols.
///
/// Examples of measurements/observations that may be stored in this container include modulated symbols and noise
/// variances.
///
/// \tparam measure_type The type of the represented RE measurement (e.g., \c float or \c cf_t).
template <typename measure_type>
class re_measurement
{
public:
  /// Default destructor
  virtual ~re_measurement() = default;

  /// Returns the RE measurements dimensions.
  virtual const re_measurement_dimensions& size() const = 0;

  /// Returns a read-write view over the RE measurements corresponding to the given \ref slice0be287 "slice".
  virtual span<measure_type> get_slice(unsigned i_slice) = 0;

  /// Returns a read-only view over the RE measurements corresponding to the given \ref slice0be287 "slice".
  virtual span<const measure_type> get_slice(unsigned i_slice) const = 0;

  /// \brief Writes data on a \ref slice0be287 "slice".
  ///
  /// \param[in] data     Input data.
  /// \param[in] i_slice  Index of the destination slice.
  /// \remark The size of \c data should be equal to the number of REs in a slice.
  virtual void set_slice(span<const measure_type> data, unsigned i_slice) = 0;

  /// Returns a read-write view over the RE measurements corresponding to the given symbol index and \ref slice0be287
  /// "slice".
  virtual span<measure_type> get_symbol(unsigned i_symbol, unsigned i_slice) = 0;

  /// Returns a read-only view over the RE measurements corresponding to the given symbol index and \ref slice0be287
  /// "slice".
  virtual span<const measure_type> get_symbol(unsigned i_symbol, unsigned i_slice) const = 0;

  /// \brief Updates the size of the internal buffer to the given dimensions.
  /// \remark The amount of memory reserved for a re_measurement object depends on the implementation. This method only
  /// affects the amount of accessible REs and the \ref slice0be287 "slice" size.
  virtual void resize(const re_measurement_dimensions& dims) = 0;
};

namespace detail {

/// Number of dimensions that are contained for each indexing level.
enum re_measurement_nof_dimensions : unsigned { symbol = 1, slice = 2, all = 3 };

/// \brief Base container for RE measurements based on a tensor.
///
/// \tparam measure_type The type of the represented RE measurement (e.g., \c float or \c cf_t).
/// \tparam Tensor       The type of the tensor to instantiate.
template <typename measure_type, typename Tensor>
class re_measurement_tensor_base : public re_measurement<measure_type>
{
public:
  // See interface for documentation.
  const re_measurement_dimensions& size() const override { return dimensions; }

  // See interface for documentation.
  span<measure_type> get_slice(unsigned i_slice) override
  {
    srsran_assert(i_slice < dimensions.nof_slices,
                  "Requested slice {}, but only {} slices are supported.",
                  i_slice,
                  dimensions.nof_slices);
    return re_meas.template get_view<re_measurement_nof_dimensions::slice>({i_slice});
  }

  // See interface for documentation.
  span<const measure_type> get_slice(unsigned i_slice) const override
  {
    srsran_assert(i_slice < dimensions.nof_slices,
                  "Requested slice {}, but only {} slices are supported.",
                  i_slice,
                  dimensions.nof_slices);
    return re_meas.template get_view<re_measurement_nof_dimensions::slice>({i_slice});
  }

  // See interface for documentation.
  void set_slice(span<const measure_type> data, unsigned i_slice) override
  {
    srsran_assert(i_slice < dimensions.nof_slices,
                  "Requested slice {}, but only {} slices are supported.",
                  i_slice,
                  dimensions.nof_slices);
    srsran_assert(data.size() == dimensions.nof_subc * dimensions.nof_symbols,
                  "Slice size mismatch: given {}, current {}.",
                  data.size(),
                  dimensions.nof_subc * dimensions.nof_symbols);

    span<measure_type> slice = re_meas.template get_view<re_measurement_nof_dimensions::slice>({i_slice});
    srsvec::copy(slice, data);
  }

  // See interface for documentation.
  span<measure_type> get_symbol(unsigned i_symbol, unsigned i_slice) override
  {
    srsran_assert(i_symbol < dimensions.nof_symbols,
                  "Requested symbol {}, but only {} symbols are supported.",
                  i_slice,
                  dimensions.nof_symbols);
    srsran_assert(i_slice < dimensions.nof_slices,
                  "Requested slice {}, but only {} slices are supported.",
                  i_slice,
                  dimensions.nof_slices);
    return re_meas.template get_view<re_measurement_nof_dimensions::symbol>({i_symbol, i_slice});
  }

  // See interface for documentation.
  span<const measure_type> get_symbol(unsigned i_symbol, unsigned i_slice) const override
  {
    srsran_assert(i_symbol < dimensions.nof_symbols,
                  "Requested symbol {}, but only {} symbols are supported.",
                  i_slice,
                  dimensions.nof_symbols);
    srsran_assert(i_slice < dimensions.nof_slices,
                  "Requested slice {}, but only {} slices are supported.",
                  i_slice,
                  dimensions.nof_slices);
    return re_meas.template get_view<re_measurement_nof_dimensions::symbol>({i_symbol, i_slice});
  }

  // See interface for documentation.
  void resize(const re_measurement_dimensions& dims) override
  {
    srsran_assert(dims.nof_subc <= reserved_dims.nof_subc,
                  "Requested {} subcarriers, but at most {} are allowed.",
                  dims.nof_subc,
                  reserved_dims.nof_subc);
    srsran_assert(dims.nof_symbols <= reserved_dims.nof_symbols,
                  "Requested {} OFDM symbols, but at most {} are allowed.",
                  dims.nof_symbols,
                  reserved_dims.nof_symbols);
    srsran_assert(dims.nof_slices <= reserved_dims.nof_slices,
                  "Requested {} slices, but at most {} are supported.",
                  dims.nof_slices,
                  reserved_dims.nof_slices);

    dimensions = dims;
    re_meas.resize({dims.nof_subc, dims.nof_symbols, dims.nof_slices});
  }

protected:
  /// Maximum reserved dimensions.
  re_measurement_dimensions reserved_dims;

  /// \brief RE measurements container.
  ///
  /// RE measurements should be thought as three-dimensional tensor with the first two dimensions representing, in
  /// order, subcarriers and OFDM symbols. Typically, the third dimension is referred to as slice and represents either
  /// receive ports (before channel equalization) or transmit layers (after channel equalization). The underlying data
  /// structure is a single vector, indexed in the same order: i) subcarriers, ii) OFDM symbols, iii) slice.
  Tensor re_meas;

private:
  /// Current dimensions.
  re_measurement_dimensions dimensions;
};

} // namespace detail

/// \brief Static container for RE measurements.
///
/// The amount of memory reserved is fixed and set at compilation time.
///
/// \tparam measure_type The type of the represented RE measurement (e.g., \c float or \c cf_t).
/// \tparam MAX_NOF_SUBC    Maximum number of subcarriers.
/// \tparam MAX_NOF_SYMBOLS Maximum number of symbols.
/// \tparam MAX_NOF_SLICES  Maximum number of slices.
template <typename measure_type, unsigned MAX_NOF_SUBC, unsigned MAX_NOF_SYMBOLS, unsigned MAX_NOF_SLICES>
class static_re_measurement
  : public detail::re_measurement_tensor_base<measure_type,
                                              static_tensor<detail::re_measurement_nof_dimensions::all,
                                                            measure_type,
                                                            MAX_NOF_SUBC * MAX_NOF_SYMBOLS * MAX_NOF_SLICES>>
{
  using base = detail::re_measurement_tensor_base<measure_type,
                                                  static_tensor<detail::re_measurement_nof_dimensions::all,
                                                                measure_type,
                                                                MAX_NOF_SUBC * MAX_NOF_SYMBOLS * MAX_NOF_SLICES>>;

public:
  /// Constructor: checks template type.
  static_re_measurement()
  {
    static_assert(std::is_same<measure_type, float>::value || std::is_same<measure_type, cf_t>::value,
                  "At the moment, this template is only available for float and cf_t.");
    base::reserved_dims = {MAX_NOF_SUBC, MAX_NOF_SYMBOLS, MAX_NOF_SLICES};
  }

  /// Constructor: sets the size of the internal buffer.
  explicit static_re_measurement(const re_measurement_dimensions& dims) : static_re_measurement()
  {
    static_assert(std::is_same<measure_type, float>::value || std::is_same<measure_type, cf_t>::value,
                  "At the moment, this template is only available for float and cf_t.");
    base::resize(dims);
  }
};

/// \brief Dynamic container for RE measurements.
///
/// The amount of memory reserved is fixed and set at construction time.
///
/// \tparam measure_type The type of the represented RE measurement (e.g., \c float or \c cf_t).
/// \warning Instantiating an object of this class entails a heap memory allocation.
template <typename measure_type>
class dynamic_re_measurement
  : public detail::re_measurement_tensor_base<measure_type,
                                              dynamic_tensor<detail::re_measurement_nof_dimensions::all, measure_type>>
{
  using base =
      detail::re_measurement_tensor_base<measure_type,
                                         dynamic_tensor<detail::re_measurement_nof_dimensions::all, measure_type>>;

public:
  /// Constructor: checks template type and reserves internal memory.
  dynamic_re_measurement()
  {
    static_assert(std::is_same<measure_type, float>::value || std::is_same<measure_type, cf_t>::value,
                  "At the moment, this template is only available for float and cf_t.");
  }

  /// Constructor: sets the size of the internal buffer.
  explicit dynamic_re_measurement(const re_measurement_dimensions& dims) : dynamic_re_measurement() { reserve(dims); }

  /// \brief Reserve memory for the internal buffer and resize it to the given dimensions.
  /// \warning This method entails a heap memory allocation.
  void reserve(const re_measurement_dimensions& dims)
  {
    static_assert(std::is_same<measure_type, float>::value || std::is_same<measure_type, cf_t>::value,
                  "At the moment, this template is only available for float and cf_t.");

    base::reserved_dims = dims;
    base::re_meas.reserve({dims.nof_subc, dims.nof_symbols, dims.nof_slices});
  }
};

} // namespace srsran
