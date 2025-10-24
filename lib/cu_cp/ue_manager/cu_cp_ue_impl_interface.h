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

#include "../ue_security_manager/ue_security_manager_impl.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/nrppa/nrppa.h"

namespace srsran {

namespace srs_cu_cp {

// Forward declarations.
class up_resource_manager;
class ue_task_scheduler;
class ue_security_manager;
class ngap_rrc_ue_notifier;

/// Common UE interface.
class cu_cp_ue_impl_interface
{
public:
  virtual ~cu_cp_ue_impl_interface() = default;

  /// \brief Get the UE index of the UE.
  virtual ue_index_t get_ue_index() const = 0;

  /// \brief Get the index of the DU where the UE is connected.
  virtual du_index_t get_du_index() const = 0;

  /// \brief Get the CU-UP index of the UE.
  virtual cu_up_index_t get_cu_up_index() const = 0;

  /// \brief Get the UP resource manager of the UE.
  virtual up_resource_manager& get_up_resource_manager() = 0;

  /// \brief Get the task scheduler of the UE.
  virtual ue_task_scheduler& get_task_sched() = 0;

  /// \brief Get the security manager of the UE.
  virtual ue_security_manager& get_security_manager() = 0;

  /// \brief Get the RRC UE notifier of the UE.
  virtual ngap_rrc_ue_notifier& get_ngap_rrc_ue_notifier() = 0;

  /// \brief Get the measurement results of the UE.
  virtual std::optional<cell_measurement_positioning_info>& get_measurement_results() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
