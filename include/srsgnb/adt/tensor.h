
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

#include "srsgnb/adt/span.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <array>
#include <numeric>
#include <vector>

namespace srsgnb {

/// \brief Base tensor interface.
/// \tparam NDIMS Number of dimensions.
/// \tparam Type  Type of data to store.
/// \tparam Index_type <c>enum class</c> representing the tensor dimensions.
template <unsigned NDIMS, typename Type, typename Index_type = unsigned>
class tensor
{
public:
  /// Describes the tensor dimensions.
  using dims = Index_type;
  /// Default destructor.
  virtual ~tensor() = default;

  /// \brief Gets a view of the lowest \c N dimensions.
  /// \tparam N          Number of dimensions to return.
  /// \param[in] indices Indices of each dimension except the \c N lowest ones.
  /// \return A view of the elements along the first dimension.
  template <unsigned N = 1>
  span<Type> get_view(const std::array<unsigned, NDIMS - N>& indices)
  {
    span<Type>                         data            = get_data();
    const std::array<unsigned, NDIMS>& dimensions_size = get_dimensions_size();

    // Calculate the view size for the first N dimensions.
    unsigned view_size = std::accumulate(dimensions_size.begin(), dimensions_size.begin() + N, 1, std::multiplies<>());

    return data.subspan(get_view_offset<N>(indices), view_size);
  }

  /// \brief Gets a read-only view of the lowest \c N dimensions.
  /// \tparam N          Number of dimensions to return.
  /// \param[in] indices Indices of each dimension except the \c N lowest ones.
  /// \return A read-only view of the elements along the first dimension.
  template <unsigned N = 1>
  span<const Type> get_view(const std::array<unsigned, NDIMS - N>& indices) const
  {
    span<const Type>                   data            = get_data();
    const std::array<unsigned, NDIMS>& dimensions_size = get_dimensions_size();

    // Calculate the view size for the first N dimensions.
    unsigned view_size = std::accumulate(dimensions_size.begin(), dimensions_size.begin() + N, 1, std::multiplies<>());

    return data.subspan(get_view_offset<N>(indices), view_size);
  }

  /// Gets the reference to an element.
  Type& operator[](const std::array<unsigned, NDIMS>& indices)
  {
    span<Type> data = get_data();
    return data[get_view_offset<0>(indices)];
  }

  /// Gets a read-only reference to an element.
  const Type& operator[](const std::array<unsigned, NDIMS>& indices) const
  {
    span<const Type> data = get_data();
    return data[get_view_offset<0>(indices)];
  }

  /// Gets the dimension sizes.
  virtual const std::array<unsigned, NDIMS>& get_dimensions_size() const = 0;

  /// Get a specific dimension size.
  /// \param[in] i_dimension tensor dimension.
  /// \return The size of the dimension specified by \c i_dimension.
  unsigned get_dimension_size(Index_type i_dimension) const
  {
    return get_dimensions_size()[static_cast<unsigned>(i_dimension)];
  }

  /// Resizes the tensor.
  virtual void resize(const std::array<unsigned, NDIMS>& dimensions) = 0;

protected:
  /// Gets the stored data.
  virtual span<Type> get_data() = 0;
  /// Gets the stored data for read-only purposes.
  virtual span<const Type> get_data() const = 0;

private:
  /// Gets the offset to the view of the first dimension.
  template <unsigned N>
  unsigned get_view_offset(const std::array<unsigned, NDIMS - N>& indices) const
  {
    // Get dimensions.
    const std::array<unsigned, NDIMS>& dimensions_size = get_dimensions_size();
    // Dimension initial stride.
    unsigned stride = std::accumulate(dimensions_size.begin(), dimensions_size.begin() + N, 1, std::multiplies<>());
    // Current offset of the dimension.
    unsigned offset = 0;
    // For each dimension, add its offset.
    for (unsigned dimension_index = 0; dimension_index != NDIMS - N; ++dimension_index) {
      srsgnb_assert(indices[dimension_index] < dimensions_size[dimension_index + N],
                    "Dimension {} index {} exceeds the maximum index {}.",
                    dimension_index + N,
                    indices[dimension_index],
                    dimensions_size[dimension_index + N]);
      offset += stride * indices[dimension_index];
      stride *= dimensions_size[dimension_index + N];
    }
    return offset;
  }
};

/// \brief Static tensor - the dimensions can be resized without allocating memory up to \c MAX_ELEMENTS elements.
/// \tparam NDIMS        Number of dimensions.
/// \tparam Type         Type of data to store.
/// \tparam MAX_ELEMENTS maximum number of elements.
/// \tparam Index_type <c>enum class</c> representing the tensor dimensions.
template <unsigned NDIMS, typename Type, unsigned MAX_ELEMENTS, typename Index_type = unsigned>
class static_tensor : public tensor<NDIMS, Type, Index_type>
{
public:
  using dims                 = Index_type;
  using dimensions_size_type = std::array<unsigned, NDIMS>;

  /// Default constructor with all dimensions and elements to zero.
  static_tensor() = default;

  /// Copy constructor.
  explicit static_tensor(const tensor<NDIMS, Type>& other) noexcept :
    dimensions_size(other.get_dimensions_size()), elements(other.get_data())
  {
    // Do nothing.
  }

  /// Move constructor.
  static_tensor(static_tensor&& other) noexcept :
    dimensions_size(std::move(other.dimensions_size)), elements(std::move(other.elements))
  {
    // Do nothing.
  }

  /// Resizes the tensor.
  void resize(const std::array<unsigned, NDIMS>& dimensions) override
  {
    dimensions_size     = dimensions;
    unsigned total_size = std::accumulate(begin(dimensions_size), end(dimensions_size), 1, std::multiplies<>());
    srsgnb_assert(total_size <= MAX_ELEMENTS,
                  "The total number of elements {} (dimensions: {}) exceeds the maximum number of elements {}.",
                  total_size,
                  span<const unsigned>(dimensions),
                  MAX_ELEMENTS);
  }

  // See interface for documentation.
  const std::array<unsigned, NDIMS>& get_dimensions_size() const override { return dimensions_size; }

protected:
  // See interface for documentation.
  span<Type> get_data() override { return elements; }

  // See interface for documentation.
  span<const Type> get_data() const override { return elements; }

private:
  /// Tensor actual dimensions.
  dimensions_size_type dimensions_size = {};
  /// Tensor actual storage.
  std::array<Type, MAX_ELEMENTS> elements = {};
};

/// \brief Dynamic tensor - the dimensions can be resized dynamically.
/// \tparam NDIMS        Number of dimensions.
/// \tparam Type         Type of data to store.
/// \tparam Index_type <c>enum class</c> representing the tensor dimensions.
template <unsigned NDIMS, typename Type, typename Index_type = unsigned>
class dynamic_tensor : public tensor<NDIMS, Type, Index_type>
{
public:
  using dims                 = Index_type;
  using dimensions_size_type = std::array<unsigned, NDIMS>;

  /// Default constructor with all dimensions and elements to zero.
  dynamic_tensor() = default;

  /// Constructs a tensor with initial dimensions.
  explicit dynamic_tensor(const dimensions_size_type& dimensions) { reserve(dimensions); }

  /// Copy constructor.
  explicit dynamic_tensor(const tensor<NDIMS, Type>& other) noexcept :
    dimensions_size(other.get_dimensions_size()), elements(other.get_data())
  {
    // Do nothing.
  }

  /// Move constructor.
  dynamic_tensor(dynamic_tensor&& other) noexcept :
    dimensions_size(std::move(other.dimensions_size)), elements(std::move(other.elements))
  {
    // Do nothing.
  }

  /// Reserves memory for the tensor.
  void reserve(const dimensions_size_type& dimensions)
  {
    dimensions_size     = dimensions;
    unsigned total_size = std::accumulate(begin(dimensions_size), end(dimensions_size), 1, std::multiplies<>());
    elements.resize(total_size);
  }

  /// Resizes the tensor. It resizes the maximum number of elements only if the total size number of elements is greater
  /// than the current one.
  void resize(const dimensions_size_type& dimensions) override
  {
    dimensions_size     = dimensions;
    unsigned total_size = std::accumulate(begin(dimensions_size), end(dimensions_size), 1, std::multiplies<>());
    if (total_size > elements.size()) {
      elements.resize(total_size);
    }
  }

  // See interface for documentation.
  const std::array<unsigned, NDIMS>& get_dimensions_size() const override { return dimensions_size; }

protected:
  // See interface for documentation.
  span<Type> get_data() override { return elements; }

  // See interface for documentation.
  span<const Type> get_data() const override { return elements; }

private:
  /// Tensor actual dimensions.
  dimensions_size_type dimensions_size = {};
  /// Tensor actual storage.
  std::vector<Type> elements = {};
};

} // namespace srsgnb
