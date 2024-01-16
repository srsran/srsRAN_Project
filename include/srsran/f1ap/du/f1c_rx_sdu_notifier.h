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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"

namespace srsran {
namespace srs_du {

/// \brief This interface represents the data exit point of the receiving side of a F1-C bearer of the DU.
/// The F1AP will push F1-C SDUs (e.g. PDCP PDUs/RLC SDUs) to the lower layer (e.g. RLC) using this interface.
class f1c_rx_sdu_notifier
{
public:
  virtual ~f1c_rx_sdu_notifier() = default;

  virtual void on_new_sdu(byte_buffer pdu, optional<uint32_t> pdcp_sn) = 0;
};

} // namespace srs_du
} // namespace srsran
