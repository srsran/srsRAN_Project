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

#include "srsran/cu_cp/cu_cp.h"
#include "srsran/ngap/ngap_configuration.h"
#include <future>

namespace srsran {
namespace srs_cu_cp {

class cu_cp_routine_manager;
class cu_cp_ngap_control_notifier;
class ngap_cu_cp_connection_notifier;

class amf_connection_manager
{
public:
  amf_connection_manager(cu_cp_routine_manager&       routine_manager_,
                         const ngap_configuration&    ngap_cfg_,
                         cu_cp_ngap_control_notifier& ngap_ctrl_notif_);

  /// \brief Initiates the connection to the AMF.
  /// A promise is passed as a parameter to enable blocking synchronization between the completion of the scheduled
  /// async task and the caller side.
  void connect_to_amf(std::promise<bool>* completion_signal = nullptr);

  /// Checks whether the CU-CP is connected to the AMF.
  bool is_amf_connected() const { return amf_connected.load(std::memory_order_relaxed); }

private:
  void handle_connection_setup_result(bool success);

  cu_cp_routine_manager&       routine_manager;
  const ngap_configuration&    ngap_cfg;
  cu_cp_ngap_control_notifier& ngap_ctrl_notifier;

  std::atomic<bool> amf_connected{false};
};

} // namespace srs_cu_cp
} // namespace srsran