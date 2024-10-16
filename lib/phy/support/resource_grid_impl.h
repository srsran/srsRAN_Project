/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "resource_grid_mapper_impl.h"
#include "resource_grid_reader_impl.h"
#include "resource_grid_writer_impl.h"
#include "srsran/adt/tensor.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_dimensions.h"

namespace srsran {

class resource_grid_mapper;

/// Implements a resource grid.
class resource_grid_impl : public resource_grid
{
private:
  std::array<cbf16_t, NRE * MAX_RB> temp;
  std::atomic<unsigned>             empty = {};
  unsigned                          nof_ports;
  unsigned                          nof_symb;
  unsigned                          nof_subc;

  /// \brief Stores the resource grid data.
  ///
  /// The resource grid buffer is a three-dimensional array with the dimensions representing, in order, subcarriers,
  /// OFDM symbols and antenna ports.
  dynamic_tensor<static_cast<unsigned>(resource_grid_dimensions::all), cbf16_t, resource_grid_dimensions> rg_buffer;

  /// Resource grid writer implementation.
  resource_grid_writer_impl writer;

  /// Resource grid reader implementation.
  resource_grid_reader_impl reader;

public:
  /// \brief Creates a resource grid.
  ///
  /// \param[in] nof_ports Number of ports.
  /// \param[in] nof_symb  Number of OFDM symbols.
  /// \param[in] nof_subc  Number of subcarriers.
  resource_grid_impl(unsigned nof_ports, unsigned nof_symb, unsigned nof_subc);

  // See interface for documentation.
  void set_all_zero() override;

  // See interface for documentation.
  resource_grid_writer& get_writer() override;

  // See interface for documentation.
  const resource_grid_reader& get_reader() const override;
};

} // namespace srsran
