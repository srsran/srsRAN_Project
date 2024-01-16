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

namespace srsran {

/// NR user plane protocol uplink message as described in TS 38.425.
struct nr_up_ul_message {
  //: TODO: add here dl data delivery status fields of 38.425
  byte_buffer pdu;
};

/// NR user plane protocol downlink message as described in TS 38.425.
struct nr_up_dl_message {
  //: TODO: add here dl user data fields of 38.425
  byte_buffer pdu;
};

} // namespace srsran
