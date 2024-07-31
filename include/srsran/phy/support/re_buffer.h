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
#include "srsran/adt/tensor.h"

/// \file
/// \brief Interfaces and implementations of a Resource Element buffer.
///
/// The RE buffer stores a number of resource elements for a given amount of slices. A slice can either be a transmit
/// layer or an antenna port, depending on the context.

namespace srsran {

namespace detail {

/// Interface for accessing the dimension sizes of a Resource Element buffer.
class re_buffer_dimensions
{
public:
  /// Default destructor.
  virtual ~re_buffer_dimensions() = default;

  /// Gets the current number of slices.
  virtual unsigned get_nof_slices() const = 0;

  /// Gets the current number of Resource Elements.
  virtual unsigned get_nof_re() const = 0;
};

} // namespace detail

/// \brief Resource Element buffer read-only interface.
/// \tparam T Resource element type.
template <class T = cf_t>
class re_buffer_reader : public detail::re_buffer_dimensions
{
public:
  /// \brief Gets a read-only view of a specific slice.
  /// \param[in] i_slice     Slice identifier.
  /// \remark An assertion is triggered if the slice identifier is greater than or equal to get_nof_slices().
  /// \return A read-only view of the specified slice.
  virtual span<const T> get_slice(unsigned i_slice) const = 0;
};

/// \brief Resource Element buffer read-write interface.
/// \tparam T Resource element type.
template <typename T = cf_t>
class re_buffer_writer : public detail::re_buffer_dimensions
{
public:
  /// \brief Gets a read-write view of a specific slice.
  /// \param[in] i_slice     Slice identifier.
  /// \remark An assertion is triggered if the slice identifier is greater than or equal to get_nof_slices().
  /// \return A read-write view of the specified slice.
  virtual span<T> get_slice(unsigned i_slice) = 0;
};

/// \brief Implements a resource element buffer reader view, with read-only access methods.
/// \tparam T Resource element type.
template <typename T = cf_t>
class re_buffer_reader_view : public re_buffer_reader<T>
{
public:
  /// \brief Constructs a resource element buffer view that gives access to a subspan of the REs for all slices.
  ///
  /// \param[in] buffer RE buffer accessed by the view.
  /// \param[in] offset Offset in number of RE. It determines the start of the RE view across each slice.
  /// \param[in] count Count in number of RE. It determines the number of RE spanned by the view across each slice.
  re_buffer_reader_view(const re_buffer_reader<T>& buffer, unsigned offset, unsigned count) :
    view_offset(offset), view_count(count), data(buffer)
  {
    srsran_assert((buffer.get_nof_re() >= offset + count),
                  "A view over REs [{}, {}) exceeds the RE dimension size, i.e., {}.",
                  offset,
                  offset + count,
                  buffer.get_nof_re());
  }

  // See interface for documentation.
  unsigned get_nof_slices() const override { return data.get_nof_slices(); }

  // See interface for documentation.
  unsigned get_nof_re() const override { return view_count; }

  /// \brief Gets a read-only view of a specific slice.
  ///
  /// The view spans the buffer REs in the range [\ref view_offset, \ref view_count). An assertion is triggered if the
  /// slice identifier is greater than or equal to get_nof_slices().
  ///
  /// \param[in] i_slice     Slice identifier.
  /// \return A read-only view of the specified slice.
  span<const T> get_slice(unsigned i_slice) const override
  {
    return data.get_slice(i_slice).subspan(view_offset, view_count);
  }

private:
  /// Offset of the view across the Resource Elements.
  unsigned view_offset;
  /// Number of Resource Elements per slice included in the view.
  unsigned view_count;

  /// Internal data storage.
  const re_buffer_reader<T>& data;
};

/// \brief Implements a resource element buffer writer view, with read-write access methods.
/// \tparam T Resource element type.
template <typename T = cf_t>
class re_buffer_writer_view : public re_buffer_writer<T>
{
public:
  /// \brief Constructs a resource element buffer view that gives access to a subspan of the REs for all slices.
  ///
  /// \param[in] buffer RE buffer accessed by the view.
  /// \param[in] offset Offset in number of RE. It determines the start of the RE view across each slice.
  /// \param[in] count Count in number of RE. It determines the number of RE spanned by the view across each slice.
  re_buffer_writer_view(re_buffer_writer<T>& buffer, unsigned offset, unsigned count) :
    view_offset(offset), view_count(count), data(buffer)
  {
    srsran_assert((buffer.get_nof_re() >= offset + count),
                  "A view over REs [{}, {}) exceeds the RE dimension size, i.e., {}.",
                  offset,
                  offset + count,
                  buffer.get_nof_re());
  }

  /// \brief Gets a read-write view of a specific slice.
  ///
  /// The view spans the buffer REs in the range [\ref view_offset, \ref view_count). An assertion is triggered if the
  /// slice identifier is greater than or equal to get_nof_slices().
  ///
  /// \param[in] i_slice     Slice identifier.
  /// \return A read-write view of the specified slice.
  span<T> get_slice(unsigned i_slice) override { return data.get_slice(i_slice).subspan(view_offset, view_count); }

  // See interface for documentation.
  unsigned get_nof_slices() const override { return data.get_nof_slices(); }

  // See interface for documentation.
  unsigned get_nof_re() const override { return view_count; }

private:
  /// Offset of the view across the Resource Elements.
  unsigned view_offset;
  /// Number of Resource Elements per slice included in the view.
  unsigned view_count;

  /// Internal data storage.
  re_buffer_writer<T>& data;
};

/// \brief Implements a dynamic resource element buffer.
/// \tparam T Resource element type.
template <typename T = cf_t>
class dynamic_re_buffer : public re_buffer_reader<T>, public re_buffer_writer<T>
{
public:
  /// \brief Constructs a resource element buffer for a given maximum number of slices and resource elements.
  /// \param[in] max_nof_slices_ Maximum number of slices.
  /// \param[in] max_nof_re_     Maximum number of Resource Elements.
  dynamic_re_buffer(unsigned max_nof_slices_, unsigned max_nof_re_) :
    max_nof_slices(max_nof_slices_), max_nof_re(max_nof_re_), data({max_nof_re, max_nof_slices})
  {
  }

  /// \brief Resizes the buffer to a desired number of RE and slices.
  /// \param[in] nof_slices Number of slices.
  /// \param[in] nof_re     Number of resource elements.
  /// \remark An assertion is triggered if the number of slices exceeds \ref max_nof_slices.
  /// \remark An assertion is triggered if the number of resource elements exceeds \ref max_nof_re.
  void resize(unsigned nof_slices, unsigned nof_re)
  {
    srsran_assert(nof_slices <= max_nof_slices,
                  "The number of slices (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_slices,
                  max_nof_slices);
    srsran_assert(nof_re <= max_nof_re,
                  "The number of resource elements (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_re,
                  max_nof_re);
    data.resize({nof_re, nof_slices});
  }

  // See interface for documentation.
  unsigned get_nof_slices() const override { return data.get_dimension_size(dims::slice); }

  // See interface for documentation.
  unsigned get_nof_re() const override { return data.get_dimension_size(dims::re); }

  // See interface for documentation.
  span<T> get_slice(unsigned i_slice) override { return data.get_view({i_slice}); }

  // See interface for documentation.
  span<const T> get_slice(unsigned i_slice) const override { return data.get_view({i_slice}); }

private:
  /// Internal tensor dimensions.
  enum class dims : unsigned { re = 0, slice = 1, all = 2 };

  /// Maximum number of slices.
  unsigned max_nof_slices;
  /// Maximum number of resource elements.
  unsigned max_nof_re;

  /// Internal data storage.
  dynamic_tensor<static_cast<unsigned>(dims::all), T, dims> data;
};

/// \brief Implements a static resource element buffer.
/// \tparam MaxNofSlices Maximum number of slices.
/// \tparam MaxNofRe     Maximum number of resource elements.
/// \tparam T            Resource element type.
template <unsigned MaxNofSlices, unsigned MaxNofRe, typename T = cf_t>
class static_re_buffer : public re_buffer_reader<T>, public re_buffer_writer<T>
{
public:
  /// Default constructor - creates an empty resource element buffer.
  static_re_buffer() = default;

  /// \brief Creates a resource element buffer for a number of slices and resource elements.
  /// \param[in] nof_slices Number of slices.
  /// \param[in] nof_re     Number of resource elements.
  /// \remark An assertion is triggered if the number of slices exceeds \c MaxNofSlices.
  /// \remark An assertion is triggered if the number of resource elements exceeds \c MaxNofRe.
  static_re_buffer(unsigned nof_slices, unsigned nof_re) : data({nof_re, nof_slices}) {}

  /// \brief Resizes the number of coefficients to a desired number of slices and ports.
  /// \param[in] nof_slices Number of slices.
  /// \param[in] nof_re     Number of resource elements.
  /// \remark An assertion is triggered if the number of slices exceeds \c MaxNofSlices.
  /// \remark An assertion is triggered if the number of resource elements exceeds \c MaxNofRe.
  void resize(unsigned nof_slices, unsigned nof_re)
  {
    srsran_assert(nof_slices <= MaxNofSlices,
                  "The number of slices (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_slices,
                  MaxNofSlices);
    srsran_assert(nof_re <= MaxNofRe,
                  "The number of resource elements (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_re,
                  MaxNofRe);
    data.resize({nof_re, nof_slices});
  }

  // See interface for documentation.
  unsigned get_nof_slices() const override { return data.get_dimension_size(dims::slice); }

  // See interface for documentation.
  unsigned get_nof_re() const override { return data.get_dimension_size(dims::re); }

  // See interface for documentation.
  span<T> get_slice(unsigned i_slice) override { return data.get_view({i_slice}); }

  // See interface for documentation.
  span<const T> get_slice(unsigned i_slice) const override { return data.get_view({i_slice}); }

private:
  /// Internal tensor dimensions.
  enum class dims : unsigned { re = 0, slice = 1, all = 2 };

  /// Internal data storage.
  static_tensor<static_cast<unsigned>(dims::all), T, MaxNofSlices * MaxNofRe, dims> data;
};

/// \brief Implements a modular resource element buffer reader.
///
/// In this implementation, each slice is a view to an external block of contiguous REs that must be loaded with the
/// \ref set_slice method.
///
/// \tparam T Resource element type.
template <typename T, unsigned MaxNofSlices>
class modular_re_buffer_reader : public re_buffer_reader<T>
{
public:
  /// Constructs a modular resource element buffer.
  modular_re_buffer_reader() : nof_slices(0), nof_re(0) {}

  /// Constructs a modular resource element buffer for a given number of slices and resource elements.
  modular_re_buffer_reader(unsigned nof_slices_, unsigned nof_re_) { resize(nof_slices_, nof_re_); }

  /// \brief Resizes the buffer view to a desired number of RE and slices.
  /// \param[in] nof_slices Number of slices.
  /// \param[in] nof_re     Number of resource elements.
  /// \remark An assertion is triggered if the number of slices exceeds \ref max_nof_slices.
  void resize(unsigned nof_slices_, unsigned nof_re_)
  {
    nof_slices = nof_slices_;
    nof_re     = nof_re_;
    srsran_assert(nof_slices <= data.size(),
                  "The number of slices (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_slices,
                  data.size());

    // Empty all slices.
    std::fill_n(data.begin(), nof_slices, span<const T>());
  }

  /// \brief Sets the view for a given slice.
  /// \param[in] i_slice Slice identifier.
  /// \param[in] view    Slice view.
  /// \remark An assertion is triggered if the view size is not equal to the number of resource elements.
  void set_slice(unsigned i_slice, span<const T> view)
  {
    srsran_assert(view.size() == nof_re,
                  "The view size (i.e., {}) must be equal to the number of resource elements (i.e., {}).",
                  view.size(),
                  nof_re);
    data[i_slice] = view;
  }

  // See interface for documentation.
  unsigned get_nof_slices() const override { return nof_slices; }

  // See interface for documentation.
  unsigned get_nof_re() const override { return nof_re; }

  // See interface for documentation.
  span<const T> get_slice(unsigned i_slice) const override
  {
    srsran_assert(i_slice < nof_slices,
                  "The slice index (i.e., {}) exceeds the number of slices (i.e., {}).",
                  i_slice,
                  nof_slices);
    srsran_assert(!data[i_slice].empty(), "Data for slice {} is empty.", i_slice);
    return data[i_slice];
  }

private:
  /// Current number of slices.
  unsigned nof_slices;
  /// Current number of resource elements.
  unsigned nof_re;

  /// Internal data storage.
  std::array<span<const T>, MaxNofSlices> data;
};

/// \brief Implements a modular resource element buffer writer.
///
/// In this implementation, each slice is a view to an external block of contiguous REs that must be loaded with the
/// \ref set_slice method.
///
/// \tparam T            Resource element type.
/// \tparam MaxNofSlices Maximum number of slices.
template <typename T, unsigned MaxNofSlices>
class modular_re_buffer_writer : public re_buffer_writer<T>
{
public:
  /// \brief Resizes the buffer view to a desired number of RE and slices.
  /// \param[in] nof_slices Number of slices.
  /// \param[in] nof_re     Number of resource elements.
  /// \remark An assertion is triggered if the number of slices exceeds \ref max_nof_slices.
  void resize(unsigned nof_slices_, unsigned nof_re_)
  {
    nof_slices = nof_slices_;
    nof_re     = nof_re_;
    srsran_assert(nof_slices <= data.size(),
                  "The number of slices (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_slices,
                  data.size());

    // Empty all slices.
    std::fill_n(data.begin(), nof_slices, span<T>());
  }

  /// \brief Sets the view for a given slice.
  /// \param[in] i_slice Slice identifier.
  /// \param[in] view    Slice view.
  /// \remark An assertion is triggered if the view size is not equal to the number of resource elements.
  void set_slice(unsigned i_slice, span<T> view)
  {
    srsran_assert(view.size() == nof_re,
                  "The view size (i.e., {}) must be equal to the number of resource elements (i.e., {}).",
                  view.size(),
                  nof_re);
    data[i_slice] = view;
  }

  // See interface for documentation.
  unsigned get_nof_slices() const override { return nof_slices; }

  // See interface for documentation.
  unsigned get_nof_re() const override { return nof_re; }

  // See interface for documentation.
  span<T> get_slice(unsigned i_slice) override
  {
    srsran_assert(i_slice < nof_slices,
                  "The slice index (i.e., {}) exceeds the number of slices (i.e., {}).",
                  i_slice,
                  nof_slices);
    srsran_assert(!data[i_slice].empty(), "Data for slice {} is empty.", i_slice);
    return data[i_slice];
  }

private:
  /// Current number of slices.
  unsigned nof_slices;
  /// Current number of resource elements.
  unsigned nof_re;

  /// Internal data storage.
  std::array<span<T>, MaxNofSlices> data;
};

} // namespace srsran
