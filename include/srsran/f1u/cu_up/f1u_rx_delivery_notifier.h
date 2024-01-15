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

#include <cstdint>

namespace srsran {
namespace srs_cu_up {

/// \brief This interface represents the notification exit point of the receiving side of a F1-U bearer of the CU-UP
/// through which the lower layer (e.g. RLC in the DU) notifies the PDCP Tx entity of transmit/delivery of PDCP PDUs.
class f1u_rx_delivery_notifier
{
public:
  virtual ~f1u_rx_delivery_notifier() = default;

  virtual void on_transmit_notification(uint32_t highest_pdcp_sn) = 0;
  virtual void on_delivery_notification(uint32_t highest_pdcp_sn) = 0;
};

} // namespace srs_cu_up
} // namespace srsran
