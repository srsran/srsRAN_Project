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

#include "srsran/cu_cp/cu_cp_e1_handler.h"
#include "srsran/cu_cp/cu_cp_f1c_handler.h"
#include "srsran/cu_cp/cu_cp_metrics_handler.h"
#include "srsran/cu_cp/cu_cp_ng_handler.h"
#include "srsran/cu_cp/cu_cp_types.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Public interface for a CU-CP instance.
class cu_cp
{
public:
  virtual ~cu_cp() = default;

  /// \brief Get handler of the F1-C interface of the CU-CP.
  virtual cu_cp_f1c_handler& get_f1c_handler() = 0;

  /// \brief Get handler of the E1 interface of the CU-CP.
  virtual cu_cp_e1_handler& get_e1_handler() = 0;

  /// \brief Get handler of the NG interface of the CU-CP.
  virtual cu_cp_ng_handler& get_ng_handler() = 0;

  /// \brief Get the metrics handler interface of the CU-CP.
  virtual metrics_handler& get_metrics_handler() = 0;

  /// \brief Initiate AMF TNL connection and run NG Setup Procedure.
  ///
  /// This function blocks until the procedure is complete. Once completed, the CU-CP is in operational state.
  /// \return Returns true if the connection to the AMF and NG setup procedure were successful. False, otherwise.
  virtual bool start() = 0;

  /// \brief Stop the CU-CP operation.
  virtual void stop() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
