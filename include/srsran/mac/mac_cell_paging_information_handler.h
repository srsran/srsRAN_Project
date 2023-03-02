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

namespace srsran {

struct mac_paging_information {
  du_cell_index_t cell_index;
  // TODO: Populate other Paging information.
};

/// \brief Interface to handle paging information from the CU-CP.
class mac_cell_paging_information_handler
{
public:
  virtual ~mac_cell_paging_information_handler() = default;

  /// \brief Handles Paging information.
  virtual void handle_paging_information(const mac_paging_information& msg) = 0;
};

} // namespace srsran
