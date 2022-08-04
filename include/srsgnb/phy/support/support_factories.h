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

#include "srsgnb/phy/support/prach_buffer.h"
#include "srsgnb/phy/support/prach_buffer_pool.h"
#include "srsgnb/phy/support/resource_grid_pool.h"
#include <memory>
#include <vector>

namespace srsgnb {

/// \brief Creates a generic resource grid instance for a number of ports, symbols and subcarriers.
/// \param[in] nof_ports   Number of ports.
/// \param[in] nof_symbols Number of OFDM symbols.
/// \param[in] nof_subc    Number of subcarriers.
/// \return A resource grid object.
std::unique_ptr<resource_grid> create_resource_grid(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc);

/// \brief Creates a generic resource grid pool.
/// \param config Provides the configuration of the resource grid pool.
/// \return A generic resource grid pool.
std::unique_ptr<resource_grid_pool> create_resource_grid_pool(resource_grid_pool_config& config);

/// \brief Creates a long PRACH sequence buffer.
///
/// Long buffers contain 839-element PRACH sequences for up to 4 OFDM symbols.
std::unique_ptr<prach_buffer> create_prach_buffer_long();

/// \brief Creates a short PRACH sequence buffer.
///
/// Short buffers contain 139-element PRACH sequences for up to 12 OFDM symbols.
std::unique_ptr<prach_buffer> create_prach_buffer_short();

/// \brief Returns a pool for the given PRACH buffers.
std::unique_ptr<prach_buffer_pool> create_prach_buffer_pool(std::vector<std::unique_ptr<prach_buffer>>&& elements);

} // namespace srsgnb
