/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/ntn.h"
#include <chrono>
#include <optional>
#include <variant>

namespace srsran {

namespace srs_ntn {

/// NTN Config update message to be received over a websocket interface.
struct ntn_config_update_info {
  using time_point = std::chrono::system_clock::time_point;
  nr_cell_global_id_t                                     nr_cgi;
  time_point                                              epoch_time;
  unsigned                                                ntn_ul_sync_validity_duration;
  std::variant<ecef_coordinates_t, orbital_coordinates_t> ephemeris_info;
  std::optional<ta_info_t>                                ta_info;
  std::optional<feeder_link_info_t>                       feeder_link_info;
  std::optional<geodetic_coordinates_t>                   ntn_gateway_location;
};

/// Public NTN configuration manager interface.
class ntn_configuration_manager
{
public:
  virtual ~ntn_configuration_manager() = default;

  /// \brief Handle an NTN configuration update request.
  ///
  /// This function processes an NTN configuration update, including generating timestamped SIB19 PDUs and setting the
  /// Doppler shift for PHY layer pre- and post-compensation for the feeder link.
  /// \param req Information required to perform the NTN configuration update.
  /// \return True if the update was successfully handled; false otherwise.
  virtual bool handle_ntn_config_update(const ntn_config_update_info& req) = 0;
};

} // namespace srs_ntn
} // namespace srsran
