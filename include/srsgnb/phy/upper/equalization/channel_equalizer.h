/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Channel equalizer interface.

#pragma once

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimation.h"

namespace srsgnb {

/// Describes the data structure containing the RE measurements.
struct re_measurement_dimensions {
  /// Number of contiguous PRBs.
  unsigned nof_prb = 0;
  /// Number of contiguous OFDM symbols.
  unsigned nof_symbols = 0;
  /// \brief Number of slices.
  unsigned nof_slices = 0;
};

/// \brief Container for RE measurements.
///
/// This class is used to store measurements/observations for all resource elements (REs) corresponding to a physical
/// channel. Therefore, they span a number of contiguous PRBs and a number of contiguous OFDM symbols, for all transmit
/// layers or, alternatively, for all receive antennas ports.
///
/// Examples of measurements/observations that may be stored in this container include modulated symbols and noise
/// variances.
///
/// \tparam measure_type The type of the represented RE measurement (e.g., \c float or \c cf_t).
template <typename measure_type>
class re_measurement
{
public:
  /// Default constructor.
  re_measurement() = default;

  /// Constructor: sets the size of the internal buffer.
  explicit re_measurement(const re_measurement_dimensions& dims) :
    nof_subcarriers(dims.nof_prb * NRE),
    nof_symbols(dims.nof_symbols),
    nof_slices(dims.nof_slices),
    slice_size(nof_subcarriers * nof_symbols)
  {
    srsgnb_assert(dims.nof_prb <= MAX_RB, "Requested {} RBs, but at most {} are allowed.", dims.nof_prb, MAX_RB);
    srsgnb_assert(dims.nof_symbols <= MAX_NSYMB_PER_SLOT,
                  "Requested {} OFDM symbols, but at most {} are allowed.",
                  dims.nof_symbols,
                  MAX_NSYMB_PER_SLOT);
    srsgnb_assert(dims.nof_slices <= MAX_NOF_SLICES,
                  "Requested {} slices, but at most {} are supported.",
                  dims.nof_slices,
                  static_cast<unsigned>(MAX_NOF_SLICES));

    // Resize the internal buffer.
    re_meas.resize(nof_slices * slice_size);
  }

  /// Default destructor
  ~re_measurement() = default;

  /// Returns a read-write view over the RE measurements corresponding to the given slice.
  span<measure_type> get_slice(unsigned i_slice)
  {
    auto first = re_meas.begin() + i_slice * slice_size;
    return span<measure_type>(first, slice_size);
  }

  /// Returns a read-only view over the RE measurements corresponding to the given slice.
  span<const measure_type> get_slice(unsigned i_slice) const
  {
    auto first = re_meas.begin() + i_slice * slice_size;
    return span<const measure_type>(first, slice_size);
  }

  /// \brief Writes data on a slice.
  ///
  /// \param[in] data     Input data.
  /// \param[in] i_slice  Index of the destination slice.
  /// \remark The size of \c data should be equal to the number of REs in a slice.
  void set_slice(span<measure_type> data, unsigned i_slice)
  {
    srsgnb_assert(i_slice <= nof_slices, "Requested slice {}, but only {} slices are supported.", i_slice, nof_slices);
    srsgnb_assert(data.size() == slice_size, "Slice size mismatch: given {}, current {}.", data.size(), slice_size);

    span<measure_type> slice = get_slice(i_slice);
    std::copy(data.begin(), data.end(), slice);
  }

  /// Updates the size of the internal buffer to the given dimensions.
  void resize(const re_measurement_dimensions& dims)
  {
    srsgnb_assert(dims.nof_prb <= MAX_RB, "Requested {} RBs, but at most {} are allowed.", dims.nof_prb, MAX_RB);
    srsgnb_assert(dims.nof_symbols <= MAX_NSYMB_PER_SLOT,
                  "Requested {} OFDM symbols, but at most {} are allowed.",
                  dims.nof_symbols,
                  MAX_NSYMB_PER_SLOT);
    srsgnb_assert(dims.nof_slices <= MAX_NOF_SLICES,
                  "Requested {} slices, but at most {} are supported.",
                  dims.nof_slices,
                  static_cast<unsigned>(MAX_NOF_SLICES));

    nof_subcarriers = dims.nof_prb * NRE;
    nof_symbols     = dims.nof_symbols;
    nof_slices      = dims.nof_slices;
    slice_size      = nof_subcarriers * nof_symbols;

    // Resize the internal buffer.
    re_meas.resize(nof_slices * slice_size);
  }

private:
  /// Number of subcarriers.
  unsigned nof_subcarriers = 0;

  /// Number of OFDM symbols.
  unsigned nof_symbols = 0;

  /// \name Slice specifications.
  ///
  /// A slice consists of a set of contiguous REs that spans \c nof_prb PRBs and \c nof_symbols OFDM symbols.
  /// Typically, for a PHY channel, it represents all the REs corresponding to a single Rx antenna port (before
  /// channel equalization) or to a single Tx layer (after channel equalization).
  ///@{
  /// Maximum number of slices.
  static constexpr unsigned MAX_NOF_SLICES = std::max(MAX_PORTS, pusch_constants::MAX_NOF_LAYERS);

  /// Number of slices.
  unsigned nof_slices = 0;

  /// \brief Slice size
  unsigned slice_size = 0;
  ///@}

  /// \brief Container for RE measurements.
  ///
  /// RE measurements should be thought as three-dimensional tensor with the first two dimensions representing, in
  /// order, subcarriers and OFDM symbols. Typically, the third dimension is referred to as slice and represents either
  /// receive ports (before channel equalization) or transmit layers (after channel equalization). The underlying data
  /// structure is a single vector, indexed in the same order: i) subcarriers, ii) OFDM symbols, iii) slice.
  static_vector<measure_type, MAX_RB * NRE * MAX_NSYMB_PER_SLOT * MAX_NOF_SLICES> re_meas;
};

/// Channel equalizer interface.
class channel_equalizer
{
public:
  /// Default destructor.
  virtual ~channel_equalizer() = default;

  /// \brief Equalizes the MIMO channel and combines Tx&ndash;Rx paths.
  ///
  /// For each transmit layer, the contributions of all receive ports are combined with weights obtained from the
  /// estimated channel coefficients. The variance of the point-to-point equivalent noise perturbing each modulated
  /// symbol is also estimated.
  ///
  /// \param[out] mod_symbols Estimated modulation symbols.
  /// \param[out] noise_vars  Estimated noise variances (after equalization).
  /// \param[in]  ch_symbols  Channel symbols, i.e., complex samples from the receive ports.
  /// \param[in]  ch_estimate Channel coefficients (typically provided by the channel estimator).
  virtual void equalize(re_measurement<cf_t>&       mod_symbols,
                        re_measurement<float>&      noise_vars,
                        const re_measurement<cf_t>& ch_symbols,
                        const channel_estimate&     ch_estimates);
};

} // namespace srsgnb