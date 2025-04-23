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

#include "srsran/ngap/ngap.h"
#include "srsran/ran/plmn_identity.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handler of the NG interface of the CU-CP.
///
/// This interface is used to forward NGAP messages or TNL connection updates to the CU-CP.
class cu_cp_ng_handler
{
public:
  virtual ~cu_cp_ng_handler() = default;

  /// \brief Get the NG message handler interface.
  /// \param[in] plmn The PLMN of the NGAP.
  /// \return A pointer to the NG message handler interface if it was found, nullptr otherwise.
  virtual ngap_message_handler* get_ngap_message_handler(const plmn_identity& plmn) = 0;

  /// \brief Get the state of the AMF connections.
  /// \return True if all AMFs are connected, false otherwise.
  virtual bool amfs_are_connected() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
