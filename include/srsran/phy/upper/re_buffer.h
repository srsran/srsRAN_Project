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
#include "srsran/adt/tensor.h"

namespace srsran {

namespace detail {
/// Base functions of all RE buffer interfaces.
class re_buffer_base
{
public:
  /// Default destructor.
  virtual ~re_buffer_base() = default;

  /// Gets the current number of slices.
  virtual unsigned get_nof_slices() const = 0;

  /// Gets the current number of Resource Elements.
  virtual unsigned get_nof_re() const = 0;

  /// Internal tensor dimensions.
  enum class dims : unsigned { re = 0, slice = 1, all = 2 };
};

} // namespace detail

/// \brief Resource element buffer.
///
/// It stores a number of resource elements for a given amount of slices. A slice can either be a transmit layer or an
/// antenna port, depending on the context.
class re_buffer : public detail::re_buffer_base
{
public:
  /// Default destructor.
  virtual ~re_buffer() = default;

  /// \brief Gets a read-only view of a specific slice.
  /// \param[in] i_slice     Slice identifier.
  /// \remark An assertion is triggered if the slice identifier is greater than or equal to get_nof_slices().
  /// \return A read-only view of the specified slice.
  virtual span<const cf_t> get_slice(unsigned i_slice) const = 0;

  /// \brief Gets a read-write view of a specific slice.
  /// \param[in] i_slice     Slice identifier.
  /// \remark An assertion is triggered if the slice identifier is greater than or equal to get_nof_slices().
  /// \return A read-write view of the specified slice.
  virtual span<cf_t> get_slice(unsigned i_slice) = 0;

  /// Gets a read-write view of the internal data.
  virtual tensor<static_cast<unsigned>(dims::all), cf_t, dims>& get_data() = 0;

  /// Gets a read-only view of the internal data.
  virtual const tensor<static_cast<unsigned>(dims::all), cf_t, dims>& get_data() const = 0;
};

/// Implements a read-only resource element buffer view.
class re_buffer_reader_view : public detail::re_buffer_base
{
public:
  /// Forbid default constructor;
  re_buffer_reader_view() = delete;

  /// \brief Constructs a resource element buffer view that gives access to a subspan of the REs for all slices.
  ///
  /// \param[in] buffer RE buffer accessed by the view.
  /// \param[in] offset Offset in number of RE. It determines the start of the RE view across each slice.
  /// \param[in] count Count in number of RE. It determines the number of RE spanned by the view across each slice.
  re_buffer_reader_view(const re_buffer& buffer, unsigned offset, unsigned count) :
    view_offset(offset), view_count(count), data(buffer.get_data())
  {
    srsran_assert((data.get_dimension_size(dims::re) >= offset + count),
                  "A view over REs [{}, {}) exceeds the RE tensor dimension size, i.e., {}.",
                  offset,
                  offset + count,
                  data.get_dimension_size(dims::re));
  }

  // See interface for documentation.
  unsigned get_nof_slices() const override { return data.get_dimension_size(dims::slice); }

  // See interface for documentation.
  unsigned get_nof_re() const override { return view_count; }

  /// \brief Gets a read-only view of a specific slice.
  ///
  /// The view spans the buffer REs in the range [\ref view_offset, \ref view_count). An assertion is triggered if the
  /// slice identifier is greater than or equal to get_nof_slices().
  ///
  /// \param[in] i_slice     Slice identifier.
  /// \return A read-only view of the specified slice.
  span<const cf_t> get_slice(unsigned i_slice) const
  {
    return data.get_view<1>({i_slice}).subspan(view_offset, view_count);
  }

private:
  /// Offset of the view across the Resource Elements.
  unsigned view_offset;
  /// Number of Resource Elements per slice included in the view.
  unsigned view_count;
  /// Internal data storage.
  const tensor<static_cast<unsigned>(dims::all), cf_t, dims>& data;
};

/// Implements a read-write resource element buffer view.
class re_buffer_writer_view : public detail::re_buffer_base
{
public:
  /// Forbid default constructor;
  re_buffer_writer_view() = delete;

  /// \brief Constructs a resource element buffer view that gives access to a subspan of the REs for all slices.
  ///
  /// \param[in] buffer RE buffer accessed by the view.
  /// \param[in] offset Offset in number of RE. It determines the start of the RE view across each slice.
  /// \param[in] count Count in number of RE. It determines the number of RE spanned by the view across each slice.
  re_buffer_writer_view(re_buffer& buffer, unsigned offset, unsigned count) :
    view_offset(offset), view_count(count), data(buffer.get_data())
  {
    srsran_assert((data.get_dimension_size(dims::re) >= offset + count),
                  "A view over REs [{}, {}) exceeds the RE tensor dimension size, i.e., {}.",
                  offset,
                  offset + count,
                  data.get_dimension_size(dims::re));
  }

  // See interface for documentation.
  unsigned get_nof_slices() const override { return data.get_dimension_size(dims::slice); }

  // See interface for documentation.
  unsigned get_nof_re() const override { return view_count; }

  /// \brief Gets a read-write view of a specific slice.
  ///
  /// The view spans the buffer REs in the range [\ref view_offset, \ref view_count). An assertion is triggered if the
  /// slice identifier is greater than or equal to get_nof_slices().
  ///
  /// \param[in] i_slice     Slice identifier.
  /// \return A read-write view of the specified slice.
  span<cf_t> get_slice(unsigned i_slice) { return data.get_view<1>({i_slice}).subspan(view_offset, view_count); }

private:
  /// Offset of the view across the Resource Elements.
  unsigned view_offset;
  /// Number of Resource Elements per slice included in the view.
  unsigned view_count;
  /// Internal data storage.
  tensor<static_cast<unsigned>(dims::all), cf_t, dims>& data;
};

/// Implements a dynamic resource element buffer.
class dynamic_re_buffer : public re_buffer
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
  span<cf_t> get_slice(unsigned i_slice) override { return data.get_view<1>({i_slice}); }

  // See interface for documentation.
  span<const cf_t> get_slice(unsigned i_slice) const override { return data.get_view<1>({i_slice}); }

  // See interface for documentation.
  tensor<static_cast<unsigned>(dims::all), cf_t, dims>& get_data() override { return data; }

  // See interface for documentation.
  const tensor<static_cast<unsigned>(dims::all), cf_t, dims>& get_data() const override { return data; }

private:
  /// Maximum number of slices.
  unsigned max_nof_slices;
  /// Maximum number of resource elements.
  unsigned max_nof_re;

  /// Internal data storage.
  dynamic_tensor<static_cast<unsigned>(dims::all), cf_t, dims> data;
};

/// Implements a static resource element buffer.
template <unsigned MaxNofSlices, unsigned MaxNofRe>
class static_re_buffer : public re_buffer
{
public:
  /// Default constructor - creates an empty resource element buffer.
  static_re_buffer() = default;

  /// \brief Creates a resource element buffer for a number of slices and resource elements.
  /// \param[in] nof_slices Number of slices.
  /// \param[in] nof_re     Number of resource elements.
  /// \remark An assertion is triggered if the number of slices exceeds \c MaxNofSlices.
  /// \remark An assertion is triggered if the number of resource elements exceeds \c MaxNofRe.
  static_re_buffer(unsigned nof_slices, unsigned nof_re) { resize(nof_slices, nof_re); }

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
  span<cf_t> get_slice(unsigned i_slice) override { return data.get_view({i_slice}); }

  // See interface for documentation.
  span<const cf_t> get_slice(unsigned i_slice) const override { return data.get_view({i_slice}); }

  // See interface for documentation.
  tensor<static_cast<unsigned>(dims::all), cf_t, dims>& get_data() override { return data; }

  // See interface for documentation.
  const tensor<static_cast<unsigned>(dims::all), cf_t, dims>& get_data() const override { return data; }

private:
  /// Internal data storage.
  static_tensor<static_cast<unsigned>(dims::all), cf_t, MaxNofSlices * MaxNofRe, dims> data;
};

} // namespace srsran
