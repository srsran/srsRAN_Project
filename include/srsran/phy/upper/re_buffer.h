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

/// \brief Resource element buffer.
///
/// It stores a number of resource elements for a given amount of layers.
///
class re_buffer
{
public:
  /// Default destructor.
  virtual ~re_buffer() = default;

  /// Gets the current number of layers.
  virtual unsigned get_nof_layers() const = 0;

  /// Gets the current number of resource elements.
  virtual unsigned get_nof_re() const = 0;

  /// \brief Gets a read-write view of a specific layer.
  /// \param[in] i_layer     Layer identifier.
  /// \remark An assertion is triggered if the layer identifier is greater than or equal to get_nof_layers().
  /// \return A read-write view of the specified layer.
  virtual span<cf_t> get_layer(unsigned i_layer) = 0;

  /// \brief Gets a read-only view of a specific layer.
  /// \param[in] i_layer     Layer identifier.
  /// \remark An assertion is triggered if the layer identifier is greater than or equal to get_nof_layers().
  /// \return A read-only view of the specified layer.
  virtual span<const cf_t> get_layer(unsigned i_layer) const = 0;
};

/// Implements a dynamic resource element buffer.
class dynamic_re_buffer : public re_buffer
{
public:
  /// \brief Constructs a resource element buffer for a given maximum number of layers and resource elements.
  /// \param[in] max_nof_layers_ Maximum number of layers.
  /// \param[in] max_nof_re_     Maximum number of resource elements.
  dynamic_re_buffer(unsigned max_nof_layers_, unsigned max_nof_re_) :
    max_nof_layers(max_nof_layers_), max_nof_re(max_nof_re_), data({max_nof_re, max_nof_layers})
  {
  }

  /// \brief Resizes the number of coefficients to a desired number of layers and ports.
  /// \param[in] nof_layers Number of layers.
  /// \param[in] nof_re     Number of resource elements.
  /// \remark An assertion is triggered if the number of layers exceeds \ref max_nof_layers.
  /// \remark An assertion is triggered if the number of resource elements exceeds \ref max_nof_re.
  void resize(unsigned nof_layers, unsigned nof_re)
  {
    srsran_assert(nof_layers <= max_nof_layers,
                  "The number of layers (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_layers,
                  max_nof_layers);
    srsran_assert(nof_re <= max_nof_re,
                  "The number of resource elements (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_re,
                  max_nof_re);
    data.resize({nof_re, nof_layers});
  }

  // See interface for documentation.
  unsigned get_nof_layers() const override { return data.get_dimension_size(dims::layer); }

  // See interface for documentation.
  unsigned get_nof_re() const override { return data.get_dimension_size(dims::re); }

  // See interface for documentation.
  span<cf_t> get_layer(unsigned i_layer) override { return data.get_view<1>({i_layer}); }

  // See interface for documentation.
  span<const cf_t> get_layer(unsigned i_layer) const override { return data.get_view<1>({i_layer}); }

private:
  /// Maximum number of layers.
  unsigned max_nof_layers;
  /// Maximum number of resource elements.
  unsigned max_nof_re;

  /// Internal tensor dimensions.
  enum class dims : unsigned { re = 0, layer = 1, all = 2 };

  /// Internal data storage.
  dynamic_tensor<static_cast<unsigned>(dims::all), cf_t, dims> data;
};

/// Implements a static resource element buffer.
template <unsigned MaxNofLayers, unsigned MaxNofRe>
class static_re_buffer : public re_buffer
{
public:
  /// Default constructor - creates an empty resource element buffer.
  static_re_buffer() = default;

  /// \brief Creates a resource element buffer for a number of layers and resource elements.
  /// \param[in] nof_layers Number of layers.
  /// \param[in] nof_re     Number of resource elements.
  /// \remark An assertion is triggered if the number of layers exceeds \c MaxNofLayers.
  /// \remark An assertion is triggered if the number of resource elements exceeds \c MaxNofRe.
  static_re_buffer(unsigned nof_layers, unsigned nof_re) { resize(nof_layers, nof_re); }

  /// \brief Resizes the number of coefficients to a desired number of layers and ports.
  /// \param[in] nof_layers Number of layers.
  /// \param[in] nof_re     Number of resource elements.
  /// \remark An assertion is triggered if the number of layers exceeds \c MaxNofLayers.
  /// \remark An assertion is triggered if the number of resource elements exceeds \c MaxNofRe.
  void resize(unsigned nof_layers, unsigned nof_re)
  {
    srsran_assert(nof_layers <= MaxNofLayers,
                  "The number of layers (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_layers,
                  MaxNofLayers);
    srsran_assert(nof_re <= MaxNofRe,
                  "The number of resource elements (i.e., {}) exceeds the maximum (i.e., {}).",
                  nof_re,
                  MaxNofRe);
    data.resize({nof_re, nof_layers});
  }

  // See interface for documentation.
  unsigned get_nof_layers() const override { return data.get_dimension_size(dims::layer); }

  // See interface for documentation.
  unsigned get_nof_re() const override { return data.get_dimension_size(dims::re); }

  // See interface for documentation.
  span<cf_t> get_layer(unsigned i_layer) override { return data.get_view({i_layer}); }

  // See interface for documentation.
  span<const cf_t> get_layer(unsigned i_layer) const override { return data.get_view({i_layer}); }

private:
  /// Internal tensor dimensions.
  enum class dims : unsigned { re = 0, layer = 1, all = 2 };

  /// Internal data storage.
  static_tensor<static_cast<unsigned>(dims::all), cf_t, MaxNofLayers * MaxNofRe, dims> data;
};

} // namespace srsran