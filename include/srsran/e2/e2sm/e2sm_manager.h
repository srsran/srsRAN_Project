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
#include "srsran/e2/e2sm/e2sm.h"
#include <map>

namespace srsran {

class e2sm_manager
{
public:
  e2sm_manager(srslog::basic_logger& logger_);
  /// \brief Adds an e2sm service to the list of services that can be used to unpack e2sm messages.
  /// \param[in] oid The oid of the e2sm service.
  /// \param[in] e2sm_packer The interface that will be used to unpack the e2sm messages for this service
  void add_e2sm_service(std::string oid, std::unique_ptr<e2sm_interface> e2sm_iface);

  /// \brief Gets the e2sm service interface associated with the given oid.
  /// \param[in] oid The oid of the e2sm service.
  /// \return The e2sm service interface associated with the given oid.
  e2sm_interface* get_e2sm_interface(std::string oid);

  /// \brief  Gets the e2sm service interface associated with the given ran function id.
  /// \param[in] ran_function_id  The ran function id of the e2sm service.
  /// @return  The e2sm service interface associated with the given ran function id.
  e2sm_interface* get_e2sm_interface(uint16_t ran_function_id);

  /// \brief Adds a supported ran function to the list of supported ran functions.
  /// \param[in] ran_function_id The ran function id of the supported ran function.
  /// \param[in] oid The oid of the e2sm service that can be used to unpack the e2sm messages for this ran function.
  void add_supported_ran_function(uint16_t ran_function_id, std::string oid);

private:
  std::map<std::string, std::unique_ptr<e2sm_interface>> e2sm_iface_list;
  std::map<uint16_t, std::string>                        supported_ran_functions;
  srslog::basic_logger&                                  logger;
};
} // namespace srsran