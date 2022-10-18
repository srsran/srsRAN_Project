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

#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/support/resource_grid.h"
#include <fstream>

namespace srsgnb {

/// \brief Prints the given resource grid to a file in binary format.
///
/// This function writes to a file the resource grid for the given port, for the given number of symbols and all the
/// carriers contained in the given number of PRBs.
/// The file is opened in truncated mode and writen in binary format.
///
/// \param[in] filename    Output file name (possibly, with path).
/// \param[in] grid        Resource grid to be printed.
/// \param[in] nof_symbols Number of OFDM symbols.
/// \param[in] nof_prbs    Number of PRBs int the resource grid.
/// \param[in] port        Port of the resource grid to write.
/// \return True on success, false otherwise.
inline bool
print(const char* filename, const resource_grid_reader& grid, unsigned nof_symbols, unsigned nof_prbs, unsigned port)
{
  std::ofstream file(filename, std::ios_base::binary | std::ios_base::trunc);

  if (!file.is_open()) {
    return false;
  }

  std::vector<cf_t> data;
  for (unsigned symbol_idx = 0; symbol_idx != nof_symbols; ++symbol_idx) {
    data.resize(NRE * nof_prbs);
    grid.get(data, port, symbol_idx, 0);
    file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(cf_t));
  }

  return true;
}

} // namespace srsgnb
